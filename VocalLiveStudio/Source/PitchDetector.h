#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include <array>
#include <cmath>

namespace VocalStudio
{

struct PitchDetectionResult
{
    float pitchHz { 0.0f };
    float confidence { 0.0f }; // 0.0 (unvoiced/noise) to 1.0 (strongly periodic)
    bool isVoiced { false };
    float rmsLevel { 0.0f };
};

class PitchDetector
{
public:
    PitchDetector();
    ~PitchDetector() = default;

    void prepare(double sampleRate, int maxBlockSize);
    void reset();

    // Process a block of audio. Does NOT allocate heap memory.
    // Returns the latest pitch detection result.
    PitchDetectionResult processBlock(const float* channelData, int numSamples);

    PitchDetectionResult getLatestResult() const noexcept { return latestResult; }

    void setFrequencyRange(float minHz, float maxHz);
    void setThreshold(float threshold) noexcept { yinThreshold = threshold; }
    void setSilenceThreshold(float silenceDb) noexcept;

private:
    void runYinAnalysis();

    double currentSampleRate { 48000.0 };
    float minFrequency { 60.0f };   // Male low chest voice (~B1/C2)
    float maxFrequency { 1100.0f }; // High soprano vocal range (~C6)
    float yinThreshold { 0.15f };
    float silenceLinearThreshold { 0.003f }; // ~ -50 dBFS

    int minTau { 44 };
    int maxTau { 800 };
    int windowSize { 1024 };
    int hopSize { 128 };
    int hopCounter { 0 };

    // Preallocated buffers (real-time safe)
    std::vector<float> ringBuffer;
    int ringWritePos { 0 };
    int ringBufferSize { 4096 };

    std::vector<float> yinBuffer;      // Holds CMNDF values
    std::vector<float> analysisWindow; // Window extracted from ring buffer

    // Median filter history for octave jumping prevention
    std::array<float, 3> pitchHistory {};
    int historyIndex { 0 };

    PitchDetectionResult latestResult {};
};

} // namespace VocalStudio
