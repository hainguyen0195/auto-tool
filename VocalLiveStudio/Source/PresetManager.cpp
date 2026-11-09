#include "PresetManager.h"

namespace VocalStudio
{

juce::var PresetData::toVar() const
{
    auto* obj = new juce::DynamicObject();
    obj->setProperty("name", name);

    // AutoTune
    obj->setProperty("autoTuneMode", autoTuneMode);
    obj->setProperty("rootKey", rootKey);
    obj->setProperty("scaleType", scaleType);
    obj->setProperty("retuneSpeedMs", retuneSpeedMs);
    obj->setProperty("correctionStrength", correctionStrength);
    obj->setProperty("humanize", humanize);

    // Gate
    obj->setProperty("gateEnabled", gateEnabled);
    obj->setProperty("gateThresholdDb", gateThresholdDb);
    obj->setProperty("gateAttackMs", gateAttackMs);
    obj->setProperty("gateReleaseMs", gateReleaseMs);

    // DeEsser
    obj->setProperty("deEsserEnabled", deEsserEnabled);
    obj->setProperty("deEsserFreqHz", deEsserFreqHz);
    obj->setProperty("deEsserThreshDb", deEsserThreshDb);
    obj->setProperty("deEsserReductionDb", deEsserReductionDb);

    // EQ
    obj->setProperty("eqEnabled", eqEnabled);
    obj->setProperty("eqLowGainDb", eqLowGainDb);
    obj->setProperty("eqLowFreqHz", eqLowFreqHz);
    obj->setProperty("eqMidGainDb", eqMidGainDb);
    obj->setProperty("eqMidFreqHz", eqMidFreqHz);
    obj->setProperty("eqHighGainDb", eqHighGainDb);
    obj->setProperty("eqHighFreqHz", eqHighFreqHz);

    // Comp
    obj->setProperty("compEnabled", compEnabled);
    obj->setProperty("compThresholdDb", compThresholdDb);
    obj->setProperty("compRatio", compRatio);
    obj->setProperty("compAttackMs", compAttackMs);
    obj->setProperty("compReleaseMs", compReleaseMs);
    obj->setProperty("compMakeupDb", compMakeupDb);

    // Delay
    obj->setProperty("delayEnabled", delayEnabled);
    obj->setProperty("delayTimeMs", delayTimeMs);
    obj->setProperty("delayFeedback", delayFeedback);
    obj->setProperty("delayHighDampHz", delayHighDampHz);
    obj->setProperty("delayMix", delayMix);

    // Reverb
    obj->setProperty("reverbEnabled", reverbEnabled);
    obj->setProperty("reverbRoomSize", reverbRoomSize);
    obj->setProperty("reverbDamping", reverbDamping);
    obj->setProperty("reverbWidth", reverbWidth);
    obj->setProperty("reverbMix", reverbMix);

    // Limiter
    obj->setProperty("limiterEnabled", limiterEnabled);
    obj->setProperty("limiterCeilingDb", limiterCeilingDb);

    return juce::var(obj);
}

PresetData PresetData::fromVar(const juce::var& v)
{
    PresetData p;
    if (!v.isObject())
        return p;

    p.name = v.getProperty("name", "Custom Preset").toString();

    p.autoTuneMode = v.getProperty("autoTuneMode", 0);
    p.rootKey = v.getProperty("rootKey", 0);
    p.scaleType = v.getProperty("scaleType", 1);
    p.retuneSpeedMs = static_cast<float>(v.getProperty("retuneSpeedMs", 80.0));
    p.correctionStrength = static_cast<float>(v.getProperty("correctionStrength", 0.85));
    p.humanize = static_cast<float>(v.getProperty("humanize", 0.40));

    p.gateEnabled = v.getProperty("gateEnabled", true);
    p.gateThresholdDb = static_cast<float>(v.getProperty("gateThresholdDb", -50.0));
    p.gateAttackMs = static_cast<float>(v.getProperty("gateAttackMs", 5.0));
    p.gateReleaseMs = static_cast<float>(v.getProperty("gateReleaseMs", 100.0));

    p.deEsserEnabled = v.getProperty("deEsserEnabled", true);
    p.deEsserFreqHz = static_cast<float>(v.getProperty("deEsserFreqHz", 6000.0));
    p.deEsserThreshDb = static_cast<float>(v.getProperty("deEsserThreshDb", -24.0));
    p.deEsserReductionDb = static_cast<float>(v.getProperty("deEsserReductionDb", 6.0));

    p.eqEnabled = v.getProperty("eqEnabled", true);
    p.eqLowGainDb = static_cast<float>(v.getProperty("eqLowGainDb", 0.0));
    p.eqLowFreqHz = static_cast<float>(v.getProperty("eqLowFreqHz", 120.0));
    p.eqMidGainDb = static_cast<float>(v.getProperty("eqMidGainDb", 0.0));
    p.eqMidFreqHz = static_cast<float>(v.getProperty("eqMidFreqHz", 2500.0));
    p.eqHighGainDb = static_cast<float>(v.getProperty("eqHighGainDb", 0.0));
    p.eqHighFreqHz = static_cast<float>(v.getProperty("eqHighFreqHz", 8500.0));

    p.compEnabled = v.getProperty("compEnabled", true);
    p.compThresholdDb = static_cast<float>(v.getProperty("compThresholdDb", -18.0));
    p.compRatio = static_cast<float>(v.getProperty("compRatio", 3.5));
    p.compAttackMs = static_cast<float>(v.getProperty("compAttackMs", 15.0));
    p.compReleaseMs = static_cast<float>(v.getProperty("compReleaseMs", 120.0));
    p.compMakeupDb = static_cast<float>(v.getProperty("compMakeupDb", 3.0));

    p.delayEnabled = v.getProperty("delayEnabled", true);
    p.delayTimeMs = static_cast<float>(v.getProperty("delayTimeMs", 260.0));
    p.delayFeedback = static_cast<float>(v.getProperty("delayFeedback", 0.25));
    p.delayHighDampHz = static_cast<float>(v.getProperty("delayHighDampHz", 4500.0));
    p.delayMix = static_cast<float>(v.getProperty("delayMix", 0.20));

    p.reverbEnabled = v.getProperty("reverbEnabled", true);
    p.reverbRoomSize = static_cast<float>(v.getProperty("reverbRoomSize", 0.65));
    p.reverbDamping = static_cast<float>(v.getProperty("reverbDamping", 0.40));
    p.reverbWidth = static_cast<float>(v.getProperty("reverbWidth", 0.80));
    p.reverbMix = static_cast<float>(v.getProperty("reverbMix", 0.22));

    p.limiterEnabled = v.getProperty("limiterEnabled", true);
    p.limiterCeilingDb = static_cast<float>(v.getProperty("limiterCeilingDb", -0.2));

    return p;
}

PresetManager::PresetManager()
{
    initFactoryPresets();
}

void PresetManager::initFactoryPresets()
{
    factoryPresets.clear();

    // 1. Bolero Warm
    {
        PresetData p;
        p.name = "Bolero Warm";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 120.0f;
        p.correctionStrength = 0.75f;
        p.humanize = 0.65f;

        p.gateEnabled = true;
        p.gateThresholdDb = -52.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 5500.0f;
        p.deEsserThreshDb = -22.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = 2.5f;   // Warm low
        p.eqLowFreqHz = 130.0f;
        p.eqMidGainDb = 0.5f;
        p.eqMidFreqHz = 1800.0f;
        p.eqHighGainDb = -1.0f; // Smooth warm top
        p.eqHighFreqHz = 9000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -20.0f;
        p.compRatio = 2.8f;
        p.compAttackMs = 25.0f;
        p.compReleaseMs = 180.0f;
        p.compMakeupDb = 3.5f;

        p.delayEnabled = true;
        p.delayTimeMs = 280.0f;
        p.delayFeedback = 0.20f;
        p.delayHighDampHz = 3500.0f;
        p.delayMix = 0.16f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.72f;
        p.reverbDamping = 0.50f;
        p.reverbMix = 0.26f;

        factoryPresets.push_back(p);
    }

    // 2. Bolero Sweet
    {
        PresetData p;
        p.name = "Bolero Sweet";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 150.0f;
        p.correctionStrength = 0.70f;
        p.humanize = 0.80f; // Very expressive

        p.gateEnabled = true;
        p.gateThresholdDb = -54.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 6200.0f;
        p.deEsserThreshDb = -25.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = 1.0f;
        p.eqLowFreqHz = 120.0f;
        p.eqMidGainDb = 1.2f;
        p.eqMidFreqHz = 2400.0f;
        p.eqHighGainDb = 2.0f; // Silky high
        p.eqHighFreqHz = 10000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -18.0f;
        p.compRatio = 3.0f;
        p.compAttackMs = 20.0f;
        p.compReleaseMs = 150.0f;
        p.compMakeupDb = 3.0f;

        p.delayEnabled = true;
        p.delayTimeMs = 190.0f; // Sweet slapback
        p.delayFeedback = 0.15f;
        p.delayHighDampHz = 4000.0f;
        p.delayMix = 0.14f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.62f;
        p.reverbDamping = 0.35f;
        p.reverbMix = 0.28f;

        factoryPresets.push_back(p);
    }

    // 3. Pop Natural
    {
        PresetData p;
        p.name = "Pop Natural";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 45.0f;
        p.correctionStrength = 0.85f;
        p.humanize = 0.35f;

        p.gateEnabled = true;
        p.gateThresholdDb = -48.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 6500.0f;
        p.deEsserThreshDb = -22.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = -1.0f; // Clean low rumble
        p.eqLowFreqHz = 100.0f;
        p.eqMidGainDb = 1.5f;  // Vocal presence
        p.eqMidFreqHz = 2800.0f;
        p.eqHighGainDb = 3.0f; // Air boost
        p.eqHighFreqHz = 10500.0f;

        p.compEnabled = true;
        p.compThresholdDb = -16.0f;
        p.compRatio = 4.0f;
        p.compAttackMs = 10.0f;
        p.compReleaseMs = 100.0f;
        p.compMakeupDb = 4.0f;

        p.delayEnabled = true;
        p.delayTimeMs = 320.0f;
        p.delayFeedback = 0.25f;
        p.delayMix = 0.18f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.60f;
        p.reverbDamping = 0.40f;
        p.reverbMix = 0.20f;

        factoryPresets.push_back(p);
    }

    // 4. Pop Hard Tune
    {
        PresetData p;
        p.name = "Pop Hard Tune";
        p.autoTuneMode = 1; // HardTune
        p.retuneSpeedMs = 0.0f;
        p.correctionStrength = 1.0f;
        p.humanize = 0.0f;

        p.gateEnabled = true;
        p.gateThresholdDb = -44.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 7000.0f;
        p.deEsserThreshDb = -20.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = -1.5f;
        p.eqLowFreqHz = 110.0f;
        p.eqMidGainDb = 2.0f;
        p.eqMidFreqHz = 3200.0f;
        p.eqHighGainDb = 4.0f;
        p.eqHighFreqHz = 11000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -15.0f;
        p.compRatio = 5.5f;
        p.compAttackMs = 5.0f;
        p.compReleaseMs = 80.0f;
        p.compMakeupDb = 5.0f;

        p.delayEnabled = true;
        p.delayTimeMs = 300.0f;
        p.delayFeedback = 0.35f;
        p.delayMix = 0.24f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.68f;
        p.reverbDamping = 0.25f;
        p.reverbMix = 0.25f;

        factoryPresets.push_back(p);
    }

    // 5. EDM Electronic
    {
        PresetData p;
        p.name = "EDM Electronic";
        p.autoTuneMode = 1; // HardTune
        p.retuneSpeedMs = 0.0f;
        p.correctionStrength = 1.0f;
        p.humanize = 0.0f;

        p.gateEnabled = true;
        p.gateThresholdDb = -42.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 7200.0f;
        p.deEsserThreshDb = -18.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = -3.0f;
        p.eqLowFreqHz = 130.0f;
        p.eqMidGainDb = 3.0f;
        p.eqMidFreqHz = 3500.0f;
        p.eqHighGainDb = 5.0f;
        p.eqHighFreqHz = 12000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -12.0f;
        p.compRatio = 7.0f;
        p.compAttackMs = 3.0f;
        p.compReleaseMs = 60.0f;
        p.compMakeupDb = 6.0f;

        p.delayEnabled = true;
        p.delayTimeMs = 375.0f; // dotted 8th feel
        p.delayFeedback = 0.45f;
        p.delayMix = 0.30f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.85f;
        p.reverbDamping = 0.20f;
        p.reverbMix = 0.32f;

        factoryPresets.push_back(p);
    }

    // 6. Male Deep
    {
        PresetData p;
        p.name = "Male Deep";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 90.0f;
        p.correctionStrength = 0.80f;
        p.humanize = 0.45f;

        p.gateEnabled = true;
        p.gateThresholdDb = -50.0f;

        p.deEsserEnabled = false;

        p.eqEnabled = true;
        p.eqLowGainDb = 2.0f;  // Body & warmth
        p.eqLowFreqHz = 160.0f;
        p.eqMidGainDb = 0.5f;
        p.eqMidFreqHz = 2200.0f;
        p.eqHighGainDb = 1.0f;
        p.eqHighFreqHz = 8000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -18.0f;
        p.compRatio = 3.2f;
        p.compAttackMs = 20.0f;
        p.compReleaseMs = 140.0f;
        p.compMakeupDb = 3.5f;

        p.delayEnabled = true;
        p.delayTimeMs = 240.0f;
        p.delayFeedback = 0.20f;
        p.delayMix = 0.15f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.60f;
        p.reverbDamping = 0.45f;
        p.reverbMix = 0.20f;

        factoryPresets.push_back(p);
    }

    // 7. Female Soft
    {
        PresetData p;
        p.name = "Female Soft";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 70.0f;
        p.correctionStrength = 0.80f;
        p.humanize = 0.55f;

        p.gateEnabled = true;
        p.gateThresholdDb = -52.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 6800.0f;
        p.deEsserThreshDb = -26.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = -1.0f;
        p.eqLowFreqHz = 110.0f;
        p.eqMidGainDb = 1.5f;
        p.eqMidFreqHz = 3000.0f;
        p.eqHighGainDb = 3.5f; // Silky breath & air
        p.eqHighFreqHz = 11000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -20.0f;
        p.compRatio = 3.0f;
        p.compAttackMs = 15.0f;
        p.compReleaseMs = 120.0f;
        p.compMakeupDb = 3.5f;

        p.delayEnabled = true;
        p.delayTimeMs = 270.0f;
        p.delayFeedback = 0.20f;
        p.delayMix = 0.16f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.68f;
        p.reverbDamping = 0.35f;
        p.reverbMix = 0.24f;

        factoryPresets.push_back(p);
    }

    // 8. Karaoke Clean
    {
        PresetData p;
        p.name = "Karaoke Clean";
        p.autoTuneMode = 0; // Natural
        p.retuneSpeedMs = 100.0f;
        p.correctionStrength = 0.70f;
        p.humanize = 0.60f;

        p.gateEnabled = true;
        p.gateThresholdDb = -50.0f;

        p.deEsserEnabled = true;
        p.deEsserFreqHz = 6000.0f;
        p.deEsserThreshDb = -24.0f;

        p.eqEnabled = true;
        p.eqLowGainDb = 1.0f;
        p.eqLowFreqHz = 120.0f;
        p.eqMidGainDb = 1.0f;
        p.eqMidFreqHz = 2500.0f;
        p.eqHighGainDb = 1.5f;
        p.eqHighFreqHz = 9000.0f;

        p.compEnabled = true;
        p.compThresholdDb = -18.0f;
        p.compRatio = 2.8f;
        p.compAttackMs = 15.0f;
        p.compReleaseMs = 120.0f;
        p.compMakeupDb = 2.5f;

        p.delayEnabled = true;
        p.delayTimeMs = 220.0f;
        p.delayFeedback = 0.18f;
        p.delayMix = 0.15f;

        p.reverbEnabled = true;
        p.reverbRoomSize = 0.58f;
        p.reverbDamping = 0.40f;
        p.reverbMix = 0.18f;

        factoryPresets.push_back(p);
    }
}

void PresetManager::applyPreset(const PresetData& preset, AutoTuneProcessor& autoTune, VocalEffectsRack& effects)
{
    // AutoTune
    autoTune.setMode(static_cast<AutoTuneMode>(preset.autoTuneMode));
    autoTune.setRootKey(preset.rootKey);
    autoTune.setScaleType(static_cast<ScaleType>(preset.scaleType));
    autoTune.setRetuneSpeedMs(preset.retuneSpeedMs);
    autoTune.setCorrectionStrength(preset.correctionStrength);
    autoTune.setHumanize(preset.humanize);

    // Gate
    effects.gate.enabled.store(preset.gateEnabled);
    effects.gate.thresholdDb.store(preset.gateThresholdDb);
    effects.gate.attackMs.store(preset.gateAttackMs);
    effects.gate.releaseMs.store(preset.gateReleaseMs);

    // DeEsser
    effects.deEsser.enabled.store(preset.deEsserEnabled);
    effects.deEsser.frequencyHz.store(preset.deEsserFreqHz);
    effects.deEsser.thresholdDb.store(preset.deEsserThreshDb);
    effects.deEsser.reductionDb.store(preset.deEsserReductionDb);

    // EQ
    effects.eq.enabled.store(preset.eqEnabled);
    effects.eq.lowGainDb.store(preset.eqLowGainDb);
    effects.eq.lowFreqHz.store(preset.eqLowFreqHz);
    effects.eq.midGainDb.store(preset.eqMidGainDb);
    effects.eq.midFreqHz.store(preset.eqMidFreqHz);
    effects.eq.highGainDb.store(preset.eqHighGainDb);
    effects.eq.highFreqHz.store(preset.eqHighFreqHz);

    // Compressor
    effects.comp.enabled.store(preset.compEnabled);
    effects.comp.thresholdDb.store(preset.compThresholdDb);
    effects.comp.ratio.store(preset.compRatio);
    effects.comp.attackMs.store(preset.compAttackMs);
    effects.comp.releaseMs.store(preset.compReleaseMs);
    effects.comp.makeupGainDb.store(preset.compMakeupDb);

    // Delay
    effects.delay.enabled.store(preset.delayEnabled);
    effects.delay.timeMs.store(preset.delayTimeMs);
    effects.delay.feedback.store(preset.delayFeedback);
    effects.delay.highDampHz.store(preset.delayHighDampHz);
    effects.delay.mix.store(preset.delayMix);

    // Reverb
    effects.reverb.enabled.store(preset.reverbEnabled);
    effects.reverb.roomSize.store(preset.reverbRoomSize);
    effects.reverb.damping.store(preset.reverbDamping);
    effects.reverb.width.store(preset.reverbWidth);
    effects.reverb.mix.store(preset.reverbMix);

    // Limiter
    effects.limiter.enabled.store(preset.limiterEnabled);
    effects.limiter.ceilingDb.store(preset.limiterCeilingDb);
}

PresetData PresetManager::captureCurrentState(const juce::String& name, const AutoTuneProcessor& autoTune, const VocalEffectsRack& effects)
{
    PresetData p;
    p.name = name;

    p.autoTuneMode = static_cast<int>(autoTune.getMode());
    p.rootKey = autoTune.getRootKey();
    p.scaleType = static_cast<int>(autoTune.getScaleType());
    p.retuneSpeedMs = autoTune.getRetuneSpeedMs();
    p.correctionStrength = autoTune.getCorrectionStrength();
    p.humanize = autoTune.getHumanize();

    p.gateEnabled = effects.gate.enabled.load();
    p.gateThresholdDb = effects.gate.thresholdDb.load();
    p.gateAttackMs = effects.gate.attackMs.load();
    p.gateReleaseMs = effects.gate.releaseMs.load();

    p.deEsserEnabled = effects.deEsser.enabled.load();
    p.deEsserFreqHz = effects.deEsser.frequencyHz.load();
    p.deEsserThreshDb = effects.deEsser.thresholdDb.load();
    p.deEsserReductionDb = effects.deEsser.reductionDb.load();

    p.eqEnabled = effects.eq.enabled.load();
    p.eqLowGainDb = effects.eq.lowGainDb.load();
    p.eqLowFreqHz = effects.eq.lowFreqHz.load();
    p.eqMidGainDb = effects.eq.midGainDb.load();
    p.eqMidFreqHz = effects.eq.midFreqHz.load();
    p.eqHighGainDb = effects.eq.highGainDb.load();
    p.eqHighFreqHz = effects.eq.highFreqHz.load();

    p.compEnabled = effects.comp.enabled.load();
    p.compThresholdDb = effects.comp.thresholdDb.load();
    p.compRatio = effects.comp.ratio.load();
    p.compAttackMs = effects.comp.attackMs.load();
    p.compReleaseMs = effects.comp.releaseMs.load();
    p.compMakeupDb = effects.comp.makeupGainDb.load();

    p.delayEnabled = effects.delay.enabled.load();
    p.delayTimeMs = effects.delay.timeMs.load();
    p.delayFeedback = effects.delay.feedback.load();
    p.delayHighDampHz = effects.delay.highDampHz.load();
    p.delayMix = effects.delay.mix.load();

    p.reverbEnabled = effects.reverb.enabled.load();
    p.reverbRoomSize = effects.reverb.roomSize.load();
    p.reverbDamping = effects.reverb.damping.load();
    p.reverbWidth = effects.reverb.width.load();
    p.reverbMix = effects.reverb.mix.load();

    p.limiterEnabled = effects.limiter.enabled.load();
    p.limiterCeilingDb = effects.limiter.ceilingDb.load();

    return p;
}

bool PresetManager::savePresetToFile(const PresetData& preset, const juce::File& file)
{
    auto jsonString = juce::JSON::toString(preset.toVar());
    return file.replaceWithText(jsonString);
}

bool PresetManager::loadPresetFromFile(const juce::File& file, PresetData& outPreset)
{
    if (!file.existsAsFile())
        return false;

    auto jsonString = file.loadFileAsString();
    auto varData = juce::JSON::parse(jsonString);
    if (varData.isVoid())
        return false;

    outPreset = PresetData::fromVar(varData);
    return true;
}

juce::File PresetManager::getPresetsDirectory() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                 .getChildFile("VocalLiveStudio")
                 .getChildFile("Presets");
    if (!dir.exists())
        dir.createDirectory();
    return dir;
}

} // namespace VocalStudio
