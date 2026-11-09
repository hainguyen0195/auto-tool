#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include <cmath>

namespace VocalStudio
{

class PitchShifter
{
public:
    PitchShifter();
    ~PitchShifter() = default;

    void prepare(double sampleRate, int maxBlockSize, int numChannels = 2);
    void reset();

    // Set pitch shift ratio: R = f_target / f_detected
    // R = 1.0: no shift
    // R = 2.0: 1 octave up
    // R = 0.5: 1 octave down
    void setShiftRatio(float ratio) noexcept;

    // Set shift in semitones (-12.0 to +12.0)
    void setShiftSemitones(float semitones) noexcept;

    // Grain/window size in milliseconds (e.g. 30ms to 60ms; 40ms is sweet spot for vocals)
    void setWindowSizeMs(float ms);

    // Process stereo or mono audio block in-place
    void process(float* const* channelData, int numChannels, int numSamples);

    float getCurrentShiftRatio() const noexcept { return currentRatio; }

private:
    float readHermite(int channel, float readPos) const noexcept;

    double currentSampleRate { 48000.0 };
    float targetRatio { 1.0f };
    float currentRatio { 1.0f };
    float slewFactor { 0.005f }; // Smoothing factor per sample

    float windowSizeMs { 40.0f };
    int windowSamples { 1920 };

    // Dual-tap phases in [0.0, 1.0)
    float phaseA { 0.0f };
    float phaseB { 0.5f };

    // Delay buffers
    int bufferSize { 16384 };
    int writePos { 0 };
    std::vector<std::vector<float>> delayBuffers;
};

} // namespace VocalStudio
