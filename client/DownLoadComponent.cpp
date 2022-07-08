#include "DownLoadComponent.h"
#include "Device.h"
#include "ListThread.h"
#include "Trans.h"
#include "JInterface.h"

DownLoadComponent::DownLoadComponent()
{
	this->setName("Download Component");
	this->setFramesPerSecond(25);
	this->setOpaque(false);

	auto showFunc = [this] {
		this->setVisible(true);
	};

	jmadf::JInterfaceDao<void>::set("showDownload", showFunc);

	jmadf::JInterfaceDao<void>::call("showDownload");
}

void DownLoadComponent::update()
{
	if (this->isVisible() != ListThread::running()) {
		this->setVisible(ListThread::running());
		//this->setVisible(true);
	}
	const juce::Rectangle<int> screenRect = Device::getScreenSize(this);
	int barWidth = screenRect.getWidth() * 0.025;

	double cWidth = 0.3, cHeight = 0.3;
	double barHeight = 0.05;

	if (this->barCount >= barWidth * 2) {
		this->barCount = 0;
	}
	else {
		this->barCount += 2;
	}

	if (this->isVisible()) {
		this->repaint(
			(this->getWidth() - cWidth * screenRect.getWidth()) / 2,
			(this->getHeight() + cHeight * screenRect.getHeight()) / 2 - barHeight * screenRect.getHeight(),
			cWidth * screenRect.getWidth(), barHeight * screenRect.getHeight()
		);
	}
}

void DownLoadComponent::paint(juce::Graphics& g)
{
	const juce::Rectangle<int> screenRect = Device::getScreenSize(this);
	const double screenScale = Device::getScreenScale(this);

	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::widgetBackground)
		.withAlpha(0.7f)
	);
	g.fillRect(0, 0, this->getWidth(), this->getHeight());

	double cWidth = 0.3, cHeight = 0.3;
	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::widgetBackground)
	);
	g.fillRect(
		(int)((this->getWidth() - cWidth * screenRect.getWidth()) / 2), (int)((this->getHeight() - cHeight * screenRect.getHeight()) / 2),
		cWidth * screenRect.getWidth(), cHeight * screenRect.getHeight()
	);

	double barHeight = 0.05;
	juce::Image barImage(juce::Image::PixelFormat::ARGB,
		cWidth * screenRect.getWidth(), barHeight * screenRect.getHeight(), true);
	juce::Graphics bar(barImage);

	int barWidth = screenRect.getWidth() * 0.025;
	bar.addTransform(juce::AffineTransform::fromTargetPoints(
		0, 0, barWidth, 0,
		cWidth * screenRect.getWidth(), 0, cWidth * screenRect.getWidth() + barWidth, 0,
		0, barHeight * screenRect.getHeight(), 0, barHeight * screenRect.getHeight()
	));

	bar.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::highlightedFill)
	);
	for (int i = -1; i < cWidth * screenRect.getWidth() / barWidth; i++) {
		bar.fillRect(
			i * barWidth * 2 + this->barCount, 0,
			barWidth, barHeight * screenRect.getHeight()
		);
	}

	g.drawImageAt(barImage,
		(this->getWidth() - cWidth * screenRect.getWidth()) / 2,
		(this->getHeight() + cHeight * screenRect.getHeight()) / 2 - barHeight * screenRect.getHeight(),
		false
	);

	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::defaultText)
	);
	g.drawFittedText(
		Trans::tr("TE_Loa"),
		juce::Rectangle<int>((int)((this->getWidth() - cWidth * screenRect.getWidth()) / 2), (int)((this->getHeight() - cHeight * screenRect.getHeight()) / 2),
			cWidth * screenRect.getWidth(), cHeight * screenRect.getHeight() - barHeight * screenRect.getHeight()),
		juce::Justification::centred, 1
	);
}
