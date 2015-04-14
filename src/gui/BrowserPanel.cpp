
#include "gui/BrowserPanel.h"


namespace e3
{
    BrowserPanel::BrowserPanel() : Component()
    {
        bankBrowser_       = new Component();
        instrumentBrowser_ = new Component();
        tabPanel_          = new TabbedComponent(TabbedButtonBar::TabsAtTop);

        tabPanel_->addTab("Bank", Colours::grey, bankBrowser_, false);
        tabPanel_->addTab("Instrument", Colours::grey, instrumentBrowser_, false);

        addAndMakeVisible(tabPanel_);
    }


    void BrowserPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();

        int width = jmax(300, (int)(content.getWidth() * 0.75));
        tabPanel_->setBounds(content.withWidth(width));
    }

} // namespace e3