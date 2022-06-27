#include <JuceHeader.h>
#include "MainWindow.h"

class DictClientApplication final : public juce::JUCEApplication
{
public:
    DictClientApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& /*commandLine*/) override
    {
        this->mainWindow = std::make_unique<MainWindow>("MainWindow");
        this->mainWindow->setVisible(true);
    }

    void shutdown() override
    {
        this->mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& /*commandLine*/) override
    {
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(DictClientApplication)
