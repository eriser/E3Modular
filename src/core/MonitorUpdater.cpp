
#include "core/MonitorUpdater.h"

namespace e3 {

    void MonitorUpdater::monitorVoiceEvent(int numSounding)
    {
        pendingEvents_.push_back(MonitorEvent(MonitorVoices, -1, -1, numSounding));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorCpuMeterEvent(double value)
    {
        pendingEvents_.push_back(MonitorEvent(MonitorCpuMeter, value));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorNoteEvent(double pitch, double gate)
    {
        pendingEvents_.push_back(MonitorEvent(MonitorNote, pitch, gate));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorControllerEvent(int controllerId, int value)
    {
        pendingEvents_.push_back(MonitorEvent(MonitorController, controllerId, value));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorPitchbendEvent(int value)
    {
        pendingEvents_.push_back(MonitorEvent(MonitorPitchbend, value));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::handleAsyncUpdate()
    {
        for (size_t i = 0; i < pendingEvents_.size(); i++) {
            monitorUpdateSignal(pendingEvents_[i]);
        }
        pendingEvents_.clear();
    }


} // namespace e3