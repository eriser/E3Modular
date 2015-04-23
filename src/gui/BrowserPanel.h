
#pragma once

#include "JuceHeader.h"


namespace e3
{
    class TabComponent;
    class TableComponent;

    class BrowserPanel : public Component
    {
    public:
        BrowserPanel();

        void resized() override;

        void updateContents(XmlElement* root);
        XmlElement* getSelectedInstrumentXml();

    protected:
        enum TabIds { 
            kBankTab,
            kInstrumentTab
        };
        ScopedPointer<TabComponent> tabPanel_;
        ScopedPointer<TableComponent> instrumentBrowser_;
        ScopedPointer<Component> presetBrowser_;

        ScopedPointer<TextButton> openButton_;
        ScopedPointer<TextButton> saveButton_;
        ScopedPointer<TextButton> saveAsButton_;
        ScopedPointer<TextButton> newButton_;
    };


} // namespace e3