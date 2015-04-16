
#pragma once

#include "core/GlobalHeader.h"
#include "JuceHeader.h"

namespace e3
{
    //class StackedPanel;
    //class TabButtonGroup;
    class TabComponent;
    class TableComponent;

    class BrowserPanel : public Component, public Button::Listener
    {
    public:
        BrowserPanel();

        void resized() override;

    protected:
        void buttonClicked(Button*) override {}

        enum TabIds {
            kBankTab,
            kInstrumentTab
        };
        ScopedPointer<TabbedComponent> tabPanel_;
        //ScopedPointer<TabButtonGroup> tabButtons_;
        ScopedPointer<TableComponent> bankBrowser_;
        ScopedPointer<Component> instrumentBrowser_;
    };


} // namespace e3