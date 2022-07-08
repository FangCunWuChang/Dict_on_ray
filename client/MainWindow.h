#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"

class MainWindow final : public juce::DocumentWindow
{
public:
    MainWindow(juce::String name);
    ~MainWindow();

    void closeButtonPressed() override;

    void resized() override;

private:
    juce::OpenGLContext openGL;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
