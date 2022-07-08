#pragma once
#include <JuceHeader.h>

class DownLoadComponent final : public juce::AnimatedAppComponent
{
public:
	DownLoadComponent();

	void update()override;

	void paint(juce::Graphics&)override;

private:
	int barCount = 0;
};