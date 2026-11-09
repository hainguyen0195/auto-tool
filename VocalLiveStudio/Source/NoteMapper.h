#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <vector>
#include <cmath>
#include <string>

namespace VocalStudio
{

enum class ScaleType
{
    Chromatic = 0,
    Major,
    MinorNatural,
    MinorHarmonic,
    PentatonicMajor,
    PentatonicMinor,
    Custom
};

struct ScaleDefinition
{
    const char* name;
    ScaleType type;
};

class NoteMapper
{
public:
    NoteMapper();
    ~NoteMapper() = default;

    // Root key: 0 = C, 1 = C#, 2 = D, ..., 11 = B
    void setRootKey(int rootKeyIndex);
    int getRootKey() const noexcept { return rootKey; }

    void setScaleType(ScaleType type);
    ScaleType getScaleType() const noexcept { return currentScale; }

    // Toggle individual semitone in custom scale (0 to 11 relative to C)
    void setCustomNoteEnabled(int noteIndex, bool enabled);
    bool isCustomNoteEnabled(int noteIndex) const noexcept;

    // Convert frequency in Hz to MIDI note (fractional)
    static float frequencyToMidi(float freqHz) noexcept;

    // Convert MIDI note to frequency in Hz
    static float midiToFrequency(float midiNote) noexcept;

    // Get note name for a given MIDI note (e.g. "C4", "A#3")
    static juce::String midiToNoteName(int midiNote);

    // Given detected pitch in Hz, find the target quantized frequency in Hz and target MIDI note
    struct TargetResult
    {
        float targetFreqHz { 0.0f };
        float targetMidiNote { 0.0f };
        float detuneCents { 0.0f }; // Cents difference (detected - target)
        int roundedMidiNote { 0 };
        juce::String noteName;
    };

    TargetResult snapToScale(float detectedFreqHz) const;

    static const std::array<const char*, 12>& getNoteNames() noexcept;
    static const std::vector<ScaleDefinition>& getAvailableScales() noexcept;

private:
    void rebuildActiveNotes();

    int rootKey { 0 }; // 0 = C
    ScaleType currentScale { ScaleType::Major };
    std::array<bool, 12> customNotesActive {};
    std::array<bool, 12> currentScaleNotesActive {}; // absolute semitones 0..11 active in scale
};

} // namespace VocalStudio
