#pragma once
#include <JuceHeader.h>

class ConfigComponent final : public juce::Component
{
public:
	ConfigComponent();
	~ConfigComponent();

	void resized() override;
	void paint(juce::Graphics&) override;

	void show();
	void push();

private:
	std::unique_ptr<juce::GroupComponent> groupBox;
	std::unique_ptr<juce::Label> lbUrl, lbPort, lbQuery, lbList, lbLAF, lbLan;
	std::unique_ptr<juce::TextEditor> teUrl, tePort, teQuery, teList;
	std::unique_ptr<juce::TextButton> btTim, btSys, btDar, btLig;
	std::unique_ptr<juce::ComboBox> cbLan;
	std::unique_ptr<juce::DrawableButton> dbAcc, dbCan;

private:
	class AccListener final :public juce::Button::Listener
	{
	public:
		AccListener(ConfigComponent* parent) :parent(parent) {};
		void buttonClicked(juce::Button*)override;
	private:
		ConfigComponent* parent = nullptr;
	};
	class CanListener final :public juce::Button::Listener
	{
	public:
		CanListener(ConfigComponent* parent) :parent(parent) {};
		void buttonClicked(juce::Button*)override;
	private:
		ConfigComponent* parent = nullptr;
	};

	friend class AccListemer;
	friend class CanListener;

	std::unique_ptr<AccListener> lisAcc;
	std::unique_ptr<CanListener> lisCan;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigComponent)
};