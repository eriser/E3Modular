
#pragma once

#include <string>
#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {
    class VoiceMonitor;

    class ToolPanel : public Component, public Button::Listener
    {
    public:
        ToolPanel();

        void addPanelButton(const std::string& buttonLabel, int indexToInsert);
        void selectPanel(int id);

        void monitorMidiEvent(VoiceMonitorEvent evt);

        void paint(Graphics& g) override;
        void resized() override;

        Gallant::Signal1<int> panelSelectedSignal;

    private:
        void buttonClicked(Button* button) override;

        OwnedArray<Button> panelButtons_;
        ScopedPointer<VoiceMonitor> midiMonitor_;
    };

} // namespace e3