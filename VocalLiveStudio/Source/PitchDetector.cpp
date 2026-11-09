#include "PitchDetector.h"
#include <algorithm>

namespace VocalStudio
{

PitchDetector::PitchDetector()
{
    prepare(48000.0, 512);
}

void PitchDetector::setFrequencyRange(float minHz, float maxHz)
{
    minFrequency = juce::jlimit(40.0f, 300.0f, minHz);
    maxFrequency = juce::jlimit(300.0f, 2000.0f, maxHz);

    if (currentSampleRate > 0.0)
    {
        maxTau = juce::jlimit(20, windowSize - 1, static_cast<int>(std::round(currentSampleRate / minFrequency)));
        minTau = juce::jlimit(2, maxTau - 1, static_cast<int>(std::round(currentSampleRate / maxFrequency)));
    }
}

void PitchDetector::setSilenceThreshold(float silenceDb) noexcept
{
    silenceLinearThreshold = std::pow(10.0f, silenceDb / 20.0f);
}

void PitchDetector::prepare(double sampleRate, int /*maxBlockSize*/)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;

    windowSize = 1024;
    hopSize = 128; // ~2.7ms at 48kHz
    hopCounter = 0;

    ringBufferSize = 4096;
    ringBuffer.assign(static_cast<size_t>(ringBufferSize), 0.0f);
    ringWritePos = 0;

    yinBuffer.assign(static_cast<size_t>(windowSize), 0.0f);
    analysisWindow.assign(static_cast<size_t>(windowSize * 2), 0.0f);

    setFrequencyRange(minFrequency, maxFrequency);
    reset();
}

void PitchDetector::reset()
{
    std::fill(ringBuffer.begin(), ringBuffer.end(), 0.0f);
    ringWritePos = 0;
    hopCounter = 0;
    pitchHistory.fill(0.0f);
    historyIndex = 0;
    latestResult = PitchDetectionResult {};
}

PitchDetectionResult PitchDetector::processBlock(const float* channelData, int numSamples)
{
    if (channelData == nullptr || numSamples <= 0)
        return latestResult;

    // Fast RMS computation of incoming block
    float sumSq = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float s = channelData[i];
        sumSq += s * s;

        // Push into circular buffer
        ringBuffer[static_cast<size_t>(ringWritePos)] = s;
        ringWritePos = (ringWritePos + 1) % ringBufferSize;

        hopCounter++;
        if (hopCounter >= hopSize)
        {
            hopCounter = 0;
            runYinAnalysis();
        }
    }

    latestResult.rmsLevel = std::sqrt(sumSq / static_cast<float>(numSamples));
    if (latestResult.rmsLevel < silenceLinearThreshold)
    {
        latestResult.isVoiced = false;
        latestResult.confidence = 0.0f;
    }

    return latestResult;
}

void PitchDetector::runYinAnalysis()
{
    // Extract recent windowSize * 2 samples from circular buffer
    int readStart = (ringWritePos - windowSize * 2 + ringBufferSize) % ringBufferSize;
    for (int i = 0; i < windowSize * 2; ++i)
    {
        analysisWindow[static_cast<size_t>(i)] = ringBuffer[static_cast<size_t>((readStart + i) % ringBufferSize)];
    }

    // Check RMS of analysis window
    float windowSumSq = 0.0f;
    for (int i = 0; i < windowSize; ++i)
    {
        float val = analysisWindow[static_cast<size_t>(i)];
        windowSumSq += val * val;
    }
    float windowRms = std::sqrt(windowSumSq / static_cast<float>(windowSize));
    if (windowRms < silenceLinearThreshold)
    {
        latestResult.pitchHz = 0.0f;
        latestResult.confidence = 0.0f;
        latestResult.isVoiced = false;
        return;
    }

    // Step 1: Difference Function
    // d_t(tau) = sum_{j=0}^{W-1} (x[j] - x[j + tau])^2
    yinBuffer[0] = 0.0f;
    for (int tau = 1; tau <= maxTau; ++tau)
    {
        float diffSum = 0.0f;
        for (int j = 0; j < windowSize; ++j)
        {
            float delta = analysisWindow[static_cast<size_t>(j)] - analysisWindow[static_cast<size_t>(j + tau)];
            diffSum += delta * delta;
        }
        yinBuffer[static_cast<size_t>(tau)] = diffSum;
    }

    // Step 2: Cumulative Mean Normalized Difference Function (CMNDF)
    yinBuffer[0] = 1.0f;
    float runningSum = 0.0f;
    for (int tau = 1; tau <= maxTau; ++tau)
    {
        runningSum += yinBuffer[static_cast<size_t>(tau)];
        if (runningSum > 1e-6f)
            yinBuffer[static_cast<size_t>(tau)] *= static_cast<float>(tau) / runningSum;
        else
            yinBuffer[static_cast<size_t>(tau)] = 1.0f;
    }

    // Step 3: Absolute Thresholding
    int tauEstimate = -1;
    for (int tau = minTau; tau <= maxTau; ++tau)
    {
        if (yinBuffer[static_cast<size_t>(tau)] < yinThreshold)
        {
            // Find local minimum in this dip
            while (tau + 1 <= maxTau && yinBuffer[static_cast<size_t>(tau + 1)] < yinBuffer[static_cast<size_t>(tau)])
            {
                tau++;
            }
            tauEstimate = tau;
            break;
        }
    }

    // If no pitch below threshold, find global minimum in search range
    if (tauEstimate == -1)
    {
        float minVal = 100.0f;
        for (int tau = minTau; tau <= maxTau; ++tau)
        {
            if (yinBuffer[static_cast<size_t>(tau)] < minVal)
            {
                minVal = yinBuffer[static_cast<size_t>(tau)];
                tauEstimate = tau;
            }
        }
    }

    if (tauEstimate <= 0 || tauEstimate >= maxTau)
    {
        latestResult.pitchHz = 0.0f;
        latestResult.confidence = 0.0f;
        latestResult.isVoiced = false;
        return;
    }

    // Step 4: Parabolic Interpolation for Sub-Sample Accuracy
    float x0 = static_cast<float>(tauEstimate);
    float y1 = yinBuffer[static_cast<size_t>(tauEstimate - 1)];
    float y2 = yinBuffer[static_cast<size_t>(tauEstimate)];
    float y3 = yinBuffer[static_cast<size_t>(tauEstimate + 1)];

    float denom = (2.0f * (2.0f * y2 - y1 - y3));
    float delta = 0.0f;
    if (std::abs(denom) > 1e-6f)
    {
        delta = (y3 - y1) / denom;
        if (delta < -1.0f || delta > 1.0f)
            delta = 0.0f;
    }

    float finalTau = x0 + delta;
    float rawPitch = (finalTau > 0.0f) ? static_cast<float>(currentSampleRate / finalTau) : 0.0f;
    float cmndfAtMin = y2;
    float confidence = juce::jlimit(0.0f, 1.0f, 1.0f - cmndfAtMin);

    // Voiced decision
    bool voiced = (confidence >= 0.65f) && (rawPitch >= minFrequency) && (rawPitch <= maxFrequency);

    if (voiced)
    {
        // 3-point median filter to eliminate octave jumps and false transients
        pitchHistory[static_cast<size_t>(historyIndex)] = rawPitch;
        historyIndex = (historyIndex + 1) % 3;

        std::array<float, 3> sorted = pitchHistory;
        std::sort(sorted.begin(), sorted.end());
        float medianPitch = sorted[1];

        latestResult.pitchHz = (medianPitch > 0.0f) ? medianPitch : rawPitch;
        latestResult.confidence = confidence;
        latestResult.isVoiced = true;
    }
    else
    {
        latestResult.isVoiced = false;
        latestResult.confidence = confidence;
        // Keep pitch at 0 or decay
        latestResult.pitchHz = 0.0f;
    }
}

} // namespace VocalStudio
