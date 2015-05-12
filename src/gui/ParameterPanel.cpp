
#include <e3_Trace.h>
#include "core/Module.h"
#include "core/Instrument.h"
#include "gui/Style.h"
#include "gui/ParameterPanel.h"


namespace e3 {




    //--------------------------------------------------------------
    // class InstrumentParameterPanel
    //--------------------------------------------------------------

    InstrumentParameterPanel::InstrumentParameterPanel(ParameterPanel* owner) :
        owner_(owner)
    {
        Style& style = Style::getInstance();
        Colour textColour = style.findColour( TextEditor::textColourId );
        
        headerLabel_.setFont( Font( 18, Font::bold ) );
        headerLabel_.setColour( Label::textColourId, textColour );
        headerLabel_.setEditable( false, true, true );
        headerLabel_.setBorderSize( BorderSize<int>( 0, 0, 0, 0 ) );
        headerLabel_.addListener( this );
        headerLabel_.setText( "Instrument", dontSendNotification );
        addAndMakeVisible( &headerLabel_ );

        nameLabel_.setText( "Name:", dontSendNotification );
        categoryLabel_.setText( "Category:", dontSendNotification );
        commentLabel_.setText( "Comment:", dontSendNotification );
        voicesLabel_.setText( "Voices:", dontSendNotification );
        unisonLabel_.setText( "Unison:", dontSendNotification );
        spreadLabel_.setText( "Spread:", dontSendNotification );

        nameEditor_.setName( "name" );
        categoryEditor_.setName( "category" );
        commentEditor_.setName( "comment" );

        voicesEditor_.setName( "numVoices" );
        voicesEditor_.setRange( 1, 999 );
        voicesEditor_.setJustificationType( Justification::centredRight );
        unisonEditor_.setName( "numUnison" );
        unisonEditor_.setRange( 1, 10 );
        unisonEditor_.setJustificationType( Justification::centredRight );
        spreadEditor_.setName( "unisonSpread" );
        spreadEditor_.setRange( 0, 1200 );
        spreadEditor_.setJustificationType( Justification::centredRight );

        holdButton_.setName( "hold" );
        holdButton_.setButtonText( "Hold" );
        retriggerButton_.setName( "retrigger" );
        retriggerButton_.setButtonText( "Mono/Retrigger" );
        legatoButton_.setName( "legato" );
        legatoButton_.setButtonText( "Mono/Legato" );

        Label* labels[]          = { &nameLabel_, &categoryLabel_, &commentLabel_, 
                                     &voicesLabel_, &unisonLabel_, &spreadLabel_ };
        Label* editors[]         = { &nameEditor_, &categoryEditor_, &commentEditor_,
                                     &voicesEditor_, &unisonEditor_, &spreadEditor_ };
        ToggleButton* buttons[]  = { &holdButton_, &retriggerButton_, &legatoButton_ };

        for (int i = 0; i < ARRAY_SIZE( labels ); i++)
        {
            labels[i]->setFont( Font( 10, Font::plain ) );
            labels[i]->setColour( Label::textColourId, textColour );
            labels[i]->setBorderSize( BorderSize<int>(0, 0, 0, 0) );
            addAndMakeVisible( labels[i] );
        }

        for (int i = 0; i < ARRAY_SIZE( editors ); i++)
        {
            editors[i]->setFont( Font( 12, Font::plain ) );
            editors[i]->setColour( Label::textColourId, textColour );
            editors[i]->setColour( Label::outlineColourId, textColour );
            editors[i]->setEditable( true );
            editors[i]->addListener( this );
            addAndMakeVisible( editors[i] );
        }

        for (int i = 0; i < ARRAY_SIZE( buttons ); i++)
        {
            buttons[i]->addListener( this );
            addAndMakeVisible( buttons[i] );
        }
    }


    void InstrumentParameterPanel::resized()
    {
        int t = 40;
        int l = 10;
        int w = 200 - 20;
        headerLabel_.setBounds( l, 0, w, 25 );

        nameLabel_.setBounds( l, t, w, 20 );
        nameEditor_.setBounds( l, t + 20, w, 20 );
        categoryLabel_.setBounds( l, t + 45, w, 20 );
        categoryEditor_.setBounds( l, t + 65, w, 20 );
        commentLabel_.setBounds( l, t + 90, w, 20 );
        commentEditor_.setBounds( l, t + 110, w, 20 );

        t = getHeight() - 150;
        holdButton_.setBounds(      l, t, w, 20 );
        retriggerButton_.setBounds( l, t + 30, w, 20 );
        legatoButton_.setBounds(    l, t + 60, w, 20 );

        t = getHeight() - 40;
        voicesLabel_.setBounds( l, t, 55, 20 );
        unisonLabel_.setBounds( l + 65, t, 55, 20 );
        spreadLabel_.setBounds( l + 130, t, 55, 20 );

        voicesEditor_.setBounds( l, t + 20, 60, 20 );
        unisonEditor_.setBounds( l + 65, t + 20, 60, 20 );
        spreadEditor_.setBounds( l + 130, t + 20, 60, 20 );
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

        holdButton_.setToggleState( instrument->hold_, dontSendNotification );
        retriggerButton_.setToggleState( instrument->retrigger_, dontSendNotification );
        legatoButton_.setToggleState( instrument->legato_, dontSendNotification );

        voicesEditor_.setText( String(instrument->numVoices_), dontSendNotification );
        unisonEditor_.setText( String( instrument->numUnison_), dontSendNotification );
        spreadEditor_.setText( String( instrument->unisonSpread_), dontSendNotification );
    }


    void InstrumentParameterPanel::labelTextChanged( Label* label )
    {
        owner_->instrumentAttributesSignal( label->getName().toStdString(), label->getText() );
    }


    void InstrumentParameterPanel::buttonClicked( Button* button )
    {
        TRACE( "InstrumentParameterPanel::buttonClicked, text=%s state=%d\n", button->getName().toRawUTF8(), button->getToggleState() );
        owner_->instrumentAttributesSignal( button->getName().toStdString(), button->getToggleState() );
    }


    //void InstrumentParameterPanel::buttonStateChanged( Button* button )
    //{
    //    TRACE( "InstrumentParameterPanel::buttonStateChanged, text=%s state=%d\n", button->getName().toRawUTF8(), button->getToggleState() );
    //}




    //--------------------------------------------------------------
    // class ModuleParameterPanel
    //--------------------------------------------------------------

    ModuleParameterPanel::ModuleParameterPanel(ParameterPanel* owner) :
        owner_(owner)
    {
        Style& style = Style::getInstance();
        Colour textColour = style.findColour( TextEditor::textColourId );

        headerLabel_.setFont( Font( 18, Font::bold ) );
        headerLabel_.setColour( Label::textColourId, textColour );
        headerLabel_.setEditable( false, true, true );
        headerLabel_.setBorderSize( BorderSize<int>( 0, 0, 0, 0 ) );
        headerLabel_.addListener( this );
        headerLabel_.setText( "Instrument", dontSendNotification );
        addAndMakeVisible( &headerLabel_ );
    }


    void ModuleParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();

        int w = getWidth();

        headerLabel_.setBounds( 10, 0, w - 20, 25 );
    }


    void ModuleParameterPanel::paint( Graphics& g )
    {
        g.setColour( findColour( Style::BackgroundColourId ) );
        g.fillAll();
    }


    void ModuleParameterPanel::update( Instrument* instrument, Module* module )
    {
        headerLabel_.setText( module->getLabel(), dontSendNotification );

        removeAllParameters();

        Rectangle<int> r( 0, 50, 200, 25 );
        const Preset& preset = instrument->getPreset();
        addParameters( preset.getModuleParameters(), module, r );
        addParameters( preset.getLinkParameters(), module, r );
    }


    void ModuleParameterPanel::addParameters( ParameterSet& parameters, Module* module, Rectangle<int>& r )
    {
        int id = module->getId();

        for (ParameterSet::iterator it = parameters.moduleFirst( id ); it != parameters.moduleLast( id ); ++it)
        {
            const Parameter& p = *it;
            ParameterStrip* strip = new ParameterStrip( r, module, &p );
            parameters_.add( strip );
            addAndMakeVisible( strip );
            r.translate( 0, 30 );
        }
    }


    void ModuleParameterPanel::removeAllParameters()
    {
        for( int i = 0; i < parameters_.size(); ++i )
        {
            ParameterStrip* strip = parameters_.getUnchecked( i );
            removeChildComponent( strip );
        }
        parameters_.clear();
    }


    void ModuleParameterPanel::labelTextChanged( Label* label )
    {
        TRACE( "ModuleParameterPanel::labelTextChanged, text=%s\n", label->getText().toRawUTF8() );
        owner_->instrumentAttributesSignal( label->getName().toStdString(), label->getText() );
    }


    //--------------------------------------------------------------
    // class ParameterStrip
    //--------------------------------------------------------------

    ParameterStrip::ParameterStrip( const Rectangle<int>& bounds, Module* module, const Parameter* parameter ) :
        module_(module),
        parameter_(parameter)
    {
        setBounds( bounds );

        Style& style = Style::getInstance();
        Colour textColour = style.findColour( TextEditor::textColourId );

        label_.setText( parameter->label_, dontSendNotification );
        label_.setColour( Label::textColourId, textColour );
        label_.setEditable( false, true, true );
        label_.setBorderSize( BorderSize<int>( 0, 0, 0, 0 ) );
        label_.setFont( Font( 10, Font::plain ) );
        addAndMakeVisible( &label_ );

        switch( parameter_->controlType_ )
        {
        case ControlSlider:
        case ControlBiSlider:
        {
            slider_.setPopupMenuEnabled( false );
            slider_.setSliderStyle( Slider::LinearBar );
            slider_.setRange( parameter_->getMin(), parameter_->getMax(), parameter_->getInterval() );
            slider_.setValue( parameter_->value_, dontSendNotification );
            slider_.addListener( this );

            slider_.setBounds( 10, 0, 120, 20 );
            label_.setBounds( 140, 0, 80, 20 );
            addAndMakeVisible( &slider_ );
            break;
        }
        case ControlCheckbox:
            break;
        case ControlNumEdit:
            break;
        }
    }


    void ParameterStrip::sliderValueChanged( Slider* slider )
    {
        ASSERT( slider == &slider_ );
        parameter_->value_ = slider->getValue();
        module_->setParameter( *parameter_ );
    }



    //--------------------------------------------------------------
    // class ParameterPanel
    //--------------------------------------------------------------

    ParameterPanel::ParameterPanel()
    {
        instrumentPanel_ = new InstrumentParameterPanel( this );
        modulePanel_     = new ModuleParameterPanel( this );

        addChildComponent( instrumentPanel_ );
        addChildComponent( modulePanel_ );
    }


    void ParameterPanel::resized()
    {
        Rectangle<int> bounds = getLocalBounds();

        instrumentPanel_->setBounds( bounds );
        modulePanel_->setBounds( bounds );
    }


    void ParameterPanel::showInstrument( Instrument* instrument )
    {
        ASSERT( instrument );
        if (instrument == nullptr) return;

        if (instrumentPanel_->isVisible() == false) {
            instrumentPanel_->setVisible( true );
            instrumentPanel_->toFront( true );
            modulePanel_->setVisible( false );
        }
        instrumentPanel_->update( instrument );
    }


    void ParameterPanel::showModule( Instrument* instrument, Module* module )
    {
        ASSERT( module );
        if (module == nullptr) return;

        if (modulePanel_->isVisible() == false) 
        {
            modulePanel_->setVisible( true );
            modulePanel_->toFront( true );
            modulePanel_->update( instrument, module );
            instrumentPanel_->setVisible( false );
        }
    }


} // namespace e3
