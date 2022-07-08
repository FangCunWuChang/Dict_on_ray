#pragma once
#include <JuceHeader.h>
#include "../Trans.h"

class TranslationFactory
{
public:
	TranslationFactory() = default;
	virtual ~TranslationFactory() = default;

	virtual const juce::String getName() = 0;
	virtual const std::map<juce::String, juce::String> getList() = 0;
};