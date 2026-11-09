#include "MainComponent.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace VocalStudio
{

class VocalLiveStudioApplication : public juce::JUCEApplication
{
public:
    VocalLiveStudioApplication() = default;

    const juce::String getApplicationName() override       { return "Vocal Live Studio"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise(const juce::String& /*commandLineParameters*/) override
    {
        audioEngine = std::make_unique<AudioEngine>();
        audioEngine->initialize(1, 2); // 1 in, 2 out

        mainWindow = std::make_unique<MainWindow>(getApplicationName(), *audioEngine);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        if (audioEngine != nullptr)
        {
            audioEngine->shutdown();
            audioEngine = nullptr;
        }
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& /*commandLine*/) override
    {
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name, AudioEngine& engine)
            : DocumentWindow(name,
                             juce::Colour(0xff12141e),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(engine), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            setResizeLimits(1024, 700, 1920, 1200);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<MainWindow> mainWindow;
};

} // namespace VocalStudio

START_JUCE_APPLICATION(VocalStudio::VocalLiveStudioApplication)
