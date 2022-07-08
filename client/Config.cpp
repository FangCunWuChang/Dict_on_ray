#include "Config.h"

std::unique_ptr<Config> Config::_instance = std::make_unique<Config>();

void Config::init(const juce::String& configFile)
{
	Config::_instance->_fileName = configFile;
	juce::File file(configFile);

	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config = juce::JSON::parse(file);
	if (!Config::_instance->_config.isObject()) {
		Config::_instance->_config = new juce::DynamicObject;
	}
}

void Config::destory()
{
	Config::_instance = nullptr;
}

juce::String Config::getUrl()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["url"];
	if (result.isString()) {
		return result.toString();
	}
	return Config::_instance->commonUrl;
}

uint16_t Config::getPort()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["port"];
	if (result.isInt()) {
		return (int)result;
	}
	return Config::_instance->commonPort;
}

juce::String Config::getListQuery()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["list-query"];
	if (result.isString()) {
		return result.toString();
	}
	return Config::_instance->commonListQuery;
}

juce::String Config::getQuery()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["query"];
	if (result.isString()) {
		return result.toString();
	}
	return Config::_instance->commonQuery;
}

juce::String Config::getLanguage()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["language"];
	if (result.isString()) {
		return result.toString();
	}
	return Config::_instance->commonLanguage;
}

LAFInitType Config::getLAFType()
{
	juce::ScopedReadLock locker(Config::_instance->_lock);
	juce::var result = Config::_instance->_config["laf-type"];
	if (result.isInt()) {
		return (LAFInitType)(int)result;
	}
	return Config::_instance->commonLAFType;
}

void Config::setUrl(const juce::String& value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("url", value);
}

void Config::setPort(const uint16_t value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("port", value);
}

void Config::setListQuery(const juce::String& value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("list-query", value);
}

void Config::setQuery(const juce::String& value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("query", value);
}

void Config::setLanguage(const juce::String& value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("language", value);
}

void Config::setLAFType(const LAFInitType value)
{
	juce::ScopedWriteLock locker(Config::_instance->_lock);
	Config::_instance->_config.getDynamicObject()->setProperty("laf-type", (int)value);
}

void Config::save()
{
	Config::save(Config::_instance->_fileName);
}

void Config::save(const juce::String& configFile)
{
	juce::File file(configFile);
	file.deleteFile();
	juce::FileOutputStream stream(file);
	
	if (stream.openedOk()) {
		juce::JSON::writeToStream(
			stream, Config::_instance->_config
		);
		stream.flush();
	}
}
