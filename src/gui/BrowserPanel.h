
#pragma once

#include "JuceHeader.h"


namespace e3
{
    class TabComponent;
    class InstrumentBrowser;

    class BrowserPanel : public Component
    {
    public:
        void resized() override;

        void BrowserPanel::setComponents( InstrumentBrowser* instrumentBrowser, Component* presetBrowser );
        void updateContents( XmlElement* root );
        XmlElement* getSelectedInstrumentXml();

    protected:
        ScopedPointer<TabComponent> tabPanel_;
        InstrumentBrowser* instrumentBrowser_ = nullptr;
        Component* presetBrowser_ = nullptr;

        TextButton openButton_;
        TextButton saveButton_;
        TextButton saveAsButton_;
        TextButton newButton_;
    };


} // namespace e3