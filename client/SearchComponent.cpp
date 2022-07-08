#include "SearchComponent.h"
#include "JInterface.h"
#include "SVGData.h"
#include "Trans.h"

SearchComponent::SearchComponent()
{
	this->setName("Search Component");
	this->setFramesPerSecond(25);
	this->setOpaque(false);

	auto showFunc = [this] {
		this->closeCount = 0;
		this->closeFlag = false;
		this->setVisible(true);
	};

	jmadf::JInterfaceDao<void>::set("showSearch", showFunc);

	auto hideFunc = [this] {
		this->closeCount = 0;
		this->closeFlag = true;
	};

	jmadf::JInterfaceDao<void>::set("hideSearch", hideFunc);

	juce::Colour pColor = ((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::defaultText);

	juce::Path pLoad = juce::Drawable::parseSVGPath(juce::String(SVGData::loader_4_line));

	this->pathLoad.setPath(pLoad);
	this->pathLoad.setFill(pColor);
}

void SearchComponent::update()
{
	float tn = (50 - this->closeCount) / 50.f;
	this->setAlpha(tn);

	if (this->loopCount < 2000 - 1) {
		this->loopCount++;
	}
	else {
		this->loopCount = 0;
	}
	if (this->closeFlag) {
		if (this->closeCount < 50 - 1) {
			this->closeCount++;
		}
		else {
			this->closeCount = 0;
			this->closeFlag = false;
			this->setVisible(false);
		}
		this->repaint();
	}
	else {
		this->repaint(
			this->getWidth() - this->getHeight(), 0,
			this->getHeight(), this->getHeight()
		);
	}
}

void SearchComponent::paint(juce::Graphics& g)
{
	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::windowBackground)
	);
	g.fillRect(0, 0, this->getWidth(), this->getHeight());


	juce::Image loadImage(juce::Image::PixelFormat::ARGB,
		this->getHeight(), this->getHeight(), true);
	juce::Graphics load(loadImage);
	float angle = this->loopCount / 2000.f * 360.f;
	load.addTransform(juce::AffineTransform::rotation(
		angle, this->getHeight() / 2.f, this->getHeight() / 2.f
	));

	double per = 0.6;
	this->pathLoad.drawWithin(load,
		juce::Rectangle<float>(
			this->getHeight() * (1 - per) / 2, this->getHeight() * (1 - per) / 2,
			this->getHeight() * per, this->getHeight() * per
			), juce::RectanglePlacement::centred | juce::RectanglePlacement::stretchToFit,
		100);

	g.drawImageAt(loadImage,
		this->getWidth() - this->getHeight(), 0,
		false
	);

	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::defaultText)
	);
	g.drawFittedText(
		this->closeFlag ? Trans::tr("TE_Sec") : Trans::tr("TE_Ser"),
		juce::Rectangle<int>(0, 0,
			this->getWidth() - this->getHeight(), this->getHeight()),
		juce::Justification::centred, 1
	);
}