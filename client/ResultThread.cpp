#include "ResultThread.h"
#include "Config.h"
#include "Trans.h"

std::unique_ptr<ResultThread> ResultThread::instance = std::make_unique<ResultThread>();

ResultThread::ResultThread()
	:Thread("Result Thread")
{
	this->callBack = [](const juce::String&, const juce::String&, bool) {};
}

ResultThread::~ResultThread()
{
	if (this->isThreadRunning())
	{
		this->stopThread(11000);
		this->waitForThreadToExit(-1);
	}
}

void ResultThread::destory()
{
	ResultThread::instance = nullptr;
}

bool ResultThread::running()
{
	return ResultThread::instance->isThreadRunning();
}

void ResultThread::startGet(const juce::String& word)
{
	ResultThread::instance->word = word;
	ResultThread::instance->startThread();
}

void ResultThread::setCallBack(CallBack callBack)
{
	ResultThread::instance->callBack = callBack;
}

void ResultThread::waitGet()
{
	ResultThread::instance->waitForThreadToExit(-1);
}

void ResultThread::run()
{
	juce::URL getUrl(
		"https://" + Config::getUrl() + ":" + juce::String(Config::getPort()) +
		"/?" + Config::getQuery() + "=" + this->word);

	int statusCode = 0;
	auto stream = getUrl.createInputStream(
		juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
		.withConnectionTimeoutMs(10000)
		.withStatusCode(&statusCode)
	);

	if (statusCode == 200 && !this->threadShouldExit()) {
		juce::var res = juce::JSON::parse(stream->readEntireStreamAsString());
		if (res["status"]) {
			this->callBack(res["result"].toString(), res["word"].toString(), true);
		}
		else {
			this->callBack(Trans::tr("TE_Null"), res["word"].toString(), true);
		}
	}
	else {
		this->callBack(Trans::tr("TE_NNF") + juce::String(statusCode), this->word, false);
	}
}
