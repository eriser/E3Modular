
#include "gui/Style.h"
#include "gui/TabComponent.h"
//#include "gui/TabButtonGroup.h"
#include "gui/TableComponent.h"
#include "gui/BrowserPanel.h"


namespace e3
{
    BrowserPanel::BrowserPanel() : Component()
    {
        //Colour bkgndColour = getLookAndFeel().findColour(Style::kContentBackground1ColourId);
        bankBrowser_       = new TableComponent();
        instrumentBrowser_ = new Component();

        //tabPanel_ = new TabbedComponent(TabbedButtonBar::TabsAtTop);
        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtTop, 10);
        tabPanel_->addTab("Bank", Colours::transparentBlack, bankBrowser_, false, 0);
        tabPanel_->addTab("Instrument", Colours::transparentBlack, instrumentBrowser_, false, 1);
        //tabPanel_->setIndent(0);

        //tabPanel_          = new StackedPanel();
        //tabButtons_        = new TabButtonGroup(43);

        //tabButtons_->addButton("Bank", kBankTab);
        //tabButtons_->addButton("Instrument", kInstrumentTab);

        //tabPanel_->insertPanel(bankBrowser_, kBankTab);
        //tabPanel_->insertPanel(instrumentBrowser_, kInstrumentTab);
        //tabButtons_->triggerButton(kBankTab);

        //tabButtons_->tabButtonClickedSignal.Connect(tabPanel_.get(), &StackedPanel::showPanel);

        addAndMakeVisible(tabPanel_);
        //tabPanel_->resized();
        //tabPanel_->repaint();
        //addAndMakeVisible(tabButtons_);
    }


    void BrowserPanel::resized()
    {
        //tabButtons_->setBounds(0, 20, 250, 25);
        //
        //Rectangle<int> content = getLocalBounds();

        //int width = jmax(300, (int)(content.getWidth() * 0.75));
        //tabPanel_->setBounds(content.withWidth(width).withTrimmedTop(50));

        Rectangle<int> content = getLocalBounds();

        int width = jmax(300, (int)(content.getWidth() * 0.75));
        tabPanel_->setBounds(content.withWidth(width));
    }


} // namespace e3