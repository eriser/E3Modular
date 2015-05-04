
#include <e3_Trace.h>
#include "core/Module.h"
#include "core/Instrument.h"
#include "gui/Style.h"
#include "gui/ParameterPanel.h"


namespace e3 {


    //--------------------------------------------------------------
    // class InstrumentParameterPanel
    //--------------------------------------------------------------

    InstrumentParameterPanel::InstrumentParameterPanel()
    {
        
        headerLabel_.setFont( Font( 18, Font::bold ) );
        headerLabel_.setEditable( false, true, true );
        headerLabel_.addListener( this );
        headerLabel_.setText( "Instrument", dontSendNotification );
        addAndMakeVisible( &headerLabel_ );

        nameLabel_.setText( "Instrument Name:", dontSendNotification );
        categoryLabel_.setText( "Category:", dontSendNotification );
        commentLabel_.setText( "Comment:", dontSendNotification );

        Label* labels[]  = { &nameLabel_, &categoryLabel_, &commentLabel_ };
        Label* editors[] = { &nameEditor_, &categoryEditor_, &commentEditor_ };

        for (int i = 0; i < ARRAY_SIZE( labels ); i++)
        {
            labels[i]->setFont( Font( 10, Font::plain ) );
            addAndMakeVisible( labels[i] );
        }

        Style& style = Style::getInstance();
        for (int i = 0; i < ARRAY_SIZE( editors ); i++)
        {
            editors[i]->setFont( Font( 12, Font::plain ) );
            editors[i]->setColour( Label::textColourId, style.findColour( TextEditor::textColourId ) );
            editors[i]->setColour( Label::outlineColourId, style.findColour( TextEditor::textColourId) );
            editors[i]->setEditable( true );
            editors[i]->addListener( this );
            addAndMakeVisible( editors[i] );
        }
    }


    void InstrumentParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();
        int l = 10;
        int w = bounds.getWidth() - 20;

        headerLabel_.setBounds( bounds.withHeight( 25 ) );

        nameLabel_.setBounds(      l, 100, w, 20 );
        nameEditor_.setBounds(     l, 120, w, 20 );
        categoryLabel_.setBounds(  l, 145, w, 20 );
        categoryEditor_.setBounds( l, 165, w, 20 );
        commentLabel_.setBounds(   l, 190, w, 20 );
        commentEditor_.setBounds(  l, 210, w, 20 );
    }


    void InstrumentParameterPanel::paint( Graphics& g )
    {
        g.setColour( findColour( Style::BackgroundColourId ) );
        g.fillAll();
    }


    void InstrumentParameterPanel::update( Instrument* instrument )
    {
        nameEditor_.setText( instrument->name_, dontSendNotification );
        categoryEditor_.setText( instrument->category_, dontSendNotification );
        commentEditor_.setText( instrument->comment_, dontSendNotification );

    }


    void InstrumentParameterPanel::labelTextChanged( Label* label )
    {
        TRACE( "InstrumentParameterPanel::labelTextChanged, text=%s\n", label->getText().toRawUTF8() );
    }



    //--------------------------------------------------------------
    // class ModuleParameterPanel
    //--------------------------------------------------------------

    ModuleParameterPanel::ModuleParameterPanel()
    {
        headerLabel_.setFont( Font( 18, Font::bold ) );
        headerLabel_.setEditable( false, true, true );
        headerLabel_.addListener( this );
        addAndMakeVisible( &headerLabel_ );
    }


    void ModuleParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();

        headerLabel_.setBounds( bounds.withHeight( 25 ) );
    }


    void ModuleParameterPanel::paint( Graphics& g )
    {
        g.setColour( findColour( Style::BackgroundColourId ) );
        g.fillAll();
    }


    void ModuleParameterPanel::update( Module* module )
    {
        headerLabel_.setText( module->getLabel(), dontSendNotification );
    }


    void ModuleParameterPanel::labelTextChanged( Label* label )
    {
        TRACE( "ModuleParameterPanel::labelTextChanged, text=%s\n", label->getText().toRawUTF8() );
    }


    //--------------------------------------------------------------
    // class ParameterPanel
    //--------------------------------------------------------------

    ParameterPanel::ParameterPanel()
    {
        instrumentPanel_ = new InstrumentParameterPanel();
        modulePanel_     = new ModuleParameterPanel();

        addChildComponent( instrumentPanel_ );
        addChildComponent( modulePanel_ );
    }


    void ParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();

        instrumentPanel_->setBounds( bounds );
        modulePanel_->setBounds( bounds );
    }


    void ParameterPanel::paint( Graphics& )
    {
        //g.setColour( Colours::blue );
        //g.fillAll();
    }


    void ParameterPanel::showInstrument( Instrument* instrument )
    {
        ASSERT( instrument );
        if (instrument == nullptr) return;

        instrumentPanel_->setVisible( true );
        instrumentPanel_->toFront( true );
        instrumentPanel_->update( instrument );
    }


    void ParameterPanel::showModule( Module* module )
    {
        ASSERT( module );
        if (module == nullptr) return;

        modulePanel_->setVisible( true );
        modulePanel_->toFront( true );
        modulePanel_->update( module );
    }


} // namespace e3
