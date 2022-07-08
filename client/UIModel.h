#pragma once
#include <JuceHeader.h>
#include "LAF.h"

class UIModel final
{
	static std::unique_ptr<UIModel> _uiModel;

public:
	static void destory();
	static void setFlag();
	static void clearFlag();
	static bool haveFlag();

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

	static void setSearch(const juce::String& value);
	static void setResult(const juce::String& value);

	static void setList(const juce::String& value);

	static juce::String getSearch();
	static juce::String getResult();

	static juce::String getList();

public:
	UIModel() = default;

private:
	bool flag = false;

private:
	juce::String commonUrl = "localhost";
	uint16_t commonPort = 443;
	juce::String commonListQuery = "word-list";
	juce::String commonQuery = "word";
	juce::String commonLanguage = "zh-cn";

	LAFInitType commonLAFType = LAFInitType::Time;

	juce::String search;
	juce::String result;
	juce::String list;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UIModel)
};