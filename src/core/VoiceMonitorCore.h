
#pragma once

#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {

    enum VoiceMonitorEventType {
        kMonitorNote,
        kMonitorController,
        kMonitorPitchbend,
        kMonitorVoices,
    };


    struct VoiceMonitorEvent 
    {
        VoiceMonitorEvent(VoiceMonitorEventType type, double value1=-1, double value2=-1, int16_t numVoices=-1) :
            type(type),
            numVoices(numVoices),
            value1(value1),
            value2(value2)
        {}

        VoiceMonitorEventType type;
        int16_t numVoices;
        double value1;
        double value2;
    };

    class VoiceMonitorUpdater : public AsyncUpdater
    {
    public:
        Gallant::Signal1<VoiceMonitorEvent> monitorUpdateSignal;

    protected:
        void monitorVoiceEvent(int16_t numSounding);
        void monitorNoteEvent(double pitch, double gate);
        void monitorControllerEvent(int controllerId, int value);
        void monitorPitchbendEvent(int value);


        std::vector<VoiceMonitorEvent> pendingEvents_;

        void handleAsyncUpdate() override;
    };


} // namespace e3