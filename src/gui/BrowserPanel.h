
#pragma once

#include "JuceHeader.h"


namespace e3
{
    class TabComponent;
    class InstrumentBrowser;
    class DatabaseBrowser;

    class BrowserPanel : public Component
    {
    public:
        void resized() override;

        void BrowserPanel::setComponents( InstrumentBrowser* instrumentBrowser, DatabaseBrowser* browser );
        void updateContents( XmlElement* root );
        XmlElement* getSelectedInstrumentXml();

    protected:
        ScopedPointer<TabComponent> tabPanel_;
        InstrumentBrowser* instrumentBrowser_ = nullptr;
        DatabaseBrowser* browser_ = nullptr;

        TextButton openButton_;
        TextButton saveButton_;
        TextButton saveAsButton_;
        TextButton newButton_;
    };


} // namespace e3