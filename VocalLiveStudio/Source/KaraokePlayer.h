#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>
#include <memory>
#include <mutex>

namespace VocalStudio
{

class KaraokePlayer
{
public:
    KaraokePlayer();
    ~KaraokePlayer();

    void prepareToPlay(double sampleRate, int samplesPerBlockExpected);
    void releaseResources();

    // Fill buffer with current backing track audio
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    // File loading
    bool loadFile(const juce::File& audioFile);
    void unloadFile();

    // Transport controls
    void start();
    void stop();
    void pause();
    bool isPlaying() const noexcept;

    void setPosition(double newPositionSeconds);
    double getPosition() const;
    double getLengthInSeconds() const;

    // Track controls
    void setVolume(float newVolume) noexcept; // 0.0 to 1.5 (1.0 = 0dB)
    float getVolume() const noexcept { return backingVolume.load(std::memory_order_relaxed); }

    // Pitch transpose (-6 to +6 semitones)
    void setPitchSemitones(int semitones) noexcept;
    int getPitchSemitones() const noexcept { return pitchTranspose.load(std::memory_order_relaxed); }

    // Playback speed / tempo (0.8x to 1.2x)
    void setSpeedRatio(float ratio) noexcept;
    float getSpeedRatio() const noexcept { return tempoRatio.load(std::memory_order_relaxed); }

    juce::String getLoadedFileName() const;

private:
    double currentSampleRate { 48000.0 };
    int currentBlockSize { 512 };

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> currentReaderSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resampler;

    std::mutex loadMutex;
    juce::String currentFileName;

    std::atomic<float> backingVolume { 0.8f };
    std::atomic<int> pitchTranspose { 0 };
    std::atomic<float> tempoRatio { 1.0f };
};

} // namespace VocalStudio
