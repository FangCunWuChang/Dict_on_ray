#pragma once
#include <JuceHeader.h>

class Device final
{
public:
	Device() = delete;
	~Device() = delete;

public:
	static const juce::Rectangle<int> getScreenSize(juce::Component* component);
	static const double getScreenScale(juce::Component* component);
	static const juce::Displays::Display* getCurrentScreen(juce::Component* component);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Device)
};