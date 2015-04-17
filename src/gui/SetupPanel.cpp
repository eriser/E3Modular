
#ifdef BUILD_TARGET_APP
#include "app/AudioEngine.h"
#include "app/Application.h"
#include "core/Processor.h"
#endif

#include "gui/Style.h"
#include "gui/TabComponent.h"
#include "gui/PreferencesPanel.h"
#include "gui/SetupPanel.h"


namespace e3
{
    SetupPanel::SetupPanel() : Component()
    {
        createComponents();
    }


    void SetupPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        tabPanel_->setBounds(content);
    }


#ifdef BUILD_TARGET_APP
    void SetupPanel::createComponents()
    {
        AudioEngine* engine         = Application::getApp()->getAudioEngine();
        AudioDeviceManager* manager = engine->getDeviceManager();
        Processor* processor        = engine->getProcessor();

        audioPanel_ = new AudioDeviceSelectorComponent(
            *manager,
            processor->getNumInputChannels(),
            processor->getNumInputChannels(),
            processor->getNumOutputChannels(),
            processor->getNumOutputChannels(),
            true, false,
            true, false);

        preferencesPanel_ = new PreferencesPanel();

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtLeft);
        tabPanel_->addTab("Preferences", Colours::transparentBlack, preferencesPanel_, false, kPreferencesTab);
        tabPanel_->addTab("Audio", Colours::transparentBlack, audioPanel_, false, kAudioTab);

        addAndMakeVisible(tabPanel_);
    }
#endif

#ifdef BUILD_TARGET_VST
    void SetupPanel::createComponents()
    {
        preferencesPanel_ = new PreferencesPanel();

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtLeft);
        tabPanel_->addTab("Preferences", Colours::transparentBlack, preferencesPanel_, false, kPreferencesTab);

        addAndMakeVisible(tabPanel_);
    }
#endif

} // namespace e3