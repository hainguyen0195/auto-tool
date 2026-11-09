#include "PitchShifter.h"
#include <algorithm>

namespace VocalStudio
{

static constexpr float TWO_PI = 6.28318530717958647692f;

PitchShifter::PitchShifter()
{
    prepare(48000.0, 512, 2);
}

void PitchShifter::prepare(double sampleRate, int /*maxBlockSize*/, int numChannels)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    bufferSize = 16384; // ~340ms buffer at 48kHz, more than plenty for 40ms grains

    int chCount = std::max(1, numChannels);
    delayBuffers.resize(static_cast<size_t>(chCount));
    for (auto& buf : delayBuffers)
    {
        buf.assign(static_cast<size_t>(bufferSize), 0.0f);
    }

    setWindowSizeMs(windowSizeMs);
    reset();
}

void PitchShifter::reset()
{
    for (auto& buf : delayBuffers)
        std::fill(buf.begin(), buf.end(), 0.0f);

    writePos = 0;
    phaseA = 0.0f;
    phaseB = 0.5f;
    targetRatio = 1.0f;
    currentRatio = 1.0f;
}

void PitchShifter::setWindowSizeMs(float ms)
{
    windowSizeMs = juce::jlimit(15.0f, 100.0f, ms);
    windowSamples = static_cast<int>(std::round(currentSampleRate * (windowSizeMs * 0.001f)));
    if (windowSamples < 64)
        windowSamples = 64;
    if (windowSamples > bufferSize / 2)
        windowSamples = bufferSize / 2;
}

void PitchShifter::setShiftRatio(float ratio) noexcept
{
    // Clamp shift ratio to reasonable vocal correction range: [0.5, 2.0] (-1 octave to +1 octave)
    targetRatio = juce::jlimit(0.5f, 2.0f, ratio);
}

void PitchShifter::setShiftSemitones(float semitones) noexcept
{
    float ratio = std::pow(2.0f, semitones / 12.0f);
    setShiftRatio(ratio);
}

float PitchShifter::readHermite(int channel, float readPos) const noexcept
{
    const auto& buf = delayBuffers[static_cast<size_t>(channel)];
    
    // Wrap to [0, bufferSize)
    while (readPos < 0.0f)
        readPos += static_cast<float>(bufferSize);
    while (readPos >= static_cast<float>(bufferSize))
        readPos -= static_cast<float>(bufferSize);

    int i1 = static_cast<int>(readPos);
    float frac = readPos - static_cast<float>(i1);

    int i0 = (i1 - 1 + bufferSize) % bufferSize;
    int i2 = (i1 + 1) % bufferSize;
    int i3 = (i1 + 2) % bufferSize;

    float y0 = buf[static_cast<size_t>(i0)];
    float y1 = buf[static_cast<size_t>(i1)];
    float y2 = buf[static_cast<size_t>(i2)];
    float y3 = buf[static_cast<size_t>(i3)];

    // 4-point, 3rd-order Hermite interpolation
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

void PitchShifter::process(float* const* channelData, int numChannels, int numSamples)
{
    if (channelData == nullptr || numChannels <= 0 || numSamples <= 0)
        return;

    int activeChannels = std::min(numChannels, static_cast<int>(delayBuffers.size()));
    float invWindowSamples = 1.0f / static_cast<float>(windowSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        // Smooth slew for pitch shift ratio (prevents clicks)
        currentRatio += (targetRatio - currentRatio) * slewFactor;

        // Rate of phase change: (1.0 - currentRatio) / windowSamples
        float deltaPhase = (1.0f - currentRatio) * invWindowSamples;

        // Advance phases
        phaseA += deltaPhase;
        if (phaseA >= 1.0f)
            phaseA -= 1.0f;
        else if (phaseA < 0.0f)
            phaseA += 1.0f;

        phaseB = phaseA + 0.5f;
        if (phaseB >= 1.0f)
            phaseB -= 1.0f;

        // Hann windows: 0.5 - 0.5 * cos(2*pi*phase)
        float winA = 0.5f - 0.5f * std::cos(TWO_PI * phaseA);
        float winB = 0.5f - 0.5f * std::cos(TWO_PI * phaseB);

        // Calculate read delays for tap A and tap B
        float delayA = phaseA * static_cast<float>(windowSamples);
        float delayB = phaseB * static_cast<float>(windowSamples);

        // Base safety margin to prevent reading ahead of write position
        float baseDelay = static_cast<float>(windowSamples) * 0.1f + 16.0f;

        float readPosA = static_cast<float>(writePos) - (delayA + baseDelay);
        float readPosB = static_cast<float>(writePos) - (delayB + baseDelay);

        // Process all channels
        for (int ch = 0; ch < activeChannels; ++ch)
        {
            float inputSample = channelData[ch][i];

            // Write to circular delay buffer
            delayBuffers[static_cast<size_t>(ch)][static_cast<size_t>(writePos)] = inputSample;

            // Read with Hermite interpolation from both taps
            float outA = readHermite(ch, readPosA);
            float outB = readHermite(ch, readPosB);

            // Crossfaded combination
            channelData[ch][i] = outA * winA + outB * winB;
        }

        // Advance write pointer
        writePos = (writePos + 1) % bufferSize;
    }
}

} // namespace VocalStudio
