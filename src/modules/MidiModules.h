
#pragma once

#include <string>
#include "core/Module.h"


namespace e3 {
    class Polyphony;

    class MidiGate : public Module 
    {
    public:
        MidiGate();

        void connectSignals() override;
        void disconnectSignals() override;
        void onMidiGate( double gate, uint16_t voice);

        std::string debugLabel_ = "MidiGate";

    protected:
        EventOutport gateOutport_;
    };


    class MidiPitch : public Module 
    {
    public:
        MidiPitch();
        MidiPitch(
            ModuleType moduleType,
            const std::string& label,
            VoicingType voicingType,
            ProcessingType processingType);

        void connectSignals() override;
        void disconnectSignals() override;

        void initVoices() override;
        void setParameter(int paramId, double value, double modulation=0.f, int16_t voice=-1) override;
        void onMidiNote( double pitch, double gate, uint16_t voice );
        void onMidiPitchbend(int value);
        
        void processControl() throw() override;

        std::string debugLabel_ = "MidiPitch";

    protected:
        void createParameters();
        void calcGlide(double freq, uint16_t voice);
        void setGlideTime(double time);

        enum Params { 
            ParamBendRange  = 0,
            ParamGlideTime  = 1,
            ParamGlideAuto  = 2,
        };

        Buffer< double > glideDeltaBuffer_, glideTargetBuffer_, freqBuffer_;

        double* freq_        = nullptr;
        double* glideTarget_ = nullptr;
        double* glideDelta_  = nullptr;

        double glideFrames_  = 0;
        bool glideAuto_      = true;

        double bendFactor_   = 1.0;
        int16_t bendRange_   = 2;

        EventOutport pitchOutport_;
    };


    class MidiInput : public MidiPitch
    {
    public:
        MidiInput();

        void connectSignals() override;
        void disconnectSignals() override;
        void onMidiNote(double pitch, double gate, uint16_t voice);

        std::string debugLabel_ = "MidiInput";

    protected:
        EventOutport gateOutport_;
    };

}
