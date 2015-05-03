
#pragma once

#include "JuceHeader.h"


namespace e3
{
    class ParameterPanel : public Component, public Label::Listener
    {
    public:
        ParameterPanel();

        void resized() override;
        void paint( Graphics& g );

        void showInstrument( Instrument* instrument );
        void showModule( Module* module );

        void labelTextChanged( Label *labelThatHasChanged ) override;

    protected:
        ScopedPointer<Label> headerLabel_;
    };


} // namespace e3