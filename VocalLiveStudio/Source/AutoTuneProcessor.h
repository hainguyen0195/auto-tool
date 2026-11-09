#pragma once

#include "PitchDetector.h"
#include "NoteMapper.h"
#include "PitchShifter.h"
#include <juce_core/juce_core.h>
#include <atomic>

namespace VocalStudio
{

enum class AutoTuneMode
{
    Natural = 0,
    HardTune,
    Bypass
};

class AutoTuneProcessor
{
public:
    AutoTuneProcessor();
    ~AutoTuneProcessor() = default;

    void prepare(double sampleRate, int maxBlockSize, int numChannels = 2);
    void reset();

    // Audio block processing (real-time safe, no heap allocations)
    void process(float* const* channelData, int numChannels, int numSamples);

    // Mode control
    void setMode(AutoTuneMode newMode) noexcept;
    AutoTuneMode getMode() const noexcept;

    // Musical key & scale
    void setRootKey(int rootKeyIndex) noexcept;
    int getRootKey() const noexcept;

    void setScaleType(ScaleType scale) noexcept;
    ScaleType getScaleType() const noexcept;

    void setCustomNoteEnabled(int noteIndex, bool enabled);
    bool isCustomNoteEnabled(int noteIndex) const noexcept;

    // Auto-tune parameters
    void setRetuneSpeedMs(float ms) noexcept;
    float getRetuneSpeedMs() const noexcept;

    void setCorrectionStrength(float strength0to1) noexcept;
    float getCorrectionStrength() const noexcept;

    void setHumanize(float humanize0to1) noexcept;
    float getHumanize() const noexcept;

    // Live monitoring stats for UI
    float getLiveDetectedPitchHz() const noexcept { return liveDetectedPitchHz.load(std::memory_order_relaxed); }
    float getLiveTargetPitchHz() const noexcept { return liveTargetPitchHz.load(std::memory_order_relaxed); }
    float getLiveDetuneCents() const noexcept { return liveDetuneCents.load(std::memory_order_relaxed); }
    bool isLiveVoiced() const noexcept { return liveVoiced.load(std::memory_order_relaxed); }
    juce::String getLiveNoteName() const;

private:
    double currentSampleRate { 48000.0 };

    PitchDetector detector;
    NoteMapper noteMapper;
    PitchShifter shifter;

    // Thread-safe parameters
    std::atomic<int> currentMode { static_cast<int>(AutoTuneMode::Natural) };
    std::atomic<int> currentRootKey { 0 };
    std::atomic<int> currentScaleType { static_cast<int>(ScaleType::Major) };
    std::atomic<float> retuneSpeedMs { 80.0f };       // ms
    std::atomic<float> correctionStrength { 0.85f };  // 0.0 to 1.0
    std::atomic<float> humanize { 0.40f };            // 0.0 to 1.0

    // Internal DSP state
    float smoothedTargetPitchHz { 0.0f };
    float currentPitchShiftRatio { 1.0f };
    int unvoicedDecayFrames { 0 };

    // Live display values
    std::atomic<float> liveDetectedPitchHz { 0.0f };
    std::atomic<float> liveTargetPitchHz { 0.0f };
    std::atomic<float> liveDetuneCents { 0.0f };
    std::atomic<bool> liveVoiced { false };
    std::atomic<int> liveTargetMidiNote { 0 };

    // Scratch mono buffer for pitch detection
    std::vector<float> monoAnalysisBuffer;
};

} // namespace VocalStudio
