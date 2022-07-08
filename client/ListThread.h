#pragma once
#include <JuceHeader.h>

class ListThread final :public juce::Thread
{
	static std::unique_ptr<ListThread> instance;

public:
	using CallBack = std::function<void(void)>;

	static void destory();

	static bool running();

	static void startGet();
	static void startGetAsync(CallBack);
	static bool result();
	static void waitGet();

public:
	ListThread();
	~ListThread()override;

	void run()override;

private:
	bool _result = false;
	CallBack callBack;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListThread)
};