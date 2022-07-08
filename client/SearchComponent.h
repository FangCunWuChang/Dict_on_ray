#pragma once
#include <JuceHeader.h>

class SearchComponent final : public juce::AnimatedAppComponent
{
public:
	SearchComponent();
	void update()override;
	void paint(juce::Graphics&)override;

	bool closeFlag = false;
	int closeCount = 0;

private:
	int loopCount = 0;
	juce::DrawablePath pathLoad;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchComponent)
};