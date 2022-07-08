#include "MainComponent.h"
#include "WordList.h"
#include "UIModel.h"
#include "Device.h"
#include "SVGData.h"
#include "LAF.h"
#include "ListThread.h"
#include "JInterface.h"
#include "Trans.h"
#include "WordListModel.h"
#include "ResultThread.h"
#include "SearchThread.h"

MainComponent::MainComponent()
    : Component("Main Window Central Component")
{
    this->_config = std::make_unique<ConfigComponent>();
    this->addChildComponent(this->_config.get());

    this->_download = std::make_unique<DownLoadComponent>();
    this->addChildComponent(this->_download.get());

    this->_search = std::make_unique<SearchComponent>();
    this->addChildComponent(this->_search.get());

    if (UIModel::haveFlag() || !WordList::available()) {
        this->_config->show();
        this->_config->setVisible(true);
    }

    this->dbConfig = std::make_unique<juce::DrawableButton>("Config Drawable Button",
        juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
    this->dbRefresh = std::make_unique<juce::DrawableButton>("Refresh Drawable Button",
        juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
    this->dbUI = std::make_unique<juce::DrawableButton>("Color Mode Drawable Button",
        juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);

    this->dbConfig->setConnectedEdges(
        juce::Button::ConnectedEdgeFlags::ConnectedOnLeft |
        juce::Button::ConnectedEdgeFlags::ConnectedOnRight
    );
    this->dbRefresh->setConnectedEdges(
        juce::Button::ConnectedEdgeFlags::ConnectedOnLeft |
        juce::Button::ConnectedEdgeFlags::ConnectedOnRight
    );
    this->dbUI->setConnectedEdges(
        juce::Button::ConnectedEdgeFlags::ConnectedOnLeft |
        juce::Button::ConnectedEdgeFlags::ConnectedOnRight
    );

    juce::Colour dbColor = juce::Desktop::getInstance().getDefaultLookAndFeel()
        .findColour(juce::DrawableButton::ColourIds::textColourId);

    juce::DrawablePath pathCon, pathRef, pathUI;
    juce::Path pCon = juce::Drawable::parseSVGPath(juce::String(SVGData::settings_line));
    juce::Path pRef = juce::Drawable::parseSVGPath(juce::String(SVGData::refresh_line));
    juce::Path pUI = juce::Drawable::parseSVGPath(juce::String(
        LAFManager::get() == LAFDay ? SVGData::moon_fill : SVGData::sun_line));

    pathCon.setPath(pCon);
    pathRef.setPath(pRef);
    pathUI.setPath(pUI);
    pathCon.setFill(dbColor);
    pathRef.setFill(dbColor);
    pathUI.setFill(dbColor);

    this->dbConfig->setImages(&pathCon);
    this->dbRefresh->setImages(&pathRef);
    this->dbUI->setImages(&pathUI);

    this->addAndMakeVisible(this->dbConfig.get());
    this->addAndMakeVisible(this->dbRefresh.get());
    this->addAndMakeVisible(this->dbUI.get());

    this->lisCon = std::make_unique<ConListener>(this);
    this->lisRef = std::make_unique<RefListener>(this);
    this->lisUI = std::make_unique<UIListener>(this);

    this->dbConfig->addListener(this->lisCon.get());
    this->dbRefresh->addListener(this->lisRef.get());
    this->dbUI->addListener(this->lisUI.get());

    this->list = std::make_unique<juce::ListBox>("Word List Box", WordListModel::getInstance());
    this->addAndMakeVisible(this->list.get());

    this->teSearch = std::make_unique<juce::TextEditor>("Search Text Editor");
    this->teSearch->setJustification(juce::Justification::centredLeft);
    this->teSearch->setTextToShowWhenEmpty(
        Trans::tr("TE_Empty"),
        juce::Desktop::getInstance().getDefaultLookAndFeel()
        .findColour(juce::TextEditor::ColourIds::focusedOutlineColourId)
    );
    this->addAndMakeVisible(this->teSearch.get());

    this->teResult = std::make_unique<juce::TextEditor>("Result Text Editor");
    this->teResult->setJustification(juce::Justification::topLeft);
    this->teResult->setMultiLine(true);
    this->teResult->setReadOnly(true);
    this->addAndMakeVisible(this->teResult.get());

    jmadf::JInterfaceDao<void>::set(
        "resetSearch", [this] {
            this->teSearch->clear();
            jmadf::JInterfaceDao<const juce::String&>::call("checkout", "");
            this->list->selectRow(-1);
            this->list->updateContent();
            this->list->scrollToEnsureRowIsOnscreen(0);
            this->teResult->clear();
        }
    );
    this->setSize(600, 400);

    this->teSearch->setText(UIModel::getSearch());
    this->teResult->setText(UIModel::getResult());
    int index = WordListModel::find(UIModel::getList());
    if (index > -1) {
        this->list->selectRow(index);
    }

    ResultThread::setCallBack(
        [this](const juce::String& result, const juce::String& word, bool /*status*/) {
            juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
            this->teResult->setText(word + "\n" + result);
            this->list->setEnabled(true);
            this->teSearch->setEnabled(true);
        }
    );
    jmadf::JInterfaceDao<const juce::String&>::set(
        "getResult", [this](const juce::String& word) {
            if (!ResultThread::running()) {
                this->teSearch->setEnabled(false);
                this->list->setEnabled(false);
                ResultThread::startGet(word);
            }
        }
    );

    SearchThread::setCallBack(
        [this] {
            juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
            this->list->selectRow(-1);
            this->list->updateContent();
            this->list->scrollToEnsureRowIsOnscreen(0);
            this->teResult->clear();
            jmadf::JInterfaceDao<void>::call("hideSearch");
        }
    );

    this->teSearch->onReturnKey = [this] {
        jmadf::JInterfaceDao<void>::call("showSearch");
        SearchThread::startGet(this->teSearch->getText());
    };
    this->teSearch->onTextChange = [this] {
        if (this->teSearch->getText().isEmpty()) {
            jmadf::JInterfaceDao<void>::call("showSearch");
            SearchThread::startGet(juce::String());
        }
    };

    if (SearchThread::running()) {
        jmadf::JInterfaceDao<void>::call("showSearch");
    }
}

MainComponent::~MainComponent()
{
    jmadf::JInterfaceDao<const juce::String&>::set(
        "getResult", [](const juce::String&) {}
    );
    ResultThread::setCallBack(
        [](const juce::String& /*result*/, const juce::String& /*word*/, bool /*status*/) {}
    );
    SearchThread::setCallBack([] {});
    UIModel::setSearch(this->teSearch->getText());
    UIModel::setResult(this->teResult->getText());
    UIModel::setList(WordListModel::item(this->list->getLastRowSelected()));
    this->list->setModel(nullptr);
}

void MainComponent::paint(juce::Graphics& /*g*/)
{
}

void MainComponent::resized()
{
    this->_config->setBounds(0, 0, this->getWidth(), this->getHeight());
    this->_download->setBounds(0, 0, this->getWidth(), this->getHeight());

    const juce::Rectangle<int> screenRect = Device::getScreenSize(this);
    const double screenScale = Device::getScreenScale(this);

    double listWidth = 0.1, searchHeight = 0.04;
    int lWidth = listWidth * screenRect.getWidth() * screenScale;
    int sHeight = searchHeight * screenRect.getHeight() * screenScale;

    this->dbConfig->setBounds(
        0, 0, lWidth / 3, sHeight
    );
    this->dbRefresh->setBounds(
        lWidth / 3 * 1, 0, lWidth / 3, sHeight
    );
    this->dbUI->setBounds(
        lWidth / 3 * 2, 0, lWidth / 3, sHeight
    );

    this->list->setBounds(
        0, sHeight,
        lWidth, this->getHeight() - sHeight
    );

    this->teSearch->setBounds(
        lWidth, 0,
        this->getWidth() - lWidth, sHeight
    );

    this->teResult->setBounds(
        lWidth, sHeight,
        this->getWidth() - lWidth, this->getHeight() - sHeight
    );

    float fontSize = 0.025 * Device::getScreenSize(this).getHeight() * Device::getScreenScale(this);
    juce::Font resFont = this->teResult->getFont();
    resFont.setHeight(fontSize);
    this->teResult->applyFontToAllText(resFont, true);

    double slWidth = 0.1, slHeight = 0.05;
    int islWidth = slWidth * screenRect.getWidth() * screenScale;
    int islHeight = slHeight * screenRect.getHeight() * screenScale;
    this->_search->setBounds(
        this->getWidth() - islWidth, this->getHeight() - islHeight,
        islWidth, islHeight
    );

    this->_search->toFront(false);
    this->_config->toFront(false);
    this->_download->toFront(false);
}

void MainComponent::ConListener::buttonClicked(juce::Button*)
{
    this->parent->_config->show();
    this->parent->_config->setVisible(true);
}

void MainComponent::RefListener::buttonClicked(juce::Button*)
{
    ListThread::startGetAsync(
        [this] {
            if (!ListThread::result()) {
                juce::AlertWindow::showMessageBox(
                    juce::MessageBoxIconType::WarningIcon,
                    Trans::tr("WT_Err"), Trans::tr("TE_Con"),
                    Trans::tr("BT_OK")
                );
                juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
                this->parent->_config->show();
                this->parent->_config->setVisible(true);
            }
            else {
                juce::MessageManagerLock locker(juce::Thread::getCurrentThread());
                jmadf::JInterfaceDao<void>::call("resetSearch");
            }
        }
    );
    jmadf::JInterfaceDao<void>::call("showDownload");
}

void MainComponent::UIListener::buttonClicked(juce::Button*)
{
    jmadf::JInterfaceDao<LAFColorType>::call("setLookAndFeel",
        LAFManager::set(LAFManager::get() == LAFNight ? LAFDay : LAFNight));
}
