#include "MainWindow.h"
#include "Device.h"

MainWindow::MainWindow(juce::String name)
    : DocumentWindow(name,
        juce::Desktop::getInstance().getDefaultLookAndFeel()
        .findColour(juce::ResizableWindow::backgroundColourId),
        DocumentWindow::allButtons)
{
    float fontSize = 0.005 * Device::getScreenSize(this).getHeight() * Device::getScreenScale(this);
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(
        juce::Font(L"\u9ED1\u4F53", fontSize, juce::Font::FontStyleFlags::plain).getTypefacePtr());

    this->setUsingNativeTitleBar(true);
    auto* mainPtr = new MainComponent();
    this->setContentOwned(mainPtr, true);

#if JUCE_IOS || JUCE_ANDROID
    this->setFullScreen(true);
#else
    juce::Rectangle<int> screenSize = Device::getScreenSize(this);
    this->setResizable(true, true);
    this->centreWithSize(screenSize.getWidth() / 2, screenSize.getHeight() / 2);
#endif
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainWindow::resized()
{
    juce::Rectangle<int> screenSize = Device::getScreenSize(this);
    this->setResizeLimits(
        screenSize.getWidth() * 0.5, screenSize.getHeight() * 0.5,
        screenSize.getWidth() * 2, screenSize.getHeight() * 2
    );
    this->DocumentWindow::resized();
}
