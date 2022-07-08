#include "ConfigComponent.h"
#include "Trans.h"
#include "Device.h"
#include "SVGData.h"
#include "UIModel.h"
#include "Config.h"
#include "JInterface.h"
#include "ListThread.h"
#include "WordList.h"
#include "SearchThread.h"

ConfigComponent::ConfigComponent()
	:Component("Config Component")
{
	this->groupBox = std::make_unique<juce::GroupComponent>("Config Group", Trans::tr("LB_Config"));
	this->addAndMakeVisible(this->groupBox.get());
	
	this->lbUrl = std::make_unique<juce::Label>("Url Label", Trans::tr("LB_Url"));
	this->lbUrl->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbUrl.get());
	this->lbPort = std::make_unique<juce::Label>("Port Label", Trans::tr("LB_Port"));
	this->lbPort->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbPort.get());
	this->lbQuery = std::make_unique<juce::Label>("Query Label", Trans::tr("LB_Query"));
	this->lbQuery->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbQuery.get());
	this->lbList = std::make_unique<juce::Label>("List Label", Trans::tr("LB_List"));
	this->lbList->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbList.get());
	this->lbLAF = std::make_unique<juce::Label>("Look And Feel Label", Trans::tr("LB_LAF"));
	this->lbLAF->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbLAF.get());
	this->lbLan = std::make_unique<juce::Label>("Language Label", Trans::tr("LB_Lan"));
	this->lbLan->setJustificationType(juce::Justification::centredRight);
	this->addAndMakeVisible(this->lbLan.get());

	this->teUrl = std::make_unique<juce::TextEditor>("Url Text Editor");
	this->teUrl->setJustification(juce::Justification::centredLeft);
	this->addAndMakeVisible(this->teUrl.get());
	this->tePort = std::make_unique<juce::TextEditor>("Port Text Editor");
	this->tePort->setJustification(juce::Justification::centredLeft);
	this->addAndMakeVisible(this->tePort.get());
	this->teQuery = std::make_unique<juce::TextEditor>("Query Text Editor");
	this->teQuery->setJustification(juce::Justification::centredLeft);
	this->addAndMakeVisible(this->teQuery.get());
	this->teList = std::make_unique<juce::TextEditor>("List Text Editor");
	this->teList->setJustification(juce::Justification::centredLeft);
	this->addAndMakeVisible(this->teList.get());

	this->tePort->setInputFilter(
		new juce::TextEditor::LengthAndCharacterRestriction(5, "0123456789"), true);

	this->btTim = std::make_unique<juce::TextButton>(Trans::tr("BT_Tim"), Trans::tr("TP_Tim"));
	this->btTim->setRadioGroupId(10001);
	this->btTim->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
	this->addAndMakeVisible(this->btTim.get());
	this->btSys = std::make_unique<juce::TextButton>(Trans::tr("BT_Sys"), Trans::tr("TP_Sys"));
	this->btSys->setRadioGroupId(10001);
	this->btSys->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft | juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
	this->addAndMakeVisible(this->btSys.get());
	this->btDar = std::make_unique<juce::TextButton>(Trans::tr("BT_Dar"), Trans::tr("TP_Dar"));
	this->btDar->setRadioGroupId(10001);
	this->btDar->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft | juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
	this->addAndMakeVisible(this->btDar.get());
	this->btLig = std::make_unique<juce::TextButton>(Trans::tr("BT_Lig"), Trans::tr("TP_Lig"));
	this->btLig->setRadioGroupId(10001);
	this->btLig->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);
	this->addAndMakeVisible(this->btLig.get());

	this->btTim->setClickingTogglesState(true);
	this->btSys->setClickingTogglesState(true);
	this->btDar->setClickingTogglesState(true);
	this->btLig->setClickingTogglesState(true);

	this->btTim->setToggleState(true, juce::NotificationType::dontSendNotification);

	this->cbLan = std::make_unique<juce::ComboBox>("Language Combo Box");
	this->cbLan->setJustificationType(juce::Justification::centredLeft);
	this->addAndMakeVisible(this->cbLan.get());

	juce::Colour dbColor = juce::Desktop::getInstance().getDefaultLookAndFeel()
		.findColour(juce::DrawableButton::ColourIds::textColourId);
	this->dbAcc = std::make_unique<juce::DrawableButton>(
		"Accept Drawable Button",
		juce::DrawableButton::ButtonStyle::ImageOnButtonBackground
		);
	this->dbCan = std::make_unique<juce::DrawableButton>(
		"Cancel Drawable Button",
		juce::DrawableButton::ButtonStyle::ImageOnButtonBackground
		);

	juce::DrawablePath pathAcc, pathCan;
	juce::Path pAcc = juce::Drawable::parseSVGPath(juce::String(SVGData::check_line));
	juce::Path pCan = juce::Drawable::parseSVGPath(juce::String(SVGData::close_line));

	pathAcc.setPath(pAcc);
	pathCan.setPath(pCan);
	pathAcc.setFill(dbColor);
	pathCan.setFill(dbColor);

	this->dbAcc->setImages(&pathAcc);
	this->dbCan->setImages(&pathCan);

	this->addAndMakeVisible(this->dbAcc.get());
	this->addAndMakeVisible(this->dbCan.get());

	this->teUrl->setText(Config::getUrl());
	this->tePort->setText(juce::String(Config::getPort()));
	this->teQuery->setText(Config::getQuery());
	this->teList->setText(Config::getListQuery());
	switch (Config::getLAFType())
	{
	case LAFInitType::Time:
	{
		this->btTim->setToggleState(true, juce::NotificationType::dontSendNotification);
		break;
	}
	case LAFInitType::System:
	{
		this->btSys->setToggleState(true, juce::NotificationType::dontSendNotification);
		break;
	}
	case LAFInitType::AlwaysDark:
	{
		this->btDar->setToggleState(true, juce::NotificationType::dontSendNotification);
		break;
	}
	case LAFInitType::AlwaysLight:
	{
		this->btLig->setToggleState(true, juce::NotificationType::dontSendNotification);
		break;
	}
	}
	
	auto tList = Trans::getList();
	this->cbLan->addItemList(tList, 1);
	int tiIndex = tList.indexOf(Config::getLanguage());
	this->cbLan->setSelectedItemIndex(std::max(tiIndex, 0),
		juce::NotificationType::dontSendNotification);

	this->lisAcc = std::make_unique<AccListener>(this);
	this->lisCan = std::make_unique<CanListener>(this);
	this->dbAcc->addListener(this->lisAcc.get());
	this->dbCan->addListener(this->lisCan.get());
}

ConfigComponent::~ConfigComponent()
{
	if (this->isVisible()) {
		this->push();
	}
	else {
		UIModel::clearFlag();
	}
}

void ConfigComponent::show()
{
	this->dbCan->setEnabled(WordList::available());
	if (UIModel::haveFlag()) {
		this->teUrl->setText(UIModel::getUrl());
		this->tePort->setText(juce::String(UIModel::getPort()));
		this->teQuery->setText(UIModel::getQuery());
		this->teList->setText(UIModel::getListQuery());
		switch (UIModel::getLAFType())
		{
		case LAFInitType::Time:
		{
			this->btTim->setToggleState(true, juce::NotificationType::dontSendNotification);
			break;
		}
		case LAFInitType::System:
		{
			this->btSys->setToggleState(true, juce::NotificationType::dontSendNotification);
			break;
		}
		case LAFInitType::AlwaysDark:
		{
			this->btDar->setToggleState(true, juce::NotificationType::dontSendNotification);
			break;
		}
		case LAFInitType::AlwaysLight:
		{
			this->btLig->setToggleState(true, juce::NotificationType::dontSendNotification);
			break;
		}
		}

		auto tList = Trans::getList();
		this->cbLan->addItemList(tList, 1);
		int tiIndex = tList.indexOf(UIModel::getLanguage());
		this->cbLan->setSelectedItemIndex(std::max(tiIndex, 0),
			juce::NotificationType::dontSendNotification);
	}
}

void ConfigComponent::push()
{
	UIModel::setFlag();
	UIModel::setUrl(this->teUrl->getText());
	UIModel::setPort(this->tePort->getText().getIntValue());
	UIModel::setQuery(this->teQuery->getText());
	UIModel::setListQuery(this->teList->getText());
	if (this->btTim->getToggleState()) {
		UIModel::setLAFType(LAFInitType::Time);
	}
	else if (this->btSys->getToggleState()) {
		UIModel::setLAFType(LAFInitType::System);
	}
	else if (this->btDar->getToggleState()) {
		UIModel::setLAFType(LAFInitType::AlwaysDark);
	}
	else if (this->btLig->getToggleState()) {
		UIModel::setLAFType(LAFInitType::AlwaysLight);
	}
	UIModel::setLanguage(this->cbLan->getText());
}

void ConfigComponent::resized()
{
	const juce::Rectangle<int> screenRect = Device::getScreenSize(this);
	const double screenScale = Device::getScreenScale(this);

	double xBorder = 0.004, yBorder = 0.01;
	double bottomBorder = 0.1;

	double xMargin = 0.01, yMargin = 0.01;
	
	int groupHeight = std::min(
		(int)(this->getHeight() - bottomBorder * screenRect.getHeight()), screenRect.getHeight() / 2);
	this->groupBox->setBounds(
		xBorder * screenRect.getWidth(), yBorder * screenRect.getHeight(),
		this->getWidth() - 2 * (xBorder * screenRect.getWidth()), groupHeight);

	auto groupRect = this->groupBox->getBoundsInParent();
	juce::Rectangle<int> settingArea(
		groupRect.getX() + xMargin * screenRect.getWidth(), groupRect.getY() + yMargin * screenRect.getHeight(),
		groupRect.getWidth() - 2 * xMargin * screenRect.getWidth(), groupRect.getHeight() - 2 * yMargin * screenRect.getHeight()
	);

	int lbHeight = 0.03 * screenRect.getHeight() * screenScale;
	int splitHeight = (settingArea.getHeight() - 5 * lbHeight) / 4;

	double labelWidth = std::min(0.1, screenRect.getWidth() * 0.055 / settingArea.getWidth());

	double lbX1 = labelWidth;
	this->lbUrl->setBounds(
		settingArea.getX(), settingArea.getY() + splitHeight / 2,
		settingArea.getWidth() * lbX1, lbHeight
	);

	double lbX3 = 0.7, lbX5 = lbX3 + labelWidth;
	this->lbPort->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX3, settingArea.getY() + splitHeight / 2,
		settingArea.getWidth() * (lbX5 - lbX3), lbHeight
	);

	this->lbQuery->setBounds(
		settingArea.getX(), settingArea.getY() + splitHeight / 2 + splitHeight * 1 + lbHeight * 1,
		settingArea.getWidth() * lbX1, lbHeight
	);
	
	double lbX2 = 0.5, lbX4 = lbX2 + labelWidth;
	this->lbList->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX2, settingArea.getY() + splitHeight / 2 + splitHeight * 1 + lbHeight * 1,
		settingArea.getWidth() * (lbX4 - lbX2), lbHeight
	);

	this->lbLAF->setBounds(
		settingArea.getX(), settingArea.getY() + splitHeight / 2 + splitHeight * 2 + lbHeight * 2,
		settingArea.getWidth() * lbX1, lbHeight
	);

	this->lbLan->setBounds(
		settingArea.getX(), settingArea.getY() + splitHeight / 2 + splitHeight * 3 + lbHeight * 4,
		settingArea.getWidth() * lbX1, lbHeight
	);

	this->teUrl->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX1, settingArea.getY() + splitHeight / 2,
		settingArea.getWidth() * (lbX3 - lbX1), lbHeight
	);
	this->tePort->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX5, settingArea.getY() + splitHeight / 2,
		settingArea.getWidth() * (1 - lbX5), lbHeight
	);

	this->teQuery->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX1, settingArea.getY() + splitHeight / 2 + splitHeight * 1 + lbHeight * 1,
		settingArea.getWidth() * (lbX2 - lbX1), lbHeight
	);
	this->teList->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX4, settingArea.getY() + splitHeight / 2 + splitHeight * 1 + lbHeight * 1,
		settingArea.getWidth() * (1 - lbX4), lbHeight
	);

	int buttonWidth = settingArea.getWidth() / 4;
	this->btTim->setBounds(
		settingArea.getX() + buttonWidth * 0, settingArea.getY() + splitHeight / 2 + splitHeight * 2 + lbHeight * 3,
		buttonWidth, lbHeight
	);
	this->btSys->setBounds(
		settingArea.getX() + buttonWidth * 1, settingArea.getY() + splitHeight / 2 + splitHeight * 2 + lbHeight * 3,
		buttonWidth, lbHeight
	);
	this->btDar->setBounds(
		settingArea.getX() + buttonWidth * 2, settingArea.getY() + splitHeight / 2 + splitHeight * 2 + lbHeight * 3,
		buttonWidth, lbHeight
	);
	this->btLig->setBounds(
		settingArea.getX() + buttonWidth * 3, settingArea.getY() + splitHeight / 2 + splitHeight * 2 + lbHeight * 3,
		buttonWidth, lbHeight
	);

	double cbX1 = 0.5;
	this->cbLan->setBounds(
		settingArea.getX() + settingArea.getWidth() * lbX1, settingArea.getY() + splitHeight / 2 + splitHeight * 3 + lbHeight * 4,
		settingArea.getWidth() * (cbX1 - lbX1), lbHeight
	);

	double sbXMargin = 0.17, sbYMargin = 0.02;
	double sbSplitWidth = 0.02;

	int sbWidth = (this->getWidth()
		- 2 * sbXMargin * screenRect.getWidth()
		- sbSplitWidth * screenRect.getWidth()) / 2;
	sbWidth = std::min(sbWidth, (int)(screenRect.getWidth() * 0.1));
	int sbHeight = bottomBorder * screenRect.getHeight() - 2 * sbYMargin * screenRect.getHeight();

	int sbY = groupRect.getBottom() + sbYMargin * screenRect.getHeight();
	this->dbAcc->setBounds(
		this->getWidth() / 2.0 - sbSplitWidth * screenRect.getWidth() / 2 - sbWidth,
		sbY,
		sbWidth, sbHeight
	);
	this->dbCan->setBounds(
		this->getWidth() / 2.0 + sbSplitWidth * screenRect.getWidth() / 2,
		sbY,
		sbWidth, sbHeight
	);
}

void ConfigComponent::paint(juce::Graphics& g)
{
	g.setColour(
		((juce::LookAndFeel_V4&)(juce::Desktop::getInstance().getDefaultLookAndFeel()))
		.getCurrentColourScheme().getUIColour(juce::LookAndFeel_V4::ColourScheme::UIColour::widgetBackground)
	);
	g.fillAll();
}

void ConfigComponent::AccListener::buttonClicked(juce::Button*)
{
	bool needRefresh = false;
	bool lafChanged = false, lanChanged = false;

	bool urlChanged = false, portChanged = false;
	if (this->parent->teUrl->getText() != Config::getUrl()) {
		urlChanged = true;
	}
	if (this->parent->tePort->getText().getIntValue() != Config::getPort()) {
		portChanged = true;
	}
	Config::setUrl(this->parent->teUrl->getText());
	Config::setPort(this->parent->tePort->getText().getIntValue());
	Config::setQuery(this->parent->teQuery->getText());
	Config::setListQuery(this->parent->teList->getText());

	LAFInitType type = LAFInitType::Time;
	if (this->parent->btTim->getToggleState()) {
		type = LAFInitType::Time;
	}
	else if (this->parent->btSys->getToggleState()) {
		type = LAFInitType::System;
	}
	else if (this->parent->btDar->getToggleState()) {
		type = LAFInitType::AlwaysDark;
	}
	else if (this->parent->btLig->getToggleState()) {
		type = LAFInitType::AlwaysLight;
	}
	if (type != Config::getLAFType()) {
		lafChanged = true;
	}
	Config::setLAFType(type);

	if (this->parent->cbLan->getText() != Config::getLanguage()) {
		lanChanged = true;
	}
	Config::setLanguage(this->parent->cbLan->getText());
	if (lanChanged) {
		Trans::refresh();
	}

	if (
		urlChanged || portChanged ||
		!WordList::available()
		) {
		if (SearchThread::running()) {
			SearchThread::startGet(juce::String());
		}
		ListThread::startGetAsync(
			[this, lafChanged, lanChanged] {
				if (!ListThread::result()) {
					juce::AlertWindow::showMessageBox(
						juce::MessageBoxIconType::WarningIcon,
						Trans::tr("WT_Err"), Trans::tr("TE_Con"),
						Trans::tr("BT_OK")
					);
					return;
				}
				Config::save();

				juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
				UIModel::clearFlag();
				this->parent->setVisible(false);

				jmadf::JInterfaceDao<void>::call("resetSearch");

				bool needRefresh = lafChanged || lanChanged;
				if (needRefresh) {
					jmadf::JInterfaceDao<void>::call("refreshUI");
				}
			}
		);
		jmadf::JInterfaceDao<void>::call("showDownload");
	}

	Config::save();

	UIModel::clearFlag();
	this->parent->setVisible(false);

	needRefresh = lafChanged || lanChanged;
	if (needRefresh) {
		jmadf::JInterfaceDao<void>::call("refreshUI");
	}
}

void ConfigComponent::CanListener::buttonClicked(juce::Button*)
{
	UIModel::clearFlag();
	this->parent->setVisible(false);
}
