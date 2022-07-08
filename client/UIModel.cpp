#include "UIModel.h"

std::unique_ptr<UIModel> UIModel::_uiModel = std::make_unique<UIModel>();

void UIModel::destory()
{
	UIModel::_uiModel = nullptr;
}

void UIModel::setFlag()
{
	UIModel::_uiModel->flag = true;
}

void UIModel::clearFlag()
{
	UIModel::_uiModel->flag = false;
}

bool UIModel::haveFlag()
{
	return UIModel::_uiModel->flag;
}

juce::String UIModel::getUrl()
{
	return UIModel::_uiModel->commonUrl;
}

uint16_t UIModel::getPort()
{
	return UIModel::_uiModel->commonPort;
}

juce::String UIModel::getListQuery()
{
	return UIModel::_uiModel->commonListQuery;
}

juce::String UIModel::getQuery()
{
	return UIModel::_uiModel->commonQuery;
}

juce::String UIModel::getLanguage()
{
	return UIModel::_uiModel->commonLanguage;
}

LAFInitType UIModel::getLAFType()
{
	return UIModel::_uiModel->commonLAFType;
}

void UIModel::setUrl(const juce::String& value)
{
	UIModel::_uiModel->commonUrl = value;
}

void UIModel::setPort(const uint16_t value)
{
	UIModel::_uiModel->commonPort = value;
}

void UIModel::setListQuery(const juce::String& value)
{
	UIModel::_uiModel->commonListQuery = value;
}

void UIModel::setQuery(const juce::String& value)
{
	UIModel::_uiModel->commonQuery = value;
}

void UIModel::setLanguage(const juce::String& value)
{
	UIModel::_uiModel->commonLanguage = value;
}

void UIModel::setLAFType(const LAFInitType value)
{
	UIModel::_uiModel->commonLAFType = value;
}

void UIModel::setSearch(const juce::String& value)
{
	UIModel::_uiModel->search = value;
}

void UIModel::setResult(const juce::String& value)
{
	UIModel::_uiModel->result = value;
}

void UIModel::setList(const juce::String& value)
{
	UIModel::_uiModel->list = value;
}

juce::String UIModel::getSearch()
{
	return UIModel::_uiModel->search;
}

juce::String UIModel::getResult()
{
	return UIModel::_uiModel->result;
}

juce::String UIModel::getList()
{
	return UIModel::_uiModel->list;
}
