#include "MainWindow.h"

MainWindow::MainWindow(juce::String name)
    : DocumentWindow(name,
        juce::Desktop::getInstance().getDefaultLookAndFeel()
        .findColour(juce::ResizableWindow::backgroundColourId),
        DocumentWindow::allButtons)
{
    this->setUsingNativeTitleBar(true);
    this->setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
    this->setFullScreen(true);
#else
    this->setResizable(true, true);
    this->centreWithSize(this->getWidth(), this->getHeight());
#endif
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
