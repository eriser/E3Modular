
#pragma once

#include "JuceHeader.h"

namespace e3
{
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
        ScopedPointer<TabComponent> tabPanel_;
        ScopedPointer<TableComponent> bankBrowser_;
        ScopedPointer<Component> instrumentBrowser_;
    };


} // namespace e3