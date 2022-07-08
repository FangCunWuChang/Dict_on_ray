#include <JuceHeader.h>
#include "MainWindow.h"
#include "JInterface.h"
#include "LAF.h"
#include "Config.h"
#include "Trans.h"
#include "UIModel.h"
#include "ListThread.h"
#include "WordList.h"
#include "WordListModel.h"
#include "ResultThread.h"
#include "SearchThread.h"

class DictClientApplication final : public juce::JUCEApplication
{
public:
    DictClientApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& /*commandLine*/) override
    {
        Config::init(
            juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
            .getParentDirectory().getFullPathName() + "/config.json"
        );
        WordList::init(
            juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile)
            .getParentDirectory().getFullPathName() + "/list.txt"
        );
        Trans::init();

        jmadf::JInterfaceDao<const juce::String&>::set(
            "checkout", [](const juce::String& word) {
                static juce::SpinLock lock;
                juce::GenericScopedLock<juce::SpinLock> locker(lock);

                WordListModel::setList(WordList::find(word));
                SearchThread::setList(*(WordListModel::get()));
            }
        );
        jmadf::JInterfaceDao<const juce::String&>::call("checkout", "");

        this->lookAndFeels.add(new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getDarkColourScheme()));
        this->lookAndFeels.add(new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getMidnightColourScheme()));
        this->lookAndFeels.add(new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getDarkColourScheme()));
        this->lookAndFeels.add(new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getLightColourScheme()));

        auto newWindowFunc = [this] {
            bool over = (bool)this->mainWindow;
            juce::Rectangle<int> rect;
            bool isFull = false;
            if (over) {
                isFull = this->mainWindow->isFullScreen();
                this->mainWindow->setFullScreen(false);
                rect = this->mainWindow->getBounds();
            }
            this->mainWindow = nullptr;
            this->mainWindow = std::make_unique<MainWindow>("Dict-on-ray Client");
            if (over) {
                this->mainWindow->setBounds(rect);
                this->mainWindow->setFullScreen(isFull);
            }
            this->mainWindow->setVisible(true);
        };

        jmadf::JInterfaceDao<LAFColorType>::set("setLookAndFeel",
            [this, newWindowFunc](LAFColorType type) {
                juce::LookAndFeel* laf = this->lookAndFeels[(int)type];
                if (laf) {
                    juce::Desktop::getInstance()
                        .setDefaultLookAndFeel(laf);
                    newWindowFunc();
                }
            }
        );

        this->darkListener = std::make_unique<DarkListener>();
        juce::Desktop::getInstance()
            .addDarkModeSettingListener(this->darkListener.get());

        this->timer = std::make_unique<UITimer>();

        auto refreshFunc = [this] {
            bool UILight = false;
            juce::Time time = juce::Time::getCurrentTime();

            switch (Config::getLAFType())
            {
            case LAFInitType::Time:
            {
                if (time.getHours() >= AppControl::DayTime && time.getHours() < AppControl::NightTime) {
                    UILight = true;
                }
                break;
            }
            case LAFInitType::System:
            {
                if (!juce::Desktop::getInstance().isDarkModeActive()) {
                    UILight = true;
                }
                break;
            }
            case LAFInitType::AlwaysDark:
            {
                break;
            }
            case LAFInitType::AlwaysLight:
            {
                UILight = true;
                break;
            }
            }

            if (time.getHours() >= AppControl::DayTime && time.getHours() < AppControl::NightTime) {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth(),
                    AppControl::NightTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->timer->startTimer((int)(bTimes - cTimes));
            }
            else if (time.getHours() < AppControl::DayTime) {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth(),
                    AppControl::DayTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->timer->startTimer((int)(bTimes - cTimes));
            }
            else {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth() + 1,
                    AppControl::DayTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->timer->startTimer((int)(bTimes - cTimes));
            }

            jmadf::JInterfaceDao<LAFColorType>::call("setLookAndFeel", LAFManager::set(UILight ? LAFDay : LAFNight));
        };

        jmadf::JInterfaceDao<void>::set("refreshUI", refreshFunc);
        jmadf::JInterfaceDao<void>::call("refreshUI");
    }

    void shutdown() override
    {
        this->mainWindow = nullptr;
        SearchThread::destory();
        ResultThread::destory();
        ListThread::destory();
        WordListModel::destory();
        jmadf::JInterface::destoryInstance();
        UIModel::destory();
        Trans::destory();
        WordList::destory();
        Config::destory();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

public:
    class DarkListener final : public juce::DarkModeSettingListener
    {
    public:
        DarkListener() = default;

        void darkModeSettingChanged() override
        {
            if (Config::getLAFType() == LAFInitType::System) {
                jmadf::JInterfaceDao<LAFColorType>::call(
                    "setLookAndFeel",
                    LAFManager::set(!juce::Desktop::getInstance().isDarkModeActive() ? LAFDay : LAFNight)
                );
            }
        };

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarkListener)
    };

    class UITimer final : public juce::Timer
    {
    public:
        void timerCallback() override
        {
            juce::Time time = juce::Time::getCurrentTime();
            if (Config::getLAFType() == LAFInitType::Time) {
                bool UILight = false;
                if (time.getHours() >= AppControl::DayTime && time.getHours() < AppControl::NightTime) {
                    UILight = true;
                }
                jmadf::JInterfaceDao<LAFColorType>::call(
                    "setLookAndFeel",
                    LAFManager::set(UILight ? LAFDay : LAFNight)
                );
            }
            if (time.getHours() >= AppControl::DayTime && time.getHours() < AppControl::NightTime) {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth(),
                    AppControl::NightTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->startTimer((int)(bTimes - cTimes));
            }
            else if (time.getHours() < AppControl::DayTime) {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth(),
                    AppControl::DayTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->startTimer((int)(bTimes - cTimes));
            }
            else {
                uint64_t bTimes = juce::Time(
                    time.getYear(),
                    time.getMonth(),
                    time.getDayOfMonth() + 1,
                    AppControl::DayTime,
                    0, 0, 0,
                    true
                ).toMilliseconds();
                uint64_t cTimes = time.currentTimeMillis();
                this->startTimer((int)(bTimes - cTimes));
            }
        };
    };

    std::unique_ptr<MainWindow> mainWindow;
    juce::OwnedArray<juce::LookAndFeel> lookAndFeels;

    std::unique_ptr<DarkListener> darkListener;
    std::unique_ptr<UITimer> timer;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DictClientApplication)
};

START_JUCE_APPLICATION(DictClientApplication)
