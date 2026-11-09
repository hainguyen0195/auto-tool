#pragma once

#include "AudioEngine.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

namespace VocalStudio
{

// Custom look and feel for dark studio theme
class StudioLookAndFeel : public juce::LookAndFeel_V4
{
public:
    StudioLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
};

// UI Component for rendering live vertical / horizontal dB meters
class PeakMeterComponent : public juce::Component
{
public:
    PeakMeterComponent(const juce::String& labelText, bool vertical = true);
    void setLevel(float levelDb);
    void setClipped(bool isClipped);
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
    bool isVertical { true };
    float currentLevelDb { -100.0f };
    bool clipped { false };
};

// Main Window Component
class MainComponent : public juce::Component,
                      public juce::Timer,
                      public juce::Button::Listener,
                      public juce::Slider::Listener,
                      public juce::ComboBox::Listener
{
public:
    MainComponent(AudioEngine& engine);
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;

private:
    void setupUI();
    void updateCustomScalePillsVisibility();
    void syncUIToPreset(const PresetData& preset);
    void openAudioSettings();
    void openLivestreamGuide();
    void openSavePresetDialog();
    void openLoadPresetDialog();
    void loadKaraokeFile();

    AudioEngine& audioEngine;
    StudioLookAndFeel studioLookAndFeel;

    // Top Header
    juce::Label titleLabel;
    juce::Label presetLabel;
    juce::ComboBox presetComboBox;
    juce::TextButton savePresetButton { "Save" };
    juce::TextButton loadPresetButton { "Load" };
    juce::TextButton audioSettingsButton { "Audio Setup" };
    juce::TextButton liveRoutingButton { "OBS / Live Guide" };

    // Master Output & Limiter
    juce::Slider masterVolSlider;
    juce::Label masterVolLabel { {}, "MASTER OUT" };
    PeakMeterComponent masterMeter { "OUT", false };
    juce::TextButton clipResetButton { "CLIP" };

    // Mic In Section
    juce::GroupComponent micGroup { "micGroup", "1. MICROPHONE INPUT" };
    juce::Slider micGainSlider;
    juce::Label micGainLabel { {}, "MIC GAIN" };
    PeakMeterComponent micMeter { "MIC", false };
    juce::ToggleButton monitorToggle { "Headphone Monitor" };
    juce::ToggleButton safeModeToggle { "Safe Mode" };

    // Auto-Tune Section
    juce::GroupComponent autoTuneGroup { "autoTuneGroup", "2. REAL-TIME AUTO-TUNE" };
    juce::TextButton naturalModeButton { "NATURAL" };
    juce::TextButton hardTuneModeButton { "HARD TUNE" };
    juce::TextButton bypassModeButton { "BYPASS" };

    juce::Label keyLabel { {}, "KEY:" };
    juce::ComboBox keyComboBox;
    juce::Label scaleLabel { {}, "SCALE:" };
    juce::ComboBox scaleComboBox;

    juce::Slider retuneSpeedSlider;
    juce::Label retuneSpeedLabel { {}, "Retune Speed" };
    juce::Slider strengthSlider;
    juce::Label strengthLabel { {}, "Strength" };
    juce::Slider humanizeSlider;
    juce::Label humanizeLabel { {}, "Humanize" };

    // Custom Scale Note Pills (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
    std::vector<std::unique_ptr<juce::TextButton>> customNotePills;

    // Live Pitch Display
    juce::Label pitchDisplayTitle { {}, "LIVE PITCH MONITOR" };
    juce::Label pitchDetectedLabel;
    juce::Label pitchTargetLabel;
    juce::Label pitchDetuneLabel;

    // Vocal Effects Section (Rack)
    juce::GroupComponent effectsGroup { "effectsGroup", "3. VOCAL EFFECTS RACK" };

    // 1. Noise Gate
    juce::ToggleButton gateToggle { "Gate" };
    juce::Slider gateThreshSlider;

    // 2. De-Esser
    juce::ToggleButton deEsserToggle { "De-Esser" };
    juce::Slider deEsserThreshSlider;

    // 3. EQ (3-Band)
    juce::ToggleButton eqToggle { "3-Band EQ" };
    juce::Slider eqLowSlider;
    juce::Slider eqMidSlider;
    juce::Slider eqHighSlider;

    // 4. Compressor
    juce::ToggleButton compToggle { "Compressor" };
    juce::Slider compThreshSlider;
    juce::Slider compRatioSlider;

    // 5. Delay
    juce::ToggleButton delayToggle { "Delay" };
    juce::Slider delayTimeSlider;
    juce::Slider delayMixSlider;

    // 6. Reverb
    juce::ToggleButton reverbToggle { "Reverb" };
    juce::Slider reverbSizeSlider;
    juce::Slider reverbMixSlider;

    // 7. Limiter
    juce::ToggleButton limiterToggle { "Anti-Clip Limiter" };

    // Karaoke Player Section (Bottom)
    juce::GroupComponent karaokeGroup { "karaokeGroup", "4. KARAOKE & BACKING TRACK PLAYER" };
    juce::TextButton loadTrackButton { "Load Track..." };
    juce::Label trackNameLabel { {}, "No audio loaded (MP3 / WAV supported)" };
    juce::TextButton playButton { "Play" };
    juce::TextButton pauseButton { "Pause" };
    juce::TextButton stopButton { "Stop" };
    juce::Slider positionSlider;
    juce::Label timeLabel { {}, "00:00 / 00:00" };

    juce::Slider backingVolSlider;
    juce::Label backingVolLabel { {}, "Track Vol" };
    PeakMeterComponent karaokeMeter { "TRACK", false };

    juce::Slider pitchShiftSlider;
    juce::Label pitchShiftLabel { {}, "Key Transpose" };
    juce::Slider tempoSlider;
    juce::Label tempoLabel { {}, "Tempo" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace VocalStudio
