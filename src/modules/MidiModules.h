
#pragma once

#include <string>
#include "core/Module.h"


namespace e3 {
    class Polyphony;

    //------------------------------------------------------------------
    // class MidiGate
    //------------------------------------------------------------------

    class MidiGate : public Module 
    {
    public:
        MidiGate();

        void connectSignals() override;
        void disconnectSignals() override;
        void onMidiGate( double gate, int voice);

        std::string debugLabel_ = "MidiGate";

    protected:
        Outport gateOutport_;
    };


    //------------------------------------------------------------------
    // class MidiFrequency
    //------------------------------------------------------------------

    class MidiFrequency : public Module
    {
    public:
        MidiFrequency();
        MidiFrequency(
            ModuleType moduleType,
            const std::string& label,
            VoicingType voicingType,
            ProcessingType processingType);

        ParameterSet& getDefaultParameters() const override;
        void connectSignals() override;
        void disconnectSignals() override;

        void initData() override;
        void setParameter(int paramId, double value, double modulation=0.f, int voice=-1) override;
        void onMidiNote( double pitch, double gate, int voice );
        void onMidiPitchbend(int value);
        
        void processControl() throw() override;

        std::string debugLabel_ = "MidiFrequency";

    protected:
        void createParameters();
        void calcGlide(double freq, int voice);
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
        int bendRange_       = 2;

        Outport freqOutport_;
    };


    //------------------------------------------------------------------
    // class MidiInput
    //------------------------------------------------------------------

    class MidiInput : public MidiFrequency
    {
    public:
        MidiInput();

        void connectSignals() override;
        void disconnectSignals() override;
        void onMidiNote(double pitch, double gate, int voice);

        std::string debugLabel_ = "MidiInput";

    protected:
        Outport gateOutport_;
    };

}
