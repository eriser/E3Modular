
#pragma once

#include "JuceHeader.h"

namespace e3
{
    class TabComponent;
    class PrefenerencesPanel;

    class SetupPanel : public Component
    {
    public:
        SetupPanel();

        void resized() override;

    protected:
        void createComponents();

        enum TabIds {
            kAudioTab,
            kPreferencesTab
        };
        ScopedPointer<TabComponent> tabPanel_;
        ScopedPointer<PreferencesPanel> preferencesPanel_;
#ifdef BUILD_TARGET_APP
        ScopedPointer<AudioDeviceSelectorComponent>  audioPanel_;
#endif
    };


} // namespace e3