#pragma once
#include <JuceHeader.h>

class ResultThread final : public juce::Thread
{
	static std::unique_ptr<ResultThread> instance;

public:
	using CallBack = std::function<void(const juce::String&, const juce::String&, bool)>;

	static void destory();

	static bool running();

	static void startGet(const juce::String& word);
	static void setCallBack(CallBack);
	static void waitGet();

public:
	ResultThread();
	~ResultThread()override;

	void run()override;

private:
	CallBack callBack;
	juce::String word;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResultThread)
};
