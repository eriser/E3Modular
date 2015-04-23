
#pragma once

#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {

    enum MonitorEventType {
        kMonitorNote,
        kMonitorController,
        kMonitorPitchbend,
        kMonitorVoices,
        kMonitorCpuMeter,
    };


    struct MonitorEvent 
    {
        MonitorEvent(MonitorEventType type, double value1=-1, double value2=-1, int16_t numVoices=-1) :
            type(type),
            numVoices(numVoices),
            value1(value1),
            value2(value2)
        {}

        MonitorEventType type;
        int16_t numVoices;
        double value1;
        double value2;
    };

    class MonitorUpdater : public AsyncUpdater
    {
    public:
        Gallant::Signal1<MonitorEvent> monitorUpdateSignal;

        void monitorVoiceEvent(int16_t numSounding);
        void monitorCpuMeterEvent(double value);

        void monitorNoteEvent(double pitch, double gate);
        void monitorControllerEvent(int controllerId, int value);
        void monitorPitchbendEvent(int value);

    protected:
        std::vector<MonitorEvent> pendingEvents_;

        void handleAsyncUpdate() override;
    };


} // namespace e3