#include "VocalEffects.h"
#include <algorithm>

namespace VocalStudio
{

static constexpr float PI_F = 3.14159265358979323846f;

void Biquad::setLowShelf(float sampleRate, float freqHz, float gainDb, float Q)
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI_F * juce::jlimit(20.0f, sampleRate * 0.49f, freqHz) / sampleRate;
    float cos_w0 = std::cos(w0);
    float sin_w0 = std::sin(w0);
    float alpha = sin_w0 / (2.0f * Q);
    float two_sqrt_A_alpha = 2.0f * std::sqrt(A) * alpha;

    float a0 = (A + 1.0f) + (A - 1.0f) * cos_w0 + two_sqrt_A_alpha;
    float inv_a0 = 1.0f / a0;

    b0 = (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 + two_sqrt_A_alpha)) * inv_a0;
    b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos_w0)) * inv_a0;
    b2 = (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 - two_sqrt_A_alpha)) * inv_a0;
    a1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cos_w0)) * inv_a0;
    a2 = ((A + 1.0f) + (A - 1.0f) * cos_w0 - two_sqrt_A_alpha) * inv_a0;
}

void Biquad::setHighShelf(float sampleRate, float freqHz, float gainDb, float Q)
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI_F * juce::jlimit(20.0f, sampleRate * 0.49f, freqHz) / sampleRate;
    float cos_w0 = std::cos(w0);
    float sin_w0 = std::sin(w0);
    float alpha = sin_w0 / (2.0f * Q);
    float two_sqrt_A_alpha = 2.0f * std::sqrt(A) * alpha;

    float a0 = (A + 1.0f) - (A - 1.0f) * cos_w0 + two_sqrt_A_alpha;
    float inv_a0 = 1.0f / a0;

    b0 = (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 + two_sqrt_A_alpha)) * inv_a0;
    b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_w0)) * inv_a0;
    b2 = (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 - two_sqrt_A_alpha)) * inv_a0;
    a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cos_w0)) * inv_a0;
    a2 = ((A + 1.0f) - (A - 1.0f) * cos_w0 - two_sqrt_A_alpha) * inv_a0;
}

void Biquad::setPeaking(float sampleRate, float freqHz, float gainDb, float Q)
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * PI_F * juce::jlimit(20.0f, sampleRate * 0.49f, freqHz) / sampleRate;
    float cos_w0 = std::cos(w0);
    float sin_w0 = std::sin(w0);
    float alpha = sin_w0 / (2.0f * Q);

    float a0 = 1.0f + alpha / A;
    float inv_a0 = 1.0f / a0;

    b0 = (1.0f + alpha * A) * inv_a0;
    b1 = (-2.0f * cos_w0) * inv_a0;
    b2 = (1.0f - alpha * A) * inv_a0;
    a1 = (-2.0f * cos_w0) * inv_a0;
    a2 = (1.0f - alpha / A) * inv_a0;
}

void Biquad::setBandPass(float sampleRate, float freqHz, float Q)
{
    float w0 = 2.0f * PI_F * juce::jlimit(20.0f, sampleRate * 0.49f, freqHz) / sampleRate;
    float cos_w0 = std::cos(w0);
    float sin_w0 = std::sin(w0);
    float alpha = sin_w0 / (2.0f * Q);

    float a0 = 1.0f + alpha;
    float inv_a0 = 1.0f / a0;

    b0 = (sin_w0 * 0.5f) * inv_a0;
    b1 = 0.0f;
    b2 = (-sin_w0 * 0.5f) * inv_a0;
    a1 = (-2.0f * cos_w0) * inv_a0;
    a2 = (1.0f - alpha) * inv_a0;
}

void Biquad::setLowPass(float sampleRate, float freqHz, float Q)
{
    float w0 = 2.0f * PI_F * juce::jlimit(20.0f, sampleRate * 0.49f, freqHz) / sampleRate;
    float cos_w0 = std::cos(w0);
    float sin_w0 = std::sin(w0);
    float alpha = sin_w0 / (2.0f * Q);

    float a0 = 1.0f + alpha;
    float inv_a0 = 1.0f / a0;

    b0 = (1.0f - cos_w0) * 0.5f * inv_a0;
    b1 = (1.0f - cos_w0) * inv_a0;
    b2 = (1.0f - cos_w0) * 0.5f * inv_a0;
    a1 = (-2.0f * cos_w0) * inv_a0;
    a2 = (1.0f - alpha) * inv_a0;
}

// -------------------------------------------------------------
// 1. Noise Gate Implementation
// -------------------------------------------------------------
void NoiseGate::prepare(double sampleRate)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    reset();
}

void NoiseGate::reset()
{
    envelope = 0.0f;
    holdCounter = 0;
}

void NoiseGate::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    float threshLin = std::pow(10.0f, thresholdDb.load(std::memory_order_relaxed) / 20.0f);
    float att = std::exp(-1.0f / (attackMs.load(std::memory_order_relaxed) * 0.001f * static_cast<float>(currentSampleRate)));
    float rel = std::exp(-1.0f / (releaseMs.load(std::memory_order_relaxed) * 0.001f * static_cast<float>(currentSampleRate)));
    int holdSamples = static_cast<int>(holdMs.load(std::memory_order_relaxed) * 0.001f * static_cast<float>(currentSampleRate));

    for (int i = 0; i < numSamples; ++i)
    {
        // Max absolute sample across channels
        float peak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float absSample = std::abs(channelData[ch][i]);
            if (absSample > peak)
                peak = absSample;
        }

        float targetGain = 0.0f;
        if (peak >= threshLin)
        {
            targetGain = 1.0f;
            holdCounter = holdSamples;
        }
        else if (holdCounter > 0)
        {
            holdCounter--;
            targetGain = 1.0f;
        }

        // Smooth gain change
        if (targetGain > envelope)
            envelope = att * envelope + (1.0f - att) * targetGain;
        else
            envelope = rel * envelope + (1.0f - rel) * targetGain;

        for (int ch = 0; ch < numChannels; ++ch)
            channelData[ch][i] *= envelope;
    }
}

// -------------------------------------------------------------
// 2. De-Esser Implementation
// -------------------------------------------------------------
void DeEsser::prepare(double sampleRate, int /*maxBlockSize*/)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    sidechainFilter[0].setBandPass(static_cast<float>(currentSampleRate), frequencyHz.load(), 1.4f);
    sidechainFilter[1].setBandPass(static_cast<float>(currentSampleRate), frequencyHz.load(), 1.4f);
    reset();
}

void DeEsser::reset()
{
    envelope = 0.0f;
    sidechainFilter[0].reset();
    sidechainFilter[1].reset();
}

void DeEsser::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    float threshDb = thresholdDb.load(std::memory_order_relaxed);
    float maxRedDb = reductionDb.load(std::memory_order_relaxed);
    float att = std::exp(-1.0f / (0.001f * static_cast<float>(currentSampleRate))); // 1ms attack
    float rel = std::exp(-1.0f / (0.035f * static_cast<float>(currentSampleRate))); // 35ms release

    for (int i = 0; i < numSamples; ++i)
    {
        float sibilanceEnergy = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sideSample = sidechainFilter[ch < 2 ? ch : 0].process(channelData[ch][i]);
            sibilanceEnergy = std::max(sibilanceEnergy, std::abs(sideSample));
        }

        float levelDb = (sibilanceEnergy > 1e-5f) ? (20.0f * std::log10(sibilanceEnergy)) : -100.0f;

        float targetGain = 1.0f;
        if (levelDb > threshDb)
        {
            float excess = levelDb - threshDb;
            float attenuation = std::min(excess * 0.75f, maxRedDb);
            targetGain = std::pow(10.0f, -attenuation / 20.0f);
        }

        if (targetGain < envelope)
            envelope = att * envelope + (1.0f - att) * targetGain;
        else
            envelope = rel * envelope + (1.0f - rel) * targetGain;

        for (int ch = 0; ch < numChannels; ++ch)
            channelData[ch][i] *= envelope;
    }
}

// -------------------------------------------------------------
// 3. 3-Band Parametric EQ Implementation
// -------------------------------------------------------------
void ParametricEQ::prepare(double sampleRate)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    reset();
}

void ParametricEQ::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        lowShelf[ch].reset();
        midPeak[ch].reset();
        highShelf[ch].reset();
    }
    lastLowGain = 999.0f;
    lastMidGain = 999.0f;
    lastHighGain = 999.0f;
}

void ParametricEQ::updateCoefficients()
{
    float lg = lowGainDb.load(std::memory_order_relaxed);
    float lf = lowFreqHz.load(std::memory_order_relaxed);
    float mg = midGainDb.load(std::memory_order_relaxed);
    float mf = midFreqHz.load(std::memory_order_relaxed);
    float hg = highGainDb.load(std::memory_order_relaxed);
    float hf = highFreqHz.load(std::memory_order_relaxed);

    float sr = static_cast<float>(currentSampleRate);

    if (lg != lastLowGain || lf != lastLowFreq)
    {
        lastLowGain = lg;
        lastLowFreq = lf;
        lowShelf[0].setLowShelf(sr, lf, lg, 0.7071f);
        lowShelf[1].setLowShelf(sr, lf, lg, 0.7071f);
    }
    if (mg != lastMidGain || mf != lastMidFreq)
    {
        lastMidGain = mg;
        lastMidFreq = mf;
        midPeak[0].setPeaking(sr, mf, mg, 1.0f);
        midPeak[1].setPeaking(sr, mf, mg, 1.0f);
    }
    if (hg != lastHighGain || hf != lastHighFreq)
    {
        lastHighGain = hg;
        lastHighFreq = hf;
        highShelf[0].setHighShelf(sr, hf, hg, 0.7071f);
        highShelf[1].setHighShelf(sr, hf, hg, 0.7071f);
    }
}

void ParametricEQ::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    updateCoefficients();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        int fIndex = ch < 2 ? ch : 0;
        auto* data = channelData[ch];
        for (int i = 0; i < numSamples; ++i)
        {
            float s = lowShelf[fIndex].process(data[i]);
            s = midPeak[fIndex].process(s);
            s = highShelf[fIndex].process(s);
            data[i] = s;
        }
    }
}

// -------------------------------------------------------------
// 4. Vocal Compressor Implementation
// -------------------------------------------------------------
void VocalCompressor::prepare(double sampleRate)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    reset();
}

void VocalCompressor::reset()
{
    envelopeDb = -90.0f;
}

void VocalCompressor::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    float thresh = thresholdDb.load(std::memory_order_relaxed);
    float rat = ratio.load(std::memory_order_relaxed);
    float makeup = std::pow(10.0f, makeupGainDb.load(std::memory_order_relaxed) / 20.0f);
    float att = std::exp(-1.0f / (attackMs.load(std::memory_order_relaxed) * 0.001f * static_cast<float>(currentSampleRate)));
    float rel = std::exp(-1.0f / (releaseMs.load(std::memory_order_relaxed) * 0.001f * static_cast<float>(currentSampleRate)));

    float kneeWidth = 6.0f; // Soft knee (dB)

    for (int i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            peak = std::max(peak, std::abs(channelData[ch][i]));

        float inputDb = (peak > 1e-6f) ? (20.0f * std::log10(peak)) : -120.0f;

        // Smooth peak detector
        if (inputDb > envelopeDb)
            envelopeDb = att * envelopeDb + (1.0f - att) * inputDb;
        else
            envelopeDb = rel * envelopeDb + (1.0f - rel) * inputDb;

        // Gain computer with soft knee
        float gainReductionDb = 0.0f;
        float diff = envelopeDb - thresh;
        if (diff > kneeWidth / 2.0f)
        {
            gainReductionDb = diff * (1.0f - 1.0f / rat);
        }
        else if (diff > -kneeWidth / 2.0f)
        {
            float kneeDiff = diff + kneeWidth / 2.0f;
            gainReductionDb = (kneeDiff * kneeDiff / (2.0f * kneeWidth)) * (1.0f - 1.0f / rat);
        }

        float gainLin = std::pow(10.0f, -gainReductionDb / 20.0f) * makeup;

        for (int ch = 0; ch < numChannels; ++ch)
            channelData[ch][i] *= gainLin;
    }
}

// -------------------------------------------------------------
// 5. Echo / Delay Implementation
// -------------------------------------------------------------
void VocalDelay::prepare(double sampleRate, int /*maxBlockSize*/)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    maxDelaySamples = static_cast<int>(currentSampleRate * 1.5); // Up to 1.5s delay
    delayLines.resize(2);
    for (auto& dl : delayLines)
        dl.assign(static_cast<size_t>(maxDelaySamples), 0.0f);

    dampFilter[0].setLowPass(static_cast<float>(currentSampleRate), highDampHz.load(), 0.7071f);
    dampFilter[1].setLowPass(static_cast<float>(currentSampleRate), highDampHz.load(), 0.7071f);
    reset();
}

void VocalDelay::reset()
{
    writeIndex = 0;
    for (auto& dl : delayLines)
        std::fill(dl.begin(), dl.end(), 0.0f);
    dampFilter[0].reset();
    dampFilter[1].reset();
}

void VocalDelay::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    float tMs = timeMs.load(std::memory_order_relaxed);
    float fb = juce::jlimit(0.0f, 0.85f, feedback.load(std::memory_order_relaxed));
    float wet = mix.load(std::memory_order_relaxed);
    float dry = 1.0f - wet * 0.5f;

    int delaySamplesL = static_cast<int>(tMs * 0.001f * static_cast<float>(currentSampleRate));
    // Stereo ping-pong offset (right channel 20% longer for wide imaging)
    int delaySamplesR = static_cast<int>(tMs * 1.25f * 0.001f * static_cast<float>(currentSampleRate));

    delaySamplesL = juce::jlimit(1, maxDelaySamples - 1, delaySamplesL);
    delaySamplesR = juce::jlimit(1, maxDelaySamples - 1, delaySamplesR);

    for (int i = 0; i < numSamples; ++i)
    {
        int readPosL = (writeIndex - delaySamplesL + maxDelaySamples) % maxDelaySamples;
        int readPosR = (writeIndex - delaySamplesR + maxDelaySamples) % maxDelaySamples;

        float delayedL = delayLines[0][static_cast<size_t>(readPosL)];
        float delayedR = delayLines[1][static_cast<size_t>(readPosR)];

        // Filter damp in feedback
        delayedL = dampFilter[0].process(delayedL);
        delayedR = dampFilter[1].process(delayedR);

        float inL = channelData[0][i];
        float inR = (numChannels > 1) ? channelData[1][i] : inL;

        // Ping-pong crossfeed feedback
        delayLines[0][static_cast<size_t>(writeIndex)] = inL + delayedR * fb;
        delayLines[1][static_cast<size_t>(writeIndex)] = inR + delayedL * fb;

        channelData[0][i] = inL * dry + delayedL * wet;
        if (numChannels > 1)
            channelData[1][i] = inR * dry + delayedR * wet;

        writeIndex = (writeIndex + 1) % maxDelaySamples;
    }
}

// -------------------------------------------------------------
// 6. Studio Reverb Implementation
// -------------------------------------------------------------
void VocalReverb::prepare(double sampleRate)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    juceReverb.setSampleRate(currentSampleRate);
    reset();
}

void VocalReverb::reset()
{
    juceReverb.reset();
}

void VocalReverb::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    params.roomSize = juce::jlimit(0.0f, 1.0f, roomSize.load(std::memory_order_relaxed));
    params.damping = juce::jlimit(0.0f, 1.0f, damping.load(std::memory_order_relaxed));
    params.width = juce::jlimit(0.0f, 1.0f, width.load(std::memory_order_relaxed));
    params.wetLevel = juce::jlimit(0.0f, 1.0f, mix.load(std::memory_order_relaxed));
    params.dryLevel = 1.0f - params.wetLevel * 0.4f;
    params.freezeMode = 0.0f;

    juceReverb.setParameters(params);

    if (numChannels >= 2)
    {
        juceReverb.processStereo(channelData[0], channelData[1], numSamples);
    }
    else if (numChannels == 1)
    {
        juceReverb.processMono(channelData[0], numSamples);
    }
}

// -------------------------------------------------------------
// 7. Output Limiter Implementation
// -------------------------------------------------------------
void VocalLimiter::prepare(double sampleRate)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    reset();
}

void VocalLimiter::reset()
{
    peakEnvelope = 0.0f;
    clipped.store(false);
}

void VocalLimiter::process(float* const* channelData, int numChannels, int numSamples)
{
    if (!enabled.load(std::memory_order_relaxed))
        return;

    float ceiling = std::pow(10.0f, ceilingDb.load(std::memory_order_relaxed) / 20.0f);
    float rel = std::exp(-1.0f / (0.050f * static_cast<float>(currentSampleRate))); // 50ms release

    for (int i = 0; i < numSamples; ++i)
    {
        float maxSample = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            maxSample = std::max(maxSample, std::abs(channelData[ch][i]));

        if (maxSample > peakEnvelope)
            peakEnvelope = maxSample;
        else
            peakEnvelope = rel * peakEnvelope + (1.0f - rel) * maxSample;

        if (maxSample >= 0.999f)
            clipped.store(true, std::memory_order_relaxed);

        float gain = 1.0f;
        if (peakEnvelope > ceiling)
            gain = ceiling / peakEnvelope;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float s = channelData[ch][i] * gain;
            // Soft-saturation tanh curve as final anti-clipping brickwall
            if (s > ceiling)
                s = ceiling + (1.0f - ceiling) * std::tanh((s - ceiling) / (1.0f - ceiling));
            else if (s < -ceiling)
                s = -ceiling + (1.0f - ceiling) * std::tanh((s + ceiling) / (1.0f - ceiling));

            channelData[ch][i] = s;
        }
    }
}

// -------------------------------------------------------------
// Master Vocal Effects Rack
// -------------------------------------------------------------
VocalEffectsRack::VocalEffectsRack()
{
    prepare(48000.0, 512, 2);
}

void VocalEffectsRack::prepare(double sampleRate, int maxBlockSize, int numChannels)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    activeChannels = std::max(1, numChannels);

    gate.prepare(currentSampleRate);
    deEsser.prepare(currentSampleRate, maxBlockSize);
    eq.prepare(currentSampleRate);
    comp.prepare(currentSampleRate);
    delay.prepare(currentSampleRate, maxBlockSize);
    reverb.prepare(currentSampleRate);
    limiter.prepare(currentSampleRate);
}

void VocalEffectsRack::reset()
{
    gate.reset();
    deEsser.reset();
    eq.reset();
    comp.reset();
    delay.reset();
    reverb.reset();
    limiter.reset();
}

void VocalEffectsRack::process(float* const* channelData, int numChannels, int numSamples)
{
    // Chain: Noise Gate -> De-Esser -> EQ -> Compressor -> Delay -> Reverb -> Limiter
    gate.process(channelData, numChannels, numSamples);
    deEsser.process(channelData, numChannels, numSamples);
    eq.process(channelData, numChannels, numSamples);
    comp.process(channelData, numChannels, numSamples);
    delay.process(channelData, numChannels, numSamples);
    reverb.process(channelData, numChannels, numSamples);
    limiter.process(channelData, numChannels, numSamples);
}

} // namespace VocalStudio
