#include "Trans.h"
#include "Config.h"
#include "translations/Translations.h"

std::unique_ptr<Trans> Trans::_instance = std::make_unique<Trans>();

void Trans::init()
{
	Trans::add(new ZH_CN);
	Trans::add(new EN_US);

	Trans::refresh();
}

void Trans::refresh()
{
	juce::ScopedWriteLock locker(Trans::_instance->_lock);
	juce::String tran = Config::getLanguage();

	for (auto i : Trans::_instance->_list) {
		if (i->getName() == tran) {
			Trans::_instance->_trans = i->getList();
			break;
		}
	}
}

const juce::String Trans::tr(const juce::String& text)
{
	juce::ScopedReadLock locker(Trans::_instance->_lock);
	auto tit = Trans::_instance->_trans.find(text);
	if(tit != Trans::_instance->_trans.end()) {
        return (*tit).second;
	}
	return text;
}

const juce::StringArray Trans::getList()
{
	juce::StringArray result;
	for (auto i : Trans::_instance->_list) {
		result.add(i->getName());
	}
	return result;
}

void Trans::destory()
{
	Trans::_instance = nullptr;
}

void Trans::add(TranslationFactory* factory)
{
	Trans::_instance->_list.add(factory);
}
