
#pragma once

#include <string>
#include "JuceHeader.h"
#include "core/GlobalHeader.h"
#include "gui/Widgets.h"



namespace e3
{
    class ParameterPanel;


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

        ParameterPanel* owner_;
    };
    
    
    class ModuleParameterPanel : public Component, public Label::Listener
    {
    public:
        ModuleParameterPanel(ParameterPanel* owner);

        void resized() override;
        void paint( Graphics& g ) override;
        void update( Module* module );
        void labelTextChanged( Label *labelThatHasChanged ) override;

    protected:
        ParameterPanel* owner_;

        Label headerLabel_;
    };


    class ParameterPanel : public Component
    {
    public:
        ParameterPanel();

        void resized() override;

        void showInstrument( Instrument* instrument );
        void showModule( Module* module );

        Gallant::Signal2<const std::string&, var> instrumentAttributesSignal;     // attributeName, value
  
    protected:
        ScopedPointer<InstrumentParameterPanel> instrumentPanel_;
        ScopedPointer<ModuleParameterPanel> modulePanel_;
    };


} // namespace e3