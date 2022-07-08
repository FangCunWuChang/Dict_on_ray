#pragma once
#include <JuceHeader.h>
#include "ConfigComponent.h"
#include "DownLoadComponent.h"
#include "SearchComponent.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

public:
	std::unique_ptr<juce::TextEditor> teSearch, teResult;
	std::unique_ptr<juce::ListBox> list;

private:
    std::unique_ptr<ConfigComponent> _config;
    std::unique_ptr<DownLoadComponent> _download;
	std::unique_ptr<SearchComponent> _search;

    std::unique_ptr<juce::DrawableButton> dbConfig, dbRefresh, dbUI;

	class ConListener final :public juce::Button::Listener
	{
	public:
		ConListener(MainComponent* parent) :parent(parent) {};
		void buttonClicked(juce::Button*)override;
	private:
		MainComponent* parent = nullptr;
	};
	class RefListener final :public juce::Button::Listener
	{
	public:
		RefListener(MainComponent* parent) :parent(parent) {};
		void buttonClicked(juce::Button*)override;
	private:
		MainComponent* parent = nullptr;
	};
	class UIListener final :public juce::Button::Listener
	{
	public:
		UIListener(MainComponent* parent) :parent(parent) {};
		void buttonClicked(juce::Button*)override;
	private:
		MainComponent* parent = nullptr;
	};

	friend class ConListemer;
	friend class RefListener;
	friend class UIListener;

	std::unique_ptr<ConListener> lisCon;
	std::unique_ptr<RefListener> lisRef;
	std::unique_ptr<UIListener> lisUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};