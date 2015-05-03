
#include <e3_Trace.h>
#include "core/Module.h"
#include "core/Instrument.h"
#include "gui/ParameterPanel.h"


namespace e3 {

    ParameterPanel::ParameterPanel()
    {
        headerLabel_ = new Label( "Header", "" );
        headerLabel_->setFont( Font( 18, Font::bold ) );
        headerLabel_->setEditable( false, true, true );
        headerLabel_->addListener( this );
        addAndMakeVisible( headerLabel_ );
    }


    void ParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();

        headerLabel_->setBounds( bounds.withHeight( 25 ) );
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

        headerLabel_->setText( instrument->name_, dontSendNotification );
    }


    void ParameterPanel::showModule( Module* module )
    {
        ASSERT( module );
        if (module == nullptr) return;

        headerLabel_->setText( module->getLabel(), dontSendNotification );
    }


    void ParameterPanel::labelTextChanged( Label* label )
    {
        TRACE( "ParameterPanel::labelTextChanged, text=%s\n", label->getText().toRawUTF8() );
    }


} // namespace e3
