
#include "gui/Style.h"
#include "gui/CommandTarget.h"
#include "gui/TabComponent.h"
#include "gui/InstrumentBrowser.h"
#include "gui/DatabaseBrowser.h"
#include "gui/BrowserPanel.h"


namespace e3
{
    void BrowserPanel::setComponents( InstrumentBrowser* instrumentBrowser, DatabaseBrowser* browser )
    {
        instrumentBrowser_ = instrumentBrowser;
        browser_           = browser;

        instrumentBrowser_->addColumn( "Id", InstrumentBrowser::IdColumn, 20 );
        instrumentBrowser_->addColumn( "Name", InstrumentBrowser::NameColumn, 200 );
        instrumentBrowser_->addColumn( "Category", InstrumentBrowser::CategoryColumn, 200 );
        instrumentBrowser_->addColumn( "Comment",  InstrumentBrowser::CommentColumn, 200 );

        tabPanel_ = new TabComponent( TabbedButtonBar::TabsAtTop, 10 );
        tabPanel_->addTab( "Database", Colours::transparentBlack, browser_, false, 0 );
        tabPanel_->addTab( "Instruments", Colours::transparentBlack, instrumentBrowser_, false, 1 );
        addAndMakeVisible( tabPanel_ );

        openButton_.setButtonText( "Open" );
        openButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdOpenInstrument, true );
        addAndMakeVisible( &openButton_ );

        saveButton_.setButtonText( "Save" );
        saveButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdSaveInstrument, true );
        addAndMakeVisible( &saveButton_ );

        saveAsButton_.setButtonText( "Save As" );
        saveAsButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdSaveInstrumentAs, true );
        addAndMakeVisible( &saveAsButton_ );

        newButton_.setButtonText( "New" );
        newButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdNewInstrument, true );
        addAndMakeVisible( &newButton_ );
    }


    void BrowserPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        int separator = jmax( 300, (int)(content.getWidth() * 0.75) );

        tabPanel_->setBounds( content.withWidth( separator ) );

        openButton_.setBounds( separator + 62, 40, 120, 25 );
        saveButton_.setBounds( separator + 62, 80, 120, 25 );
        saveAsButton_.setBounds( separator + 62, 120, 120, 25 );
        newButton_.setBounds( separator + 62, 160, 120, 25 );
    }


    void BrowserPanel::updateContents( XmlElement* root )
    {
        if( root != nullptr ) {
            instrumentBrowser_->loadData( root );
        }
    }


    XmlElement* BrowserPanel::getSelectedInstrumentXml()
    {
        return instrumentBrowser_->getActiveItem();
    }

} // namespace e3