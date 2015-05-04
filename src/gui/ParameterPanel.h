
#pragma once

#include "JuceHeader.h"


namespace e3
{
    class InstrumentParameterPanel : public Component, 
                                     public Label::Listener
    { 
    public:
        InstrumentParameterPanel();

        void paint( Graphics& g ) override;
        void resized() override;
        void update( Instrument* instrument );
        void labelTextChanged( Label *labelThatHasChanged ) override;

    protected:
        Label headerLabel_;
        Label nameLabel_;
        Label nameEditor_;
        Label categoryLabel_;
        Label categoryEditor_;
        Label commentLabel_;
        Label commentEditor_;
    };
    
    
    class ModuleParameterPanel : public Component, public Label::Listener
    {
    public:
        ModuleParameterPanel();

        void paint( Graphics& g ) override;
        void resized() override;
        void update( Module* module );
        void labelTextChanged( Label *labelThatHasChanged ) override;

    protected:
        Label headerLabel_;
    };


    class ParameterPanel : public Component
    {
    public:
        ParameterPanel();

        void resized() override;
        void paint( Graphics& g );

        void showInstrument( Instrument* instrument );
        void showModule( Module* module );

    protected:
        ScopedPointer<InstrumentParameterPanel> instrumentPanel_;
        ScopedPointer<ModuleParameterPanel> modulePanel_;
    };


} // namespace e3