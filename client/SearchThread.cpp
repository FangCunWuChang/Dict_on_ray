#include "SearchThread.h"
#include "WordListModel.h"

std::unique_ptr<SearchThread> SearchThread::instance = std::make_unique<SearchThread>();

SearchThread::SearchThread()
	:Thread("Search Thread")
{
	this->callBack = [] {};
}

SearchThread::~SearchThread()
{
	if (this->isThreadRunning())
	{
		this->stopThread(10000);
		this->waitForThreadToExit(-1);
	}
}

void SearchThread::destory()
{
	SearchThread::instance = nullptr;
}

bool SearchThread::running()
{
	return SearchThread::instance->isThreadRunning();
}

void SearchThread::startGet(const juce::String& word)
{
	if (SearchThread::instance->isThreadRunning())
	{
		SearchThread::instance->stopThread(10000);
		SearchThread::instance->waitForThreadToExit(-1);
	}
	SearchThread::instance->word = word;
	SearchThread::instance->startThread();
}

void SearchThread::setCallBack(CallBack callBack)
{
	SearchThread::instance->callBack = callBack;
}

void SearchThread::waitGet()
{
	SearchThread::instance->waitForThreadToExit(-1);
}

void SearchThread::setList(const juce::StringArray& list)
{
	juce::ScopedWriteLock locker(SearchThread::instance->lock);
	SearchThread::instance->list = list;
}

void SearchThread::run()
{
	juce::ScopedReadLock locker(this->lock);
	juce::StringArray result;
	std::vector<int> next;

	if (this->word.isEmpty()) {
		WordListModel::setList(this->list);
		this->callBack();
		return;
	}

	for (auto& s : this->list) {
		if (this->threadShouldExit()) {
			return;
		}

		SearchThread::preKMP(s, next);
		int index = SearchThread::KMP(this->word, s, next);
		if (index > -1) {
			if (index == 0) {
				result.insert(0, s);
			}
			else {
				result.add(s);
			}
		}
	}

	WordListModel::setList(result);
	this->callBack();
}

void SearchThread::preKMP(const juce::String& strLong, std::vector<int>& next)
{
	next.resize(strLong.length());

	next[0] = -1;           //while the first char not match, i++,j++
	int j = 0;
	int k = -1;
	while (j < strLong.length() - 1)
	{
		if (k == -1 || strLong[j] == strLong[k])
		{
			j++;
			k++;
			next[j] = k;
		}
		else
		{
			k = next[k];
		}
	}
}

int SearchThread::KMP(const juce::String& strShort, const juce::String& strLong, std::vector<int>& next)
{
	int i = 0;
	int j = 0;
	while (i < strLong.length() && j < (int)strShort.length())
	{
		if (j == -1 || strLong[i] == strShort[j])
		{
			i++;
			j++;
		}
		else
		{
			j = next[j];
		}
	}
	if (j == (int)strShort.length())
	{
		return i - j;
	}
	return -1;
}
