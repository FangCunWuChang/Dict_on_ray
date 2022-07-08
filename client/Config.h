#pragma once
#include <JuceHeader.h>
#include "LAF.h"

class Config final
{
public:
	static std::unique_ptr<Config> _instance;

public:
	static void init(const juce::String& configFile);
	static void destory();

public:
	Config() = default;
	~Config() = default;

public:
	static juce::String getUrl();
	static uint16_t getPort();
	static juce::String getListQuery();
	static juce::String getQuery();
	static juce::String getLanguage();

	static LAFInitType getLAFType();

	static void setUrl(const juce::String& value);
	static void setPort(const uint16_t value);
	static void setListQuery(const juce::String& value);
	static void setQuery(const juce::String& value);
	static void setLanguage(const juce::String& value);

	static void setLAFType(const LAFInitType value);

	static void save();
	static void save(const juce::String& configFile);

private:
	const juce::String commonUrl = "localhost";
	const uint16_t commonPort = 443;
	const juce::String commonListQuery = "word-list";
	const juce::String commonQuery = "word";
	const juce::String commonLanguage = "zh-cn";

	const LAFInitType commonLAFType = LAFInitType::Time;

private:
	juce::var _config;
	juce::ReadWriteLock _lock;

	juce::String _fileName;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};
