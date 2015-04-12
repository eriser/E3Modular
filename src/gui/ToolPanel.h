
#pragma once

#include <string>
#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {
    class MidiMonitor;

    class ToolPanel : public Component, public Button::Listener
    {
    public:
        ToolPanel();

        void addPanelButton(const std::string& buttonLabel, int indexToInsert);
        void selectPanel(int id);

        void monitorNoteEvent(int numSounding, double pitch, double gate, int flags);

        void paint(Graphics& g) override;
        void resized() override;

        Gallant::Signal1<int> panelSelectedSignal;

    private:
        void buttonClicked(Button* button) override;

        OwnedArray<Button> panelButtons_;
        ScopedPointer<MidiMonitor> midiMonitor_;
    };

} // namespace e3