#pragma once
#include <JuceHeader.h>

class WordListModel final : public juce::ListBoxModel
{
	static std::unique_ptr<WordListModel> instance;

public:
	static void destory();
	static WordListModel* getInstance();
	static void setList(const juce::StringArray& list);
	static int find(const juce::String& str);
	static juce::String item(int index);
	static juce::StringArray* get();

public:
	WordListModel() = default;

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
	void selectedRowsChanged(int lastRowSelected) override;
private:
	juce::StringArray dataArray;
	juce::ReadWriteLock lock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WordListModel)
};