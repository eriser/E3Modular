
#include <sstream>
#include <iomanip>

#include <e3_Trace.h>
#include <e3_CommonMacros.h>
#include "core/GlobalHeader.h"
#include "gui/MidiMonitor.h"

namespace e3 {

    //void MonitorLabel::paint(Graphics& g)
    //{
    //    g.fillAll(label.findColour(Label::backgroundColourId));
    //}


    MidiMonitor::MidiMonitor() : Component()
    {
        voicesLabel_ = new Label();
        pitchLabel_ = new Label();
        gateLabel_ = new Label();

        // TODO: set background color via Component::properties
    }


    void MidiMonitor::monitorNoteEvent(int numSounding, double pitch, double gate, int flags)
    {
        UNUSED(numSounding);
        UNUSED(flags);
        TRACE("note event: pitch: %f gate: %f\n", pitch, gate);

        if (flags & kMonitorVoices)
        {
            std::ostringstream osVoices;
            osVoices << numSounding;
            //lblVoices_->text_ = osVoices.str();
        }
        if (flags & kMonitorPitch)
        {
            if (pitch == -1) {
                //lblPitch_->text_ = "";
            }
            else {
                std::ostringstream osPitch;
                osPitch << std::setprecision(0) << std::fixed << (int16_t)pitch;
                //lblPitch_->text_ = osPitch.str();
            }
        }
        if (flags & kMonitorGate)
        {
            if (gate == -1) {
                //lblGate_->text_ = "";
            }
            else {
                std::ostringstream osGate;
                osGate << std::setprecision(0) << std::fixed << (int16_t)gate;
                //lblGate_->text_ = osGate.str();
            }
        }
        //lblVoices_->setDirty();
        //lblPitch_->setDirty();
        //lblGate_->setDirty();

        //update();
    }

    void MidiMonitor::update()
    {
        //if (lblPitch_->isDirty())
        //    lblPitch_->invalid();
        //if (lblGate_->isDirty())
        //    lblGate_->invalid();
        //if (lblVoices_->isDirty())
        //    lblVoices_->invalid();
    }




} // namespace e3