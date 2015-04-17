
#include "gui/Style.h"
#include "gui/TabComponent.h"
#include "gui/TableComponent.h"
#include "gui/BrowserPanel.h"


namespace e3
{
    BrowserPanel::BrowserPanel() : Component()
    {
        bankBrowser_       = new TableComponent();
        instrumentBrowser_ = new Component();

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtTop, 10);
        tabPanel_->addTab("Bank", Colours::transparentBlack, bankBrowser_, false, 0);
        tabPanel_->addTab("Instrument", Colours::transparentBlack, instrumentBrowser_, false, 1);

        addAndMakeVisible(tabPanel_);
    }


    void BrowserPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();

        int width = jmax(300, (int)(content.getWidth() * 0.75));
        tabPanel_->setBounds(content.withWidth(width));
    }


} // namespace e3