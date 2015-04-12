
#pragma once

#include "JuceHeader.h"

namespace e3 {

    //class MonitorLabel : public Label
    //{
    //    void paint(Graphics& g) override;
    //};



    class MidiMonitor : public Component
    {
    public:
        MidiMonitor();

        void monitorNoteEvent(int numSounding, double pitch, double gate, int flags);

    private:
        void update();

        ScopedPointer<Label> voicesLabel_;
        ScopedPointer<Label> pitchLabel_;
        ScopedPointer<Label> gateLabel_;
    };

} // namespace e3