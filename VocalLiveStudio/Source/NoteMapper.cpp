#include "NoteMapper.h"

namespace VocalStudio
{

static const std::array<const char*, 12> sNoteNames = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

static const std::vector<ScaleDefinition> sScales = {
    { "Chromatic", ScaleType::Chromatic },
    { "Major", ScaleType::Major },
    { "Natural Minor", ScaleType::MinorNatural },
    { "Harmonic Minor", ScaleType::MinorHarmonic },
    { "Pentatonic Major", ScaleType::PentatonicMajor },
    { "Pentatonic Minor", ScaleType::PentatonicMinor },
    { "Custom", ScaleType::Custom }
};

const std::array<const char*, 12>& NoteMapper::getNoteNames() noexcept
{
    return sNoteNames;
}

const std::vector<ScaleDefinition>& NoteMapper::getAvailableScales() noexcept
{
    return sScales;
}

NoteMapper::NoteMapper()
{
    customNotesActive.fill(true);
    rebuildActiveNotes();
}

void NoteMapper::setRootKey(int rootKeyIndex)
{
    rootKey = juce::jlimit(0, 11, rootKeyIndex);
    rebuildActiveNotes();
}

void NoteMapper::setScaleType(ScaleType type)
{
    currentScale = type;
    rebuildActiveNotes();
}

void NoteMapper::setCustomNoteEnabled(int noteIndex, bool enabled)
{
    if (noteIndex >= 0 && noteIndex < 12)
    {
        customNotesActive[static_cast<size_t>(noteIndex)] = enabled;
        if (currentScale == ScaleType::Custom)
            rebuildActiveNotes();
    }
}

bool NoteMapper::isCustomNoteEnabled(int noteIndex) const noexcept
{
    if (noteIndex >= 0 && noteIndex < 12)
        return customNotesActive[static_cast<size_t>(noteIndex)];
    return false;
}

void NoteMapper::rebuildActiveNotes()
{
    currentScaleNotesActive.fill(false);

    // Intervals relative to root (0 = root)
    std::vector<int> intervals;
    switch (currentScale)
    {
        case ScaleType::Chromatic:
            intervals = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
            break;
        case ScaleType::Major:
            intervals = { 0, 2, 4, 5, 7, 9, 11 };
            break;
        case ScaleType::MinorNatural:
            intervals = { 0, 2, 3, 5, 7, 8, 10 };
            break;
        case ScaleType::MinorHarmonic:
            intervals = { 0, 2, 3, 5, 7, 8, 11 };
            break;
        case ScaleType::PentatonicMajor:
            intervals = { 0, 2, 4, 7, 9 };
            break;
        case ScaleType::PentatonicMinor:
            intervals = { 0, 3, 5, 7, 10 };
            break;
        case ScaleType::Custom:
            for (int i = 0; i < 12; ++i)
            {
                if (customNotesActive[static_cast<size_t>(i)])
                    currentScaleNotesActive[static_cast<size_t>(i)] = true;
            }
            return;
    }

    for (int interval : intervals)
    {
        int note = (rootKey + interval) % 12;
        currentScaleNotesActive[static_cast<size_t>(note)] = true;
    }
}

float NoteMapper::frequencyToMidi(float freqHz) noexcept
{
    if (freqHz <= 8.1757989156f) // Below C-1
        return 0.0f;
    return 69.0f + 12.0f * std::log2(freqHz / 440.0f);
}

float NoteMapper::midiToFrequency(float midiNote) noexcept
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

juce::String NoteMapper::midiToNoteName(int midiNote)
{
    if (midiNote < 0 || midiNote > 127)
        return "-";
    int semitone = midiNote % 12;
    int octave = (midiNote / 12) - 1;
    return juce::String(sNoteNames[static_cast<size_t>(semitone)]) + juce::String(octave);
}

NoteMapper::TargetResult NoteMapper::snapToScale(float detectedFreqHz) const
{
    TargetResult result;
    if (detectedFreqHz < 40.0f || detectedFreqHz > 2500.0f)
        return result;

    float midiExact = frequencyToMidi(detectedFreqHz);
    int roundedMidi = static_cast<int>(std::round(midiExact));

    // Find nearest note that is active in current scale
    int bestMidi = roundedMidi;
    float minDistance = 100.0f;

    // Search outwards from roundedMidi up to +- 12 semitones
    for (int offset = 0; offset <= 12; ++offset)
    {
        int candidates[2] = { roundedMidi - offset, roundedMidi + offset };
        for (int cand : candidates)
        {
            if (cand < 12 || cand > 120)
                continue;

            int semitone = (cand % 12 + 12) % 12;
            if (currentScaleNotesActive[static_cast<size_t>(semitone)])
            {
                float dist = std::abs(midiExact - static_cast<float>(cand));
                if (dist < minDistance)
                {
                    minDistance = dist;
                    bestMidi = cand;
                }
            }
        }

        if (minDistance <= static_cast<float>(offset))
            break;
    }

    result.targetMidiNote = static_cast<float>(bestMidi);
    result.roundedMidiNote = bestMidi;
    result.targetFreqHz = midiToFrequency(result.targetMidiNote);
    result.detuneCents = (midiExact - result.targetMidiNote) * 100.0f;
    result.noteName = midiToNoteName(bestMidi);

    return result;
}

} // namespace VocalStudio
