#include "KaraokePlayer.h"
#include <cmath>

namespace VocalStudio
{

KaraokePlayer::KaraokePlayer()
{
    formatManager.registerBasicFormats(); // Registers WAV, AIFF, MP3, FLAC, OGG
}

KaraokePlayer::~KaraokePlayer()
{
    unloadFile();
}

void KaraokePlayer::prepareToPlay(double sampleRate, int samplesPerBlockExpected)
{
    std::lock_guard<std::mutex> lock(loadMutex);
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;
    currentBlockSize = samplesPerBlockExpected;

    if (resampler != nullptr)
        resampler->prepareToPlay(samplesPerBlockExpected, currentSampleRate);
    else
        transportSource.prepareToPlay(samplesPerBlockExpected, currentSampleRate);
}

void KaraokePlayer::releaseResources()
{
    std::lock_guard<std::mutex> lock(loadMutex);
    if (resampler != nullptr)
        resampler->releaseResources();
    else
        transportSource.releaseResources();
}

bool KaraokePlayer::loadFile(const juce::File& audioFile)
{
    if (!audioFile.existsAsFile())
        return false;

    auto* reader = formatManager.createReaderFor(audioFile);
    if (reader == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(loadMutex);

    transportSource.stop();
    if (resampler != nullptr)
        resampler->releaseResources();
    transportSource.releaseResources();

    currentReaderSource.reset(new juce::AudioFormatReaderSource(reader, true));
    transportSource.setSource(currentReaderSource.get(), 0, nullptr, reader->sampleRate);

    resampler.reset(new juce::ResamplingAudioSource(&transportSource, false, 2));
    resampler->prepareToPlay(currentBlockSize, currentSampleRate);

    float speed = tempoRatio.load(std::memory_order_relaxed);
    int pitch = pitchTranspose.load(std::memory_order_relaxed);
    float pitchFactor = std::pow(2.0f, static_cast<float>(pitch) / 12.0f);
    resampler->setResamplingRatio(speed * pitchFactor);

    currentFileName = audioFile.getFileName();
    return true;
}

void KaraokePlayer::unloadFile()
{
    std::lock_guard<std::mutex> lock(loadMutex);
    transportSource.stop();
    if (resampler != nullptr)
        resampler->releaseResources();
    transportSource.releaseResources();
    resampler.reset();
    transportSource.setSource(nullptr);
    currentReaderSource.reset();
    currentFileName.clear();
}

void KaraokePlayer::start()
{
    transportSource.start();
}

void KaraokePlayer::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void KaraokePlayer::pause()
{
    transportSource.stop();
}

bool KaraokePlayer::isPlaying() const noexcept
{
    return transportSource.isPlaying();
}

void KaraokePlayer::setPosition(double newPositionSeconds)
{
    transportSource.setPosition(newPositionSeconds);
}

double KaraokePlayer::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double KaraokePlayer::getLengthInSeconds() const
{
    return transportSource.getLengthInSeconds();
}

void KaraokePlayer::setVolume(float newVolume) noexcept
{
    backingVolume.store(juce::jlimit(0.0f, 2.0f, newVolume), std::memory_order_relaxed);
}

void KaraokePlayer::setPitchSemitones(int semitones) noexcept
{
    pitchTranspose.store(juce::jlimit(-6, 6, semitones), std::memory_order_relaxed);
    if (resampler != nullptr)
    {
        float speed = tempoRatio.load(std::memory_order_relaxed);
        float pitchFactor = std::pow(2.0f, static_cast<float>(pitchTranspose.load(std::memory_order_relaxed)) / 12.0f);
        resampler->setResamplingRatio(speed * pitchFactor);
    }
}

void KaraokePlayer::setSpeedRatio(float ratio) noexcept
{
    tempoRatio.store(juce::jlimit(0.7f, 1.3f, ratio), std::memory_order_relaxed);
    if (resampler != nullptr)
    {
        float speed = tempoRatio.load(std::memory_order_relaxed);
        float pitchFactor = std::pow(2.0f, static_cast<float>(pitchTranspose.load(std::memory_order_relaxed)) / 12.0f);
        resampler->setResamplingRatio(speed * pitchFactor);
    }
}

juce::String KaraokePlayer::getLoadedFileName() const
{
    return currentFileName;
}

void KaraokePlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (bufferToFill.buffer == nullptr || bufferToFill.numSamples <= 0)
        return;

    if (!transportSource.isPlaying() || resampler == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    resampler->getNextAudioBlock(bufferToFill);

    // Apply backing track volume
    float vol = backingVolume.load(std::memory_order_relaxed);
    if (std::abs(vol - 1.0f) > 0.001f)
    {
        bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, vol);
    }
}

} // namespace VocalStudio
