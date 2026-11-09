#pragma once

#include "AutoTuneProcessor.h"
#include "VocalEffects.h"
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>

namespace VocalStudio
{

struct PresetData
{
    juce::String name;

    // Auto-Tune
    int autoTuneMode { 0 }; // 0 = Natural, 1 = HardTune, 2 = Bypass
    int rootKey { 0 };      // 0..11
    int scaleType { 1 };    // 1 = Major
    float retuneSpeedMs { 80.0f };
    float correctionStrength { 0.85f };
    float humanize { 0.40f };

    // Noise Gate
    bool gateEnabled { true };
    float gateThresholdDb { -50.0f };
    float gateAttackMs { 5.0f };
    float gateReleaseMs { 100.0f };

    // De-Esser
    bool deEsserEnabled { true };
    float deEsserFreqHz { 6000.0f };
    float deEsserThreshDb { -24.0f };
    float deEsserReductionDb { 6.0f };

    // 3-Band EQ
    bool eqEnabled { true };
    float eqLowGainDb { 0.0f };
    float eqLowFreqHz { 120.0f };
    float eqMidGainDb { 0.0f };
    float eqMidFreqHz { 2500.0f };
    float eqHighGainDb { 0.0f };
    float eqHighFreqHz { 8500.0f };

    // Compressor
    bool compEnabled { true };
    float compThresholdDb { -18.0f };
    float compRatio { 3.5f };
    float compAttackMs { 15.0f };
    float compReleaseMs { 120.0f };
    float compMakeupDb { 3.0f };

    // Delay
    bool delayEnabled { true };
    float delayTimeMs { 260.0f };
    float delayFeedback { 0.25f };
    float delayHighDampHz { 4500.0f };
    float delayMix { 0.20f };

    // Reverb
    bool reverbEnabled { true };
    float reverbRoomSize { 0.65f };
    float reverbDamping { 0.40f };
    float reverbWidth { 0.80f };
    float reverbMix { 0.22f };

    // Limiter
    bool limiterEnabled { true };
    float limiterCeilingDb { -0.2f };

    // Serialization
    juce::var toVar() const;
    static PresetData fromVar(const juce::var& v);
};

class PresetManager
{
public:
    PresetManager();
    ~PresetManager() = default;

    const std::vector<PresetData>& getFactoryPresets() const noexcept { return factoryPresets; }

    void applyPreset(const PresetData& preset, AutoTuneProcessor& autoTune, VocalEffectsRack& effects);
    PresetData captureCurrentState(const juce::String& name, const AutoTuneProcessor& autoTune, const VocalEffectsRack& effects);

    bool savePresetToFile(const PresetData& preset, const juce::File& file);
    bool loadPresetFromFile(const juce::File& file, PresetData& outPreset);

    juce::File getPresetsDirectory() const;

private:
    void initFactoryPresets();
    std::vector<PresetData> factoryPresets;
};

} // namespace VocalStudio
