
#pragma once

#include "core/Module.h"


namespace e3 {
    class Polyphony;

    class MidiGate : public Module 
    {
    public:
        MidiGate();

        void initSignals() override;
        void resetSignals() override;
        void onMidiGate( double gate, uint16_t voice);

    protected:
        EventOutPort eventOutPort_;
    };

    class MidiPitch : public Module 
    {
    public:
        MidiPitch() : Module(kModuleMidiPitch) {}
        MidiPitch(ModuleType type) : Module(type) {}

        //void init(Polyphony* polyphony, double sampleRate) override;
        void initSignals() override;
        void resetSignals() override;

        void initPorts() override;
        void initVoices() override;
        void setParameter( uint16_t paramId, double value, double modulation=0.f, int16_t voice=-1 );
        void onMidiNote( double pitch, double gate, uint16_t voice );
        void onMidiPitchbend(int value);
        //void onMidiPitchbend(uint16_t value1, uint16_t value2);
        
        void processControl() throw() override;

    protected:
        void calcGlide( double freq, uint16_t voice );
        void setGlideTime( double time );

        enum Params { 
            kParamBendRange  = 0,
            kParamGlideTime  = 1,
            kParamGlideAuto  = 2,
        };

        Buffer< double > glideDeltaBuffer_, glideTargetBuffer_, freqBuffer_;

        double* freq_        = nullptr;
        double* glideTarget_ = nullptr;
        double* glideDelta_  = nullptr;

        double glideFrames_  = 0;
        bool glideAuto_      = true;

        double bendFactor_   = 1.0;
        int16_t bendRange_   = 2;

        EventOutPort freqOutPort_;
    };


    class MidiInput : public MidiPitch
    {
    public:
        MidiInput() : MidiPitch(kModuleMidiInput) {}

        void initPorts() override;
        void initSignals() override;
        void resetSignals() override;
        void onMidiNote(double pitch, double gate, uint16_t voice);

    protected:
        EventOutPort gateOutPort_;
    };

}
