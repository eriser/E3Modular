
#pragma once

#include <string>
#include "JuceHeader.h"
#include "core/GlobalHeader.h"
#include "gui/Widgets.h"



namespace e3
{
    class Module;
    class Instrument;
    class ParameterPanel;
    class ParameterStrip;


    class InstrumentParameterPanel : public Component, 
                                     public Label::Listener,
                                     public Button::Listener
    { 
    public:
        InstrumentParameterPanel(ParameterPanel* owner);

        void resized() override;
        void paint( Graphics& g ) override;
        void update( Instrument* instrument );

        void labelTextChanged( Label *labelThatHasChanged ) override;
        void buttonClicked( Button* button ) override;
        //void buttonStateChanged( Button* button ) override;

    protected:
        Label headerLabel_;
        Label nameLabel_;
        Label categoryLabel_;
        Label commentLabel_;
        Label nameEditor_;
        Label categoryEditor_;
        Label commentEditor_;
        Label voicesLabel_;
        Label unisonLabel_;
        Label spreadLabel_;
        NumericLabel voicesEditor_;
        NumericLabel unisonEditor_;
        NumericLabel spreadEditor_;

        ToggleButton holdButton_;
        ToggleButton retriggerButton_;
        ToggleButton legatoButton_;

        Label presetLabel_;
        Label instrumentLabel_;
        ComboBox presetBox_;
        ComboBox instrumentBox_;

        ParameterPanel* owner_;
    };
    
    
    class ModuleParameterPanel : public Component, public Label::Listener
    {
    public:
        ModuleParameterPanel(ParameterPanel* owner);

        void resized() override;
        void paint( Graphics& g ) override;
        void update( Instrument* instrument, Module* module );
        void labelTextChanged( Label *labelThatHasChanged ) override;

    protected:
        void addParameters( ParameterSet& parameters, Module* module, Rectangle<int>& r );
        void removeAllParameters();

        ParameterPanel* owner_;

        Label headerLabel_;
        OwnedArray<ParameterStrip> parameters_;
    };


    //--------------------------------------------------------------------------------------
    // class ParameterStrip
    //--------------------------------------------------------------------------------------

    class ParameterStrip : public Component, public Slider::Listener, public Button::Listener
    {
    public:
        ParameterStrip( const Rectangle<int>& bounds, Module* module, const Parameter* parameter );

        void sliderValueChanged( Slider* slider ) override;
        void buttonClicked( Button* button ) override;

        //void mouseDown( const MouseEvent& e ) override;

        //void valueChanged( Component* source );
        //void attachOrDetach();
        //void onController( int controllerId, double value );

    protected:
        //void addControl( const Rectangle<int> bounds, Module* module );
        //CMouseEventResult showCtrlDialog( const CPoint& pos );

        Module* module_;
        const Parameter* parameter_;
        Label label_;
        Slider slider_;
        ToggleButton button_;
    };

    
    class ParameterPanel : public Component
    {
    public:
        ParameterPanel();

        void resized() override;

        void showInstrument( Instrument* instrument );
        void showModule( Instrument* instrument, Module* module );

        Gallant::Signal2<const std::string&, var> instrumentAttributesSignal;     // attributeName, value
  
    protected:
        ScopedPointer<InstrumentParameterPanel> instrumentPanel_;
        ScopedPointer<ModuleParameterPanel> modulePanel_;
    };


} // namespace e3