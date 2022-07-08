#include "WordList.h"
#include "WordListModel.h"

std::unique_ptr<WordList> WordList::instance = std::make_unique<WordList>();

bool WordList::available()
{
	juce::ScopedReadLock locker(WordList::instance->lock);
	return !WordList::instance->data.isEmpty();
}

void WordList::set(const juce::String& data)
{
	juce::ScopedWriteLock locker(WordList::instance->lock);
	WordList::instance->data = data;
}

juce::StringArray WordList::find(const juce::String& word)
{
	juce::ScopedReadLock locker(WordList::instance->lock);
	juce::StringArray result;
	if (word.isEmpty()) {
		result = juce::StringArray::fromLines(WordList::instance->data);
	}
	else {
		result = juce::StringArray::fromLines(WordList::instance->data);
	}
	result.removeEmptyStrings();
	return result;
}

juce::String* WordList::get()
{
	return &(WordList::instance->data);
}

void WordList::load()
{
	juce::File file(WordList::instance->path);
	WordList::set(file.loadFileAsString());
}

void WordList::save()
{
	juce::ScopedReadLock locker(WordList::instance->lock);
	juce::File file(WordList::instance->path);
	file.deleteFile();
	file.appendText(
		WordList::instance->data,
		false, false,
		"\n"
	);
}

void WordList::init(const juce::String& path)
{
	WordList::instance->path = path;
	WordList::load();
}

void WordList::destory()
{
	WordList::instance = nullptr;
}
