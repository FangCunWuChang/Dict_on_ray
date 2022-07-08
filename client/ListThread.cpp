#include "ListThread.h"
#include "Config.h"
#include "WordList.h"

std::unique_ptr<ListThread> ListThread::instance = std::make_unique<ListThread>();

ListThread::ListThread()
	:Thread("List Thread")
{

}

ListThread::~ListThread()
{
	if (this->isThreadRunning())
	{
		this->stopThread(10000);
		this->waitForThreadToExit(-1);
	}
}

void ListThread::destory()
{
	ListThread::instance = nullptr;
}

bool ListThread::running()
{
	return ListThread::instance->isThreadRunning();
}

void ListThread::startGet()
{
	ListThread::instance->callBack = [] {};
	ListThread::instance->startThread();
}

void ListThread::startGetAsync(CallBack callBack)
{
	ListThread::instance->callBack = callBack;
	ListThread::instance->startThread();
}

bool ListThread::result()
{
	return ListThread::instance->_result;
}

void ListThread::waitGet()
{
	ListThread::instance->waitForThreadToExit(-1);
}

void ListThread::run()
{
	this->_result = false;
	juce::URL getUrl(
		"https://" + Config::getUrl() + ":" + juce::String(Config::getPort()) +
		"/" + Config::getListQuery());

	int statusCode = 0;
	auto stream = getUrl.createInputStream(
		juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
		.withConnectionTimeoutMs(10000)
		.withStatusCode(&statusCode)
	);

	if (statusCode == 200 && !this->threadShouldExit()) {
		WordList::set(stream->readEntireStreamAsString());
		WordList::save();
		this->_result = true;
	}

	if (!this->threadShouldExit()) {
		this->callBack();
	}
}
