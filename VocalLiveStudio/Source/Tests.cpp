#include "NoteMapper.h"
#include "PitchDetector.h"
#include "PitchShifter.h"
#include "VocalEffects.h"
#include "PresetManager.h"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace VocalStudio;

static int testsPassed = 0;
static int testsFailed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "[FAILED] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            testsFailed++; \
        } else { \
            testsPassed++; \
        } \
    } while (0)

void testNoteMapper()
{
    std::cout << "Running NoteMapper tests...\n";

    // 440 Hz should be MIDI note 69 (A4)
    float midi440 = NoteMapper::frequencyToMidi(440.0f);
    TEST_ASSERT(std::abs(midi440 - 69.0f) < 0.01f, "440 Hz should map to MIDI 69.0");

    // MIDI 60 should be Middle C (~261.63 Hz)
    float freq60 = NoteMapper::midiToFrequency(60.0f);
    TEST_ASSERT(std::abs(freq60 - 261.63f) < 0.1f, "MIDI 60 should map to ~261.63 Hz");

    // MIDI to note name
    TEST_ASSERT(NoteMapper::midiToNoteName(69) == "A4", "MIDI 69 should be named A4");
    TEST_ASSERT(NoteMapper::midiToNoteName(60) == "C4", "MIDI 60 should be named C4");

    // Snap to C Major scale
    NoteMapper mapper;
    mapper.setRootKey(0); // C
    mapper.setScaleType(ScaleType::Major);

    // 442 Hz (sharp A4) should snap to A4 (440 Hz)
    auto resultA = mapper.snapToScale(442.0f);
    TEST_ASSERT(resultA.noteName == "A4", "442 Hz should snap to A4");
    TEST_ASSERT(resultA.detuneCents > 0.0f, "442 Hz should have positive detune cents relative to A4");

    // In C Major, C# is not present. 277.18 Hz (C#4) should snap to C4 or D4
    float freqCSharp = NoteMapper::midiToFrequency(61.0f);
    auto resultCS = mapper.snapToScale(freqCSharp);
    TEST_ASSERT(resultCS.noteName == "C4" || resultCS.noteName == "D4", "C#4 should snap to C4 or D4 in C Major");
}

void testPitchDetector()
{
    std::cout << "Running PitchDetector tests...\n";

    PitchDetector detector;
    double sampleRate = 48000.0;
    detector.prepare(sampleRate, 512);

    // Synthesize a pure 440 Hz sine wave for 0.2 seconds (~9600 samples)
    float testFreq = 440.0f;
    int numSamples = static_cast<int>(sampleRate * 0.2);
    std::vector<float> sineWave(static_cast<size_t>(numSamples));
    for (int i = 0; i < numSamples; ++i)
    {
        sineWave[static_cast<size_t>(i)] = 0.7f * std::sin(2.0f * 3.14159265f * testFreq * static_cast<float>(i) / static_cast<float>(sampleRate));
    }

    // Feed in blocks of 256 samples
    int blockSize = 256;
    PitchDetectionResult lastRes;
    for (int offset = 0; offset + blockSize <= numSamples; offset += blockSize)
    {
        lastRes = detector.processBlock(&sineWave[static_cast<size_t>(offset)], blockSize);
    }

    TEST_ASSERT(lastRes.isVoiced, "440 Hz sine wave should be detected as voiced");
    TEST_ASSERT(std::abs(lastRes.pitchHz - 440.0f) < 3.0f, "Detected pitch should be close to 440 Hz");
    TEST_ASSERT(lastRes.confidence > 0.85f, "Sine wave should have high periodicity confidence (> 0.85)");

    // Test silence gating
    std::vector<float> silence(2048, 0.0f);
    auto silenceRes = detector.processBlock(silence.data(), 512);
    TEST_ASSERT(!silenceRes.isVoiced, "Silence should not be voiced");
}

void testPitchShifter()
{
    std::cout << "Running PitchShifter tests...\n";

    PitchShifter shifter;
    shifter.prepare(48000.0, 512, 2);

    int blockSize = 512;
    std::vector<float> left(static_cast<size_t>(blockSize));
    std::vector<float> right(static_cast<size_t>(blockSize));

    for (int i = 0; i < blockSize; ++i)
    {
        float val = 0.5f * std::sin(2.0f * 3.14159265f * 200.0f * static_cast<float>(i) / 48000.0f);
        left[static_cast<size_t>(i)] = val;
        right[static_cast<size_t>(i)] = val;
    }

    float* channels[2] = { left.data(), right.data() };
    shifter.setShiftRatio(1.25f); // Pitch up by major third
    shifter.process(channels, 2, blockSize);

    // Verify output is clean, finite, no NaNs or Infs
    bool allFinite = true;
    for (int i = 0; i < blockSize; ++i)
    {
        if (!std::isfinite(left[static_cast<size_t>(i)]) || !std::isfinite(right[static_cast<size_t>(i)]))
        {
            allFinite = false;
            break;
        }
    }
    TEST_ASSERT(allFinite, "PitchShifter output should be finite numbers without NaNs");
}

void testVocalEffects()
{
    std::cout << "Running VocalEffects tests...\n";

    VocalEffectsRack rack;
    rack.prepare(48000.0, 512, 2);

    // Test Limiter brickwall
    int blockSize = 512;
    std::vector<float> left(static_cast<size_t>(blockSize), 3.5f); // Extremely loud signal (+11 dBFS)
    std::vector<float> right(static_cast<size_t>(blockSize), 3.5f);
    float* channels[2] = { left.data(), right.data() };

    // Process through limiter
    rack.limiter.process(channels, 2, blockSize);

    float maxSample = 0.0f;
    for (int i = 0; i < blockSize; ++i)
    {
        maxSample = std::max(maxSample, std::abs(left[static_cast<size_t>(i)]));
        maxSample = std::max(maxSample, std::abs(right[static_cast<size_t>(i)]));
    }

    TEST_ASSERT(maxSample <= 1.0f, "Limiter must clamp audio output below 1.0f (0 dBFS)");

    // Test Noise Gate on silence
    std::vector<float> quietL(static_cast<size_t>(blockSize), 0.0001f); // -80 dBFS
    std::vector<float> quietR(static_cast<size_t>(blockSize), 0.0001f);
    float* quietChannels[2] = { quietL.data(), quietR.data() };

    rack.gate.thresholdDb.store(-50.0f);
    rack.gate.holdMs.store(0.0f);
    for (int b = 0; b < 10; ++b)
        rack.gate.process(quietChannels, 2, blockSize);

    TEST_ASSERT(std::abs(quietL[0]) < 0.00005f, "Noise gate should attenuate signals below threshold");
}

void testPresetManager()
{
    std::cout << "Running PresetManager tests...\n";

    PresetManager pm;
    const auto& factory = pm.getFactoryPresets();
    TEST_ASSERT(factory.size() == 8, "PresetManager should have 8 factory presets");

    // Verify preset serialization round-trip
    const auto& p1 = factory[0];
    auto varData = p1.toVar();
    auto pRoundtrip = PresetData::fromVar(varData);

    TEST_ASSERT(pRoundtrip.name == p1.name, "Preset name should match after round-trip");
    TEST_ASSERT(std::abs(pRoundtrip.retuneSpeedMs - p1.retuneSpeedMs) < 0.001f, "Retune speed should match");
    TEST_ASSERT(pRoundtrip.autoTuneMode == p1.autoTuneMode, "AutoTune mode should match");
}

int main()
{
    std::cout << "===========================================\n";
    std::cout << " VOCAL LIVE STUDIO - UNIT TEST SUITE\n";
    std::cout << "===========================================\n";

    testNoteMapper();
    testPitchDetector();
    testPitchShifter();
    testVocalEffects();
    testPresetManager();

    std::cout << "\n-------------------------------------------\n";
    std::cout << "Tests Passed: " << testsPassed << "\n";
    std::cout << "Tests Failed: " << testsFailed << "\n";
    std::cout << "-------------------------------------------\n";

    if (testsFailed == 0)
    {
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!\n";
        return 0;
    }
    else
    {
        std::cerr << "SOME TESTS FAILED!\n";
        return 1;
    }
}
