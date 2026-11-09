#include "AutoTuneProcessor.h"
#include <algorithm>

namespace VocalStudio
{

AutoTuneProcessor::AutoTuneProcessor()
{
    prepare(48000.0, 512, 2);
}

void AutoTuneProcessor::prepare(double sampleRate, int maxBlockSize, int numChannels)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    detector.prepare(currentSampleRate, maxBlockSize);
    shifter.prepare(currentSampleRate, maxBlockSize, numChannels);
    monoAnalysisBuffer.assign(static_cast<size_t>(std::max(1024, maxBlockSize)), 0.0f);
    reset();
}

void AutoTuneProcessor::reset()
{
    detector.reset();
    shifter.reset();
    smoothedTargetPitchHz = 0.0f;
    currentPitchShiftRatio = 1.0f;
    unvoicedDecayFrames = 0;

    liveDetectedPitchHz.store(0.0f, std::memory_order_relaxed);
    liveTargetPitchHz.store(0.0f, std::memory_order_relaxed);
    liveDetuneCents.store(0.0f, std::memory_order_relaxed);
    liveVoiced.store(false, std::memory_order_relaxed);
    liveTargetMidiNote.store(0, std::memory_order_relaxed);
}

void AutoTuneProcessor::setMode(AutoTuneMode newMode) noexcept
{
    currentMode.store(static_cast<int>(newMode), std::memory_order_relaxed);
}

AutoTuneMode AutoTuneProcessor::getMode() const noexcept
{
    return static_cast<AutoTuneMode>(currentMode.load(std::memory_order_relaxed));
}

void AutoTuneProcessor::setRootKey(int rootKeyIndex) noexcept
{
    currentRootKey.store(juce::jlimit(0, 11, rootKeyIndex), std::memory_order_relaxed);
    noteMapper.setRootKey(currentRootKey.load(std::memory_order_relaxed));
}

int AutoTuneProcessor::getRootKey() const noexcept
{
    return currentRootKey.load(std::memory_order_relaxed);
}

void AutoTuneProcessor::setScaleType(ScaleType scale) noexcept
{
    currentScaleType.store(static_cast<int>(scale), std::memory_order_relaxed);
    noteMapper.setScaleType(scale);
}

ScaleType AutoTuneProcessor::getScaleType() const noexcept
{
    return static_cast<ScaleType>(currentScaleType.load(std::memory_order_relaxed));
}

void AutoTuneProcessor::setCustomNoteEnabled(int noteIndex, bool enabled)
{
    noteMapper.setCustomNoteEnabled(noteIndex, enabled);
}

bool AutoTuneProcessor::isCustomNoteEnabled(int noteIndex) const noexcept
{
    return noteMapper.isCustomNoteEnabled(noteIndex);
}

void AutoTuneProcessor::setRetuneSpeedMs(float ms) noexcept
{
    retuneSpeedMs.store(juce::jlimit(0.0f, 400.0f, ms), std::memory_order_relaxed);
}

float AutoTuneProcessor::getRetuneSpeedMs() const noexcept
{
    return retuneSpeedMs.load(std::memory_order_relaxed);
}

void AutoTuneProcessor::setCorrectionStrength(float strength0to1) noexcept
{
    correctionStrength.store(juce::jlimit(0.0f, 1.0f, strength0to1), std::memory_order_relaxed);
}

float AutoTuneProcessor::getCorrectionStrength() const noexcept
{
    return correctionStrength.load(std::memory_order_relaxed);
}

void AutoTuneProcessor::setHumanize(float humanize0to1) noexcept
{
    humanize.store(juce::jlimit(0.0f, 1.0f, humanize0to1), std::memory_order_relaxed);
}

float AutoTuneProcessor::getHumanize() const noexcept
{
    return humanize.load(std::memory_order_relaxed);
}

juce::String AutoTuneProcessor::getLiveNoteName() const
{
    int midi = liveTargetMidiNote.load(std::memory_order_relaxed);
    return NoteMapper::midiToNoteName(midi);
}

void AutoTuneProcessor::process(float* const* channelData, int numChannels, int numSamples)
{
    if (channelData == nullptr || numChannels <= 0 || numSamples <= 0)
        return;

    auto mode = static_cast<AutoTuneMode>(currentMode.load(std::memory_order_relaxed));
    if (mode == AutoTuneMode::Bypass)
    {
        liveVoiced.store(false, std::memory_order_relaxed);
        liveDetectedPitchHz.store(0.0f, std::memory_order_relaxed);
        liveTargetPitchHz.store(0.0f, std::memory_order_relaxed);
        liveDetuneCents.store(0.0f, std::memory_order_relaxed);
        return;
    }

    // Downmix to mono for pitch detection without allocations
    if (static_cast<size_t>(numSamples) > monoAnalysisBuffer.size())
        monoAnalysisBuffer.resize(static_cast<size_t>(numSamples));

    if (numChannels >= 2)
    {
        for (int i = 0; i < numSamples; ++i)
            monoAnalysisBuffer[static_cast<size_t>(i)] = 0.5f * (channelData[0][i] + channelData[1][i]);
    }
    else
    {
        for (int i = 0; i < numSamples; ++i)
            monoAnalysisBuffer[static_cast<size_t>(i)] = channelData[0][i];
    }

    // Run pitch detection
    auto pitchResult = detector.processBlock(monoAnalysisBuffer.data(), numSamples);

    float detectedPitch = pitchResult.pitchHz;
    bool isVoiced = pitchResult.isVoiced && (detectedPitch >= 60.0f);

    float targetShiftRatio = 1.0f;
    float currentStrength = correctionStrength.load(std::memory_order_relaxed);
    float currentSpeedMs = retuneSpeedMs.load(std::memory_order_relaxed);
    float currentHumanizeVal = humanize.load(std::memory_order_relaxed);

    if (mode == AutoTuneMode::HardTune)
    {
        // Hard tune overrides: instant retune speed, 100% strength, 0% humanize
        currentSpeedMs = 0.0f;
        currentStrength = 1.0f;
        currentHumanizeVal = 0.0f;
    }

    if (isVoiced)
    {
        unvoicedDecayFrames = 0;
        auto snapResult = noteMapper.snapToScale(detectedPitch);

        float idealTargetFreq = snapResult.targetFreqHz;
        float detuneCents = snapResult.detuneCents;

        // Humanize deadband logic:
        // Allows natural vibrato up to +- 30 cents * humanize without aggressive pull
        float deadband = currentHumanizeVal * 25.0f; // cents
        float effectiveDetune = detuneCents;
        if (std::abs(detuneCents) < deadband)
        {
            // Fully within deadband: no pitch correction needed (let vibrato ring)
            effectiveDetune = 0.0f;
        }
        else if (effectiveDetune > 0.0f)
        {
            effectiveDetune -= deadband;
        }
        else
        {
            effectiveDetune += deadband;
        }

        // Apply correction strength
        effectiveDetune *= currentStrength;

        // Calculate corrected frequency from effective detune
        float correctedFreq = detectedPitch * std::pow(2.0f, -effectiveDetune / 1200.0f);

        // Retune speed smoothing filter
        if (currentSpeedMs <= 1.0f || smoothedTargetPitchHz <= 10.0f)
        {
            smoothedTargetPitchHz = correctedFreq;
        }
        else
        {
            // Time constant tau in seconds
            float tau = currentSpeedMs * 0.001f;
            float dt = static_cast<float>(numSamples) / static_cast<float>(currentSampleRate);
            float alpha = std::exp(-dt / tau);
            smoothedTargetPitchHz = alpha * smoothedTargetPitchHz + (1.0f - alpha) * correctedFreq;
        }

        targetShiftRatio = smoothedTargetPitchHz / detectedPitch;
        // Clamp to safe pitch shift range (+- 1 octave)
        targetShiftRatio = juce::jlimit(0.5f, 2.0f, targetShiftRatio);

        // Update live metrics
        liveDetectedPitchHz.store(detectedPitch, std::memory_order_relaxed);
        liveTargetPitchHz.store(idealTargetFreq, std::memory_order_relaxed);
        liveDetuneCents.store(detuneCents, std::memory_order_relaxed);
        liveVoiced.store(true, std::memory_order_relaxed);
        liveTargetMidiNote.store(snapResult.roundedMidiNote, std::memory_order_relaxed);
    }
    else
    {
        // Unvoiced (breath, silence, consonant 's', 't', 'k')
        // Gracefully decay shift ratio back to 1.0 to avoid warping consonants
        unvoicedDecayFrames++;
        if (unvoicedDecayFrames > 4)
        {
            targetShiftRatio = 1.0f;
            smoothedTargetPitchHz = 0.0f;
            liveVoiced.store(false, std::memory_order_relaxed);
            liveDetuneCents.store(0.0f, std::memory_order_relaxed);
        }
        else
        {
            targetShiftRatio = currentPitchShiftRatio * 0.7f + 0.3f;
        }
    }

    currentPitchShiftRatio = targetShiftRatio;
    shifter.setShiftRatio(targetShiftRatio);

    // Apply real-time pitch shifting to audio
    shifter.process(channelData, numChannels, numSamples);
}

} // namespace VocalStudio
