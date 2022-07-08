#include "Device.h"

const juce::Rectangle<int> Device::getScreenSize(juce::Component* component)
{
	const juce::Displays::Display* display = Device::getCurrentScreen(component);
	if (display == nullptr) {
		return juce::Rectangle<int>();
	}
	return display->userArea;
}

const double Device::getScreenScale(juce::Component* component)
{
	const juce::Displays::Display* display = Device::getCurrentScreen(component);
	if (display == nullptr) {
		return 1;
	}
	return display->scale;
}

const juce::Displays::Display* Device::getCurrentScreen(juce::Component* component)
{
	return juce::Desktop::getInstance().getDisplays().getDisplayForRect(component->getScreenBounds());
}
