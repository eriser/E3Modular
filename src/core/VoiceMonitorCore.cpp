
#include "core/VoiceMonitorCore.h"

namespace e3 {

    void VoiceMonitorUpdater::monitorVoiceEvent(int16_t numSounding)
    {
        pendingEvents_.push_back(VoiceMonitorEvent(kMonitorVoices, -1, -1, numSounding));
        triggerAsyncUpdate();
    }


    void VoiceMonitorUpdater::monitorNoteEvent(double pitch, double gate)
    {
        pendingEvents_.push_back(VoiceMonitorEvent(kMonitorNote, pitch, gate));
        triggerAsyncUpdate();
    }


    void VoiceMonitorUpdater::monitorControllerEvent(int controllerId, int value)
    {
        pendingEvents_.push_back(VoiceMonitorEvent(kMonitorController, controllerId, value));
        triggerAsyncUpdate();
    }


    void VoiceMonitorUpdater::monitorPitchbendEvent(int value)
    {
        pendingEvents_.push_back(VoiceMonitorEvent(kMonitorPitchbend, value));
        triggerAsyncUpdate();
    }


    void VoiceMonitorUpdater::handleAsyncUpdate()
    {
        for (size_t i = 0; i < pendingEvents_.size(); i++) {
            monitorUpdateSignal(pendingEvents_[i]);
        }
        pendingEvents_.clear();
    }


} // namespace e3