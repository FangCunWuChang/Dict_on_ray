#include "WordListModel.h"
#include "Device.h"
#include "JInterface.h"

std::unique_ptr<WordListModel> WordListModel::instance = std::make_unique<WordListModel>();

void WordListModel::destory()
{
	WordListModel::instance = nullptr;
}

WordListModel* WordListModel::getInstance()
{
	return WordListModel::instance.get();
}

void WordListModel::setList(const juce::StringArray& list)
{
	juce::ScopedWriteLock locker(WordListModel::instance->lock);
	WordListModel::instance->dataArray = list;
}

int WordListModel::find(const juce::String& str)
{
	juce::ScopedReadLock locker(WordListModel::instance->lock);
	return WordListModel::instance->dataArray.indexOf(str);
}

juce::String WordListModel::item(int index)
{
	juce::ScopedReadLock locker(WordListModel::instance->lock);
	if (index < 0 || index >= WordListModel::instance->dataArray.size())return juce::String();
	return WordListModel::instance->dataArray.getReference(index);
}

juce::StringArray* WordListModel::get()
{
	return &(WordListModel::instance->dataArray);
}

int WordListModel::getNumRows()
{
	juce::ScopedReadLock locker(this->lock);
	return this->dataArray.size();
}

void WordListModel::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
	juce::ScopedReadLock locker(this->lock);
	if (rowNumber < 0 || rowNumber >= this->dataArray.size())return;
	juce::String& str = this->dataArray.getReference(rowNumber);
	if (rowIsSelected) {
		g.setColour(
			((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
			.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::highlightedFill)
		);
		g.fillRect(juce::Rectangle<int>(0, 0, width, height));
		g.setColour(
			((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
			.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::highlightedText)
		);
		g.drawFittedText(
			str, juce::Rectangle<int>(0, 0, width, height),
			juce::Justification::centred,
			1
		);
	}
	else {
		g.setColour(
			((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
			.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::defaultText)
		);
		g.drawFittedText(
			str, juce::Rectangle<int>(0, 0, width, height),
			juce::Justification::centred,
			1
		);
	}
}

void WordListModel::selectedRowsChanged(int lastRowSelected)
{
	juce::ScopedReadLock locker(this->lock);
	if (lastRowSelected < 0 || lastRowSelected >= this->dataArray.size())return;
	juce::String& str = this->dataArray.getReference(lastRowSelected);
	jmadf::JInterfaceDao<const juce::String&>::call("getResult", str);
}
