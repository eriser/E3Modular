
#include "core/MonitorUpdater.h"

namespace e3 {

    void MonitorUpdater::monitorVoiceEvent(int16_t numSounding)
    {
        pendingEvents_.push_back(MonitorEvent(kMonitorVoices, -1, -1, numSounding));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorCpuMeterEvent(double value)
    {
        pendingEvents_.push_back(MonitorEvent(kMonitorCpuMeter, value));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorNoteEvent(double pitch, double gate)
    {
        pendingEvents_.push_back(MonitorEvent(kMonitorNote, pitch, gate));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorControllerEvent(int controllerId, int value)
    {
        pendingEvents_.push_back(MonitorEvent(kMonitorController, controllerId, value));
        triggerAsyncUpdate();
    }


    void MonitorUpdater::monitorPitchbendEvent(int value)
    {
        pendingEvents_.push_back(MonitorEvent(kMonitorPitchbend, value));
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