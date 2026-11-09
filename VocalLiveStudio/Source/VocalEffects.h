#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <vector>
#include <cmath>

namespace VocalStudio
{

// Biquad filter structure (Direct Form II Transposed)
struct Biquad
{
    float b0 { 1.0f }, b1 { 0.0f }, b2 { 0.0f };
    float a1 { 0.0f }, a2 { 0.0f };
    float z1 { 0.0f }, z2 { 0.0f };

    void reset() noexcept
    {
        z1 = 0.0f;
        z2 = 0.0f;
    }

    inline float process(float in) noexcept
    {
        float out = in * b0 + z1;
        z1 = in * b1 - a1 * out + z2;
        z2 = in * b2 - a2 * out;
        return out;
    }

    void setLowShelf(float sampleRate, float freqHz, float gainDb, float Q = 0.7071f);
    void setHighShelf(float sampleRate, float freqHz, float gainDb, float Q = 0.7071f);
    void setPeaking(float sampleRate, float freqHz, float gainDb, float Q = 1.0f);
    void setBandPass(float sampleRate, float freqHz, float Q = 1.414f);
    void setLowPass(float sampleRate, float freqHz, float Q = 0.7071f);
};

// 1. Noise Gate
class NoiseGate
{
public:
    void prepare(double sampleRate);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> thresholdDb { -50.0f }; // -80 to 0 dB
    std::atomic<float> attackMs { 5.0f };      // 1 to 50 ms
    std::atomic<float> holdMs { 50.0f };       // 10 to 200 ms
    std::atomic<float> releaseMs { 100.0f };   // 10 to 500 ms

private:
    double currentSampleRate { 48000.0 };
    float envelope { 0.0f };
    int holdCounter { 0 };
};

// 2. De-Esser
class DeEsser
{
public:
    void prepare(double sampleRate, int maxBlockSize);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> frequencyHz { 6000.0f }; // 4 kHz - 8 kHz
    std::atomic<float> thresholdDb { -24.0f };  // -40 to 0 dB
    std::atomic<float> reductionDb { 6.0f };    // Max reduction

private:
    double currentSampleRate { 48000.0 };
    Biquad sidechainFilter[2];
    float envelope { 0.0f };
};

// 3. 3-Band Parametric EQ
class ParametricEQ
{
public:
    void prepare(double sampleRate);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> lowGainDb { 0.0f };   // -15 to +15 dB
    std::atomic<float> lowFreqHz { 120.0f }; // 80 - 250 Hz
    std::atomic<float> midGainDb { 0.0f };   // -15 to +15 dB
    std::atomic<float> midFreqHz { 2500.0f };// 500 - 4000 Hz
    std::atomic<float> highGainDb { 0.0f };  // -15 to +15 dB
    std::atomic<float> highFreqHz { 8500.0f };// 6000 - 12000 Hz

private:
    void updateCoefficients();

    double currentSampleRate { 48000.0 };
    Biquad lowShelf[2];
    Biquad midPeak[2];
    Biquad highShelf[2];

    float lastLowGain { 999.0f }, lastLowFreq { 0.0f };
    float lastMidGain { 999.0f }, lastMidFreq { 0.0f };
    float lastHighGain { 999.0f }, lastHighFreq { 0.0f };
};

// 4. Vocal Compressor
class VocalCompressor
{
public:
    void prepare(double sampleRate);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> thresholdDb { -18.0f };  // -40 to 0 dB
    std::atomic<float> ratio { 3.5f };          // 1:1 to 20:1
    std::atomic<float> attackMs { 15.0f };      // 1 to 100 ms
    std::atomic<float> releaseMs { 120.0f };    // 10 to 1000 ms
    std::atomic<float> makeupGainDb { 3.0f };   // 0 to 24 dB

private:
    double currentSampleRate { 48000.0 };
    float envelopeDb { -90.0f };
};

// 5. Echo / Delay
class VocalDelay
{
public:
    void prepare(double sampleRate, int maxBlockSize);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> timeMs { 260.0f };       // 10 to 1000 ms
    std::atomic<float> feedback { 0.25f };      // 0.0 to 0.85
    std::atomic<float> highDampHz { 4500.0f };  // 1 kHz to 10 kHz
    std::atomic<float> mix { 0.20f };           // 0.0 to 1.0 (wet mix)

private:
    double currentSampleRate { 48000.0 };
    int maxDelaySamples { 96000 };
    int writeIndex { 0 };
    std::vector<std::vector<float>> delayLines;
    Biquad dampFilter[2];
};

// 6. Studio Reverb
class VocalReverb
{
public:
    void prepare(double sampleRate);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> roomSize { 0.65f }; // 0.0 to 1.0
    std::atomic<float> damping { 0.40f };  // 0.0 to 1.0
    std::atomic<float> width { 0.80f };    // 0.0 to 1.0
    std::atomic<float> mix { 0.22f };      // 0.0 to 1.0 (wet mix)

private:
    double currentSampleRate { 48000.0 };
    juce::Reverb juceReverb;
    juce::Reverb::Parameters params;
};

// 7. Output Limiter
class VocalLimiter
{
public:
    void prepare(double sampleRate);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    std::atomic<bool> enabled { true };
    std::atomic<float> ceilingDb { -0.2f }; // Ceiling in dBFS
    bool hasClipped() noexcept { return clipped.exchange(false); }

private:
    double currentSampleRate { 48000.0 };
    float peakEnvelope { 0.0f };
    std::atomic<bool> clipped { false };
};

// Master Vocal Effects Rack
class VocalEffectsRack
{
public:
    VocalEffectsRack();
    ~VocalEffectsRack() = default;

    void prepare(double sampleRate, int maxBlockSize, int numChannels = 2);
    void reset();
    void process(float* const* channelData, int numChannels, int numSamples);

    NoiseGate gate;
    DeEsser deEsser;
    ParametricEQ eq;
    VocalCompressor comp;
    VocalDelay delay;
    VocalReverb reverb;
    VocalLimiter limiter;

private:
    double currentSampleRate { 48000.0 };
    int activeChannels { 2 };
};

} // namespace VocalStudio
