#pragma once
#include <JuceHeader.h>

class TranslationFactory;

class Trans final
{
private:
	static std::unique_ptr<Trans> _instance;
	static void add(TranslationFactory* factory);

public:
	Trans() = default;
	~Trans() = default;

public:
	static void init();
	static void refresh();
	static void destory();

	static const juce::String tr(const juce::String& text);
	static const juce::StringArray getList();

private:
	juce::OwnedArray<TranslationFactory> _list;
	std::map<juce::String, juce::String> _trans;
	juce::ReadWriteLock _lock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trans)
};