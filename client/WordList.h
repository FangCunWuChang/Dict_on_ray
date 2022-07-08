#pragma once
#include <JuceHeader.h>

class WordList final
{
	static std::unique_ptr<WordList> instance;

public:
	static bool available();
	static void set(const juce::String& data);
	static void load();
	static void save();
	static juce::StringArray find(const juce::String& word);
	static juce::String* get();

	static void init(const juce::String& path);
	static void destory();
public:
	WordList() = default;

private:
	juce::String path;
	juce::String data;
	juce::ReadWriteLock lock;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WordList)
};