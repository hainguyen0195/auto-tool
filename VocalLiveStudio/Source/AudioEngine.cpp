#include "AudioEngine.h"
#include <cmath>
#include <algorithm>

namespace VocalStudio
{

AudioEngine::AudioEngine()
{
    // Apply default Bolero Warm preset initially
    if (!presetManager.getFactoryPresets().empty())
    {
        presetManager.applyPreset(presetManager.getFactoryPresets()[0], autoTune, effects);
    }
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialize(int numInputChannels, int numOutputChannels)
{
    deviceManager.addChangeListener(this);

    // Initialise audio device manager (1 in, 2 out default)
    auto error = deviceManager.initialise(numInputChannels, numOutputChannels, nullptr, true);
    if (error.isNotEmpty())
    {
        std::lock_guard<std::mutex> lock(errorMutex);
        lastErrorMessage = error;
        // Even if initial device setup had a warning, continue with defaults
    }

    deviceManager.addAudioCallback(this);
    return true;
}

void AudioEngine::shutdown()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.removeChangeListener(this);
    deviceManager.closeAudioDevice();
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    // Audio device settings changed
    auto* currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        currentSampleRate = currentDevice->getCurrentSampleRate();
        currentBlockSize = currentDevice->getCurrentBufferSizeSamples();
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device == nullptr)
        return;

    currentSampleRate = device->getCurrentSampleRate();
    currentBlockSize = device->getCurrentBufferSizeSamples();

    int maxBufferSamples = std::max(2048, currentBlockSize * 2);

    vocalBuffer.setSize(2, maxBufferSamples);
    backingBuffer.setSize(2, maxBufferSamples);
    mixBuffer.setSize(2, maxBufferSamples);

    vocalBuffer.clear();
    backingBuffer.clear();
    mixBuffer.clear();

    autoTune.prepare(currentSampleRate, currentBlockSize, 2);
    effects.prepare(currentSampleRate, currentBlockSize, 2);
    karaokePlayer.prepareToPlay(currentSampleRate, currentBlockSize);
}

void AudioEngine::audioDeviceStopped()
{
    karaokePlayer.releaseResources();
    vocalBuffer.setSize(0, 0);
    backingBuffer.setSize(0, 0);
    mixBuffer.setSize(0, 0);
}

void AudioEngine::audioDeviceError(const juce::String& errorMessage)
{
    std::lock_guard<std::mutex> lock(errorMutex);
    lastErrorMessage = errorMessage;
}

juce::String AudioEngine::getLastError() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(errorMutex));
    return lastErrorMessage;
}

void AudioEngine::setMicGain(float linearGain) noexcept
{
    micGain.store(juce::jlimit(0.0f, 4.0f, linearGain), std::memory_order_relaxed);
}

void AudioEngine::setMasterVolume(float linearVolume) noexcept
{
    masterVolume.store(juce::jlimit(0.0f, 2.0f, linearVolume), std::memory_order_relaxed);
}

void AudioEngine::setInputMonitoringEnabled(bool enabled) noexcept
{
    inputMonitoring.store(enabled, std::memory_order_relaxed);
}

void AudioEngine::setSafeModeEnabled(bool enabled) noexcept
{
    safeMode.store(enabled, std::memory_order_relaxed);
}

void AudioEngine::updatePeakMeter(std::atomic<float>& meterDb, float peakLinear, float decayFactor)
{
    float currentDb = meterDb.load(std::memory_order_relaxed);
    float newDb = (peakLinear > 1e-5f) ? (20.0f * std::log10(peakLinear)) : -100.0f;

    if (newDb > currentDb)
    {
        // Instant attack
        meterDb.store(newDb, std::memory_order_relaxed);
    }
    else
    {
        // Smooth exponential decay
        meterDb.store(currentDb * decayFactor + newDb * (1.0f - decayFactor), std::memory_order_relaxed);
    }
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                  int numInputChannels,
                                                  float* const* outputChannelData,
                                                  int numOutputChannels,
                                                  int numSamples,
                                                  const juce::AudioIODeviceCallbackContext& /*context*/)
{
    if (outputChannelData == nullptr || numOutputChannels <= 0 || numSamples <= 0)
        return;

    // Zero out preallocated buffers
    vocalBuffer.clear(0, numSamples);
    backingBuffer.clear(0, numSamples);
    mixBuffer.clear(0, numSamples);

    float currentGain = micGain.load(std::memory_order_relaxed);
    float inPeakLinear = 0.0f;

    // 1. Process Microphone Input
    if (inputChannelData != nullptr && numInputChannels > 0)
    {
        const float* micInL = inputChannelData[0];
        const float* micInR = (numInputChannels > 1) ? inputChannelData[1] : micInL;

        float* vocalL = vocalBuffer.getWritePointer(0);
        float* vocalR = vocalBuffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            float sL = micInL[i] * currentGain;
            float sR = micInR[i] * currentGain;

            vocalL[i] = sL;
            vocalR[i] = sR;

            float absVal = std::max(std::abs(sL), std::abs(sR));
            if (absVal > inPeakLinear)
                inPeakLinear = absVal;
        }
    }

    updatePeakMeter(micInputPeakDb, inPeakLinear, 0.92f);

    bool monitor = inputMonitoring.load(std::memory_order_relaxed);
    if (monitor)
    {
        // Auto-Tune processing
        autoTune.process(vocalBuffer.getArrayOfWritePointers(), 2, numSamples);

        // Vocal Effects processing (Gate, De-Esser, EQ, Comp, Delay, Reverb, Limiter)
        effects.process(vocalBuffer.getArrayOfWritePointers(), 2, numSamples);
    }
    else
    {
        // Monitoring is off: mic is silent in output monitor
        vocalBuffer.clear(0, numSamples);
    }

    // 2. Process Karaoke Player Backing Track
    juce::AudioSourceChannelInfo backingInfo(&backingBuffer, 0, numSamples);
    karaokePlayer.getNextAudioBlock(backingInfo);

    float backingPeakLinear = backingBuffer.getMagnitude(0, numSamples);
    updatePeakMeter(karaokePeakDb, backingPeakLinear, 0.92f);

    // 3. Mix Vocals and Backing Track into Master Mix Buffer
    float* mixL = mixBuffer.getWritePointer(0);
    float* mixR = mixBuffer.getWritePointer(1);
    const float* vL = vocalBuffer.getReadPointer(0);
    const float* vR = vocalBuffer.getReadPointer(1);
    const float* bL = backingBuffer.getReadPointer(0);
    const float* bR = backingBuffer.getReadPointer(1);

    float masterVol = masterVolume.load(std::memory_order_relaxed);
    float outPeakLinear = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // Sum vocals and backing track with master volume
        float sumL = (vL[i] + bL[i]) * masterVol;
        float sumR = (vR[i] + bR[i]) * masterVol;

        // Peak and clip detection
        float absMax = std::max(std::abs(sumL), std::abs(sumR));
        if (absMax >= 0.999f)
            outputClipped.store(true, std::memory_order_relaxed);

        if (absMax > outPeakLinear)
            outPeakLinear = absMax;

        mixL[i] = sumL;
        mixR[i] = sumR;
    }

    updatePeakMeter(masterOutputPeakDb, outPeakLinear, 0.92f);

    // 4. Copy to hardware / virtual audio output
    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        int srcCh = (ch < 2) ? ch : 0;
        const float* src = mixBuffer.getReadPointer(srcCh);
        float* dst = outputChannelData[ch];
        std::copy(src, src + numSamples, dst);
    }
}

} // namespace VocalStudio
