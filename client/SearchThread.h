#pragma once
#include <JuceHeader.h>

class SearchThread final : public juce::Thread
{
	static std::unique_ptr<SearchThread> instance;

public:
	using CallBack = std::function<void(void)>;

	static void destory();

	static bool running();

	static void startGet(const juce::String& word);
	static void setCallBack(CallBack);
	static void waitGet();

	static void setList(const juce::StringArray& list);

public:
	SearchThread();
	~SearchThread()override;

	void run()override;

private:
	static int KMP(const juce::String& strShort, const juce::String& strLong, std::vector<int>& next);
	static void preKMP(const juce::String& strShort, std::vector<int>& next);

private:
	CallBack callBack;
	juce::String word;
	juce::StringArray list;
	juce::ReadWriteLock lock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchThread)
};
