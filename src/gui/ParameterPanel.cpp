
#include <e3_Trace.h>
#include "core/Module.h"
#include "core/Instrument.h"
#include "core/Processor.h"
#include "gui/Style.h"
#include "gui/CommandTarget.h"

#include "gui/ParameterPanel.h"


namespace e3 {

    //--------------------------------------------------------------
    // class InstrumentParameterPanel
    //--------------------------------------------------------------

    InstrumentParameterPanel::InstrumentParameterPanel( Processor* processor ) :
        processor_(processor)
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

        nameLabel_.setText( "Name", dontSendNotification );
        fileLabel_.setText( "File", dontSendNotification );
        voicesLabel_.setText( "Voices", dontSendNotification );
        unisonLabel_.setText( "Unison", dontSendNotification );
        spreadLabel_.setText( "Spread", dontSendNotification );
        presetLabel_.setText( "Preset", dontSendNotification );

        nameEditor_.setName( "name" );

        voicesEditor_.setName( "numVoices" );
        voicesEditor_.setRange( 1, 999 );
        voicesEditor_.setJustificationType( Justification::centredRight );
        unisonEditor_.setName( "numUnison" );
        unisonEditor_.setRange( 1, 10 );
        unisonEditor_.setJustificationType( Justification::centredRight );
        spreadEditor_.setName( "unisonSpread" );
        spreadEditor_.setRange( 0, 1200 );
        spreadEditor_.setJustificationType( Justification::centredRight );

        savePresetButton_.setName( "savePreset" );
        savePresetButton_.setButtonText( "Save" );
        addPresetButton_.setName( "addPreset" );
        addPresetButton_.setButtonText( "Add" );
        deletePresetButton_.setName( "deletePreset" );
        deletePresetButton_.setButtonText( "Remove" );

        openInstrumentButton_.setButtonText( "Open" );
        newInstrumentButton_.setButtonText( "New" );
        saveInstrumentButton_.setButtonText( "Save" );
        saveInstrumentAsButton_.setButtonText( "Save As" );

        openInstrumentButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdOpenInstrument, true );
        newInstrumentButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdNewInstrument, true );
        saveInstrumentButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdSaveInstrument, true );
        saveInstrumentAsButton_.setCommandToTrigger( CommandTarget::getCommandManager(), CommandTarget::cmdSaveInstrumentAs, true );

        holdButton_.setName( "hold" );
        holdButton_.setButtonText( "Hold" );
        retriggerButton_.setName( "retrigger" );
        retriggerButton_.setButtonText( "Mono/Retrigger" );
        legatoButton_.setName( "legato" );
        legatoButton_.setButtonText( "Mono/Legato" );

        Label* labels[]    = { &nameLabel_, &presetLabel_, &fileLabel_, &voicesLabel_, &unisonLabel_, &spreadLabel_ };
        Label* editors[]   = { &nameEditor_, &voicesEditor_, &unisonEditor_, &spreadEditor_ };
        Button* buttons1[] = { &holdButton_, &retriggerButton_, &legatoButton_,
                               &savePresetButton_, &addPresetButton_, &deletePresetButton_ };
        Button* buttons2[] = { &openInstrumentButton_, &newInstrumentButton_,
                               &saveInstrumentButton_, &saveInstrumentAsButton_ };

        for (int i = 0; i < ARRAY_SIZE( labels ); i++)
        {
            labels[i]->setFont( Font( 11, Font::plain ) );
            labels[i]->setColour( Label::textColourId, textColour );
            labels[i]->setBorderSize( BorderSize<int>(0, 0, 0, 0) );
            addAndMakeVisible( labels[i] );
        }

        for (int i = 0; i < ARRAY_SIZE( editors ); i++)
        {
            editors[i]->setFont( Font( 15, Font::plain ) );
            editors[i]->setColour( Label::textColourId, textColour );
            editors[i]->setColour( Label::outlineColourId, textColour );
            editors[i]->setEditable( true );
            editors[i]->addListener( this );
            addAndMakeVisible( editors[i] );
        }

        for (int i = 0; i < ARRAY_SIZE( buttons1 ); i++)
        {
            buttons1[i]->addListener( this );
            addAndMakeVisible( buttons1[i] );
        }
        for (int i = 0; i < ARRAY_SIZE( buttons2 ); i++)
        {
            addAndMakeVisible( buttons2[i] );
        }

        presetBox_.setName( "selectPreset" );
        presetBox_.addListener( this );

        addAndMakeVisible( &presetLabel_ );
        addAndMakeVisible( &presetBox_ );
    }


    void InstrumentParameterPanel::resized()
    {
        int h = getHeight();
        bool large = h > 450;
        int t = large ? 50 : 30;
        int o = large ? 0 : 15;
        int l = 10;
        int w = 200 - 20;
        typedef Rectangle<int> rect;

        nameLabel_.setVisible( large );

        headerLabel_.setBounds( l, 0, w, 25 );
        nameLabel_.setBounds(   l, t-10, w, 20 );
        nameEditor_.setBounds(  l, t+10, w, 20 );

        presetLabel_.setBounds(            rect( l, 0, 54, 20 ).withY( t+50-o ) );
        savePresetButton_.setBounds(       rect( l, 0, 54, 20 ).withY( t+70-o ) );
        addPresetButton_.setBounds(        rect( l, 0, 54, 20 ).withY( t+70-o ).withX(l+63) );
        deletePresetButton_.setBounds(     rect( l, 0, 54, 20 ).withY( t+70-o ).withX(l+126) );
                                           
        presetBox_.setBounds(              rect( l, 0, w, 20 ).withY( t+100-o ) );

        o = large ? 0 : 40;
        fileLabel_.setBounds(              rect( l,    0, 85, 20 ).withY( t+160-o ) );
        openInstrumentButton_.setBounds(   rect( l,    0, 85, 20 ).withY( t+180-o ) );
        saveInstrumentButton_.setBounds(   rect( l+95, 0, 85, 20 ).withY( t+180-o ));
        newInstrumentButton_.setBounds(    rect( l,    0, 85, 20 ).withY( t+210-o ));
        saveInstrumentAsButton_.setBounds( rect( l+95, 0, 85, 20 ).withY( t+210-o ));

        t = large ? h - 160 : h - 140;
        holdButton_.setBounds( l, t, w, 20 );
        retriggerButton_.setBounds( l, t + 30, w, 20 );
        legatoButton_.setBounds(    l, t + 60, w, 20 );

        t = h - 50;
        voicesLabel_.setBounds( l,       t, 55, 20 );
        unisonLabel_.setBounds( l + 65,  t, 55, 20 );
        spreadLabel_.setBounds( l + 130, t, 55, 20 );

        voicesEditor_.setBounds( l,       t + 20, 60, 20 );
        unisonEditor_.setBounds( l + 65,  t + 20, 60, 20 );
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

        holdButton_.setToggleState( instrument->hold_, dontSendNotification );
        retriggerButton_.setToggleState( instrument->retrigger_, dontSendNotification );
        legatoButton_.setToggleState( instrument->legato_, dontSendNotification );

        voicesEditor_.setText( String(instrument->numVoices_), dontSendNotification );
        unisonEditor_.setText( String( instrument->numUnison_), dontSendNotification );
        spreadEditor_.setText( String( instrument->unisonSpread_), dontSendNotification );

        updatePresets( instrument );
    }


    void InstrumentParameterPanel::updatePresets( Instrument* instrument )
    {
        presetBox_.clear();
        const PresetSet& presets = instrument->getPresets();
        for (PresetSet::const_iterator it = presets.begin(); it != presets.end(); ++it)
        {
            const Preset& p = *it;
            presetBox_.addItem( p.getName(), p.getId() );
        }

        int id = presets.getCurrentPresetId();
        if (id >= 0) {
            presetBox_.setSelectedId( id );
        }
    }


    void InstrumentParameterPanel::labelTextChanged( Label* label )
    {
        processor_->setInstrumentAttribute( label->getName().toStdString(), label->getText() );
    }


    void InstrumentParameterPanel::buttonClicked( Button* button )
    {
        Instrument* instrument = processor_->getInstrument();
        String name = button->getName();

        if( name == "savePreset" )
        {
            instrument->saveCurrentPreset();
        }
        else if( name == "addPreset" )
        {
            instrument->addPreset();
            updatePresets( instrument );
            presetBox_.showEditor();
        }
        else if( name == "deletePreset" )
        {
            instrument->deleteCurrentPreset();
            updatePresets( instrument );
        }
        else {
            processor_->setInstrumentAttribute( button->getName().toStdString(), button->getToggleState() );
        }
    }


    void InstrumentParameterPanel::comboBoxChanged( CustomComboBox* )
    {
        Instrument* instrument = processor_->getInstrument();
        int selectedId = presetBox_.getSelectedId();
        instrument->loadPreset( selectedId, true );
    }


    void InstrumentParameterPanel::comboBoxTextChanged( CustomComboBox* )
    {
        std::string text = presetBox_.getText();

        Instrument* instrument = processor_->getInstrument();
        instrument->renameCurrentPreset( text );
    }





    //--------------------------------------------------------------
    // class ModuleParameterPanel
    //--------------------------------------------------------------

    ModuleParameterPanel::ModuleParameterPanel()
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

        parameters_.clear();

        Rectangle<int> r( 0, 50, 200, 25 );
        const Preset& preset = instrument->getCurrentPreset();
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


    void ModuleParameterPanel::labelTextChanged( Label* label )
    {
        // TODO: set module label
        
        UNUSED( label );
        //TRACE( "ModuleParameterPanel::labelTextChanged, text=%s\n", label->getText().toRawUTF8() );
        //owner_->instrumentAttributesSignal( label->getName().toStdString(), label->getText() );
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
            button_.setToggleState( parameter_->value_ > 0, dontSendNotification );
            //button_.setButtonText( parameter->label_ );
            label_.setBounds( 140, 0, 80, 20 );
            button_.setBounds( 110, 0, 20, 20 );
            button_.addListener( this );
            addAndMakeVisible( &button_ );
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


    void ParameterStrip::buttonClicked( Button* button )
    {
        ASSERT( button == &button_ );
        parameter_->value_ = button->getToggleState();
        module_->setParameter( *parameter_ );
    }


    //--------------------------------------------------------------
    // class ParameterPanel
    //--------------------------------------------------------------

    ParameterPanel::ParameterPanel( Processor* processor )
    {
        instrumentPanel_ = new InstrumentParameterPanel( processor );
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
