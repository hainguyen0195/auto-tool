#pragma once

#include "AutoTuneProcessor.h"
#include "VocalEffects.h"
#include "KaraokePlayer.h"
#include "PresetManager.h"
#include <juce_core/juce_core.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <atomic>
#include <vector>

namespace VocalStudio
{

class AudioEngine : public juce::AudioIODeviceCallback,
                    public juce::ChangeListener
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // Device Manager setup
    bool initialize(int numInputChannels = 1, int numOutputChannels = 2);
    void shutdown();

    juce::AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }

    // AudioIODeviceCallbackWithContext overrides
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceError(const juce::String& errorMessage) override;

    // ChangeListener override (device changed)
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Engine Controls
    void setMicGain(float linearGain) noexcept;
    float getMicGain() const noexcept { return micGain.load(std::memory_order_relaxed); }

    void setMasterVolume(float linearVolume) noexcept;
    float getMasterVolume() const noexcept { return masterVolume.load(std::memory_order_relaxed); }

    void setInputMonitoringEnabled(bool enabled) noexcept;
    bool isInputMonitoringEnabled() const noexcept { return inputMonitoring.load(std::memory_order_relaxed); }

    void setSafeModeEnabled(bool enabled) noexcept;
    bool isSafeModeEnabled() const noexcept { return safeMode.load(std::memory_order_relaxed); }

    // Metering (dB scale, e.g. -60 to 0 dBFS)
    float getMicInputLevelDb() const noexcept { return micInputPeakDb.load(std::memory_order_relaxed); }
    float getKaraokeLevelDb() const noexcept { return karaokePeakDb.load(std::memory_order_relaxed); }
    float getMasterOutputLevelDb() const noexcept { return masterOutputPeakDb.load(std::memory_order_relaxed); }
    bool hasOutputClipped() noexcept { return outputClipped.exchange(false, std::memory_order_relaxed); }

    // Processors access
    AutoTuneProcessor& getAutoTune() noexcept { return autoTune; }
    const AutoTuneProcessor& getAutoTune() const noexcept { return autoTune; }

    VocalEffectsRack& getEffects() noexcept { return effects; }
    const VocalEffectsRack& getEffects() const noexcept { return effects; }

    KaraokePlayer& getKaraokePlayer() noexcept { return karaokePlayer; }
    const KaraokePlayer& getKaraokePlayer() const noexcept { return karaokePlayer; }

    PresetManager& getPresetManager() noexcept { return presetManager; }
    const PresetManager& getPresetManager() const noexcept { return presetManager; }

    double getCurrentSampleRate() const noexcept { return currentSampleRate; }
    int getCurrentBufferSize() const noexcept { return currentBlockSize; }

    juce::String getLastError() const;

private:
    void updatePeakMeter(std::atomic<float>& meterDb, float peakLinear, float decayFactor);

    juce::AudioDeviceManager deviceManager;
    double currentSampleRate { 48000.0 };
    int currentBlockSize { 512 };

    // Core Processors
    AutoTuneProcessor autoTune;
    VocalEffectsRack effects;
    KaraokePlayer karaokePlayer;
    PresetManager presetManager;

    // Engine Settings (thread-safe atomics)
    std::atomic<float> micGain { 1.0f };         // linear
    std::atomic<float> masterVolume { 1.0f };    // linear
    std::atomic<bool> inputMonitoring { true };
    std::atomic<bool> safeMode { false };

    // Meters & Diagnostics
    std::atomic<float> micInputPeakDb { -100.0f };
    std::atomic<float> karaokePeakDb { -100.0f };
    std::atomic<float> masterOutputPeakDb { -100.0f };
    std::atomic<bool> outputClipped { false };

    std::mutex errorMutex;
    juce::String lastErrorMessage;

    // Preallocated Audio Buffers for real-time safety (no heap allocations in callback!)
    juce::AudioBuffer<float> vocalBuffer;
    juce::AudioBuffer<float> backingBuffer;
    juce::AudioBuffer<float> mixBuffer;
};

} // namespace VocalStudio
