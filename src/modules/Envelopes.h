
#pragma once

#include <string>
#include "core/Module.h"

namespace e3 {

    class ADSREnvelope : public Module
    {
    public:
        enum ParamId {
            ParamAudioIn  = 0,         // TODO: what's the use?
            ParamGate     = 1,
            ParamAttack   = 2,
            ParamDecay    = 3,
            ParamSustain  = 4,
            ParamRelease  = 5,
            ParamNumParams
        };

        ADSREnvelope();

        void initVoices() override;
        void updateInports() override;

        void processAudio() throw();
        void processControl() throw() override;

        void setParameter(int paramId, double value, double modulation=0.f, int16_t voice=-1) override;
        void setSentinel(bool sentinel) { sentinel_ = sentinel; }

    protected:  
        void keyOn(double amplitude, uint16_t voice);
        void keyOff(uint16_t voice);

        void setSampleRate(double sampleRate);

        // ranges for all times: 0-1, 1=10sec
        void setAttackRate( double time, int16_t voice=-1 );
        void setDecayRate( double time, int16_t voice=-1 );
        void setSustainLevel( double level, int16_t voice=-1 );
        void setReleaseRate( double time, int16_t voice=-1 );
        
        double calcRate( double time );

        std::string debugLabel_ = "ADSREnvelope";

        Buffer<double> valueBuffer_, targetBuffer_, deltaBuffer_, velocityBuffer_;
        double *value_, *target_, *delta_, *velocity_;

        Buffer<double> attackRateBuffer_, decayRateBuffer_, sustainLevelBuffer_, releaseRateBuffer_;
        double* attackRate_   = nullptr;
        double* decayRate_    = nullptr; 
        double* sustainLevel_ = nullptr;
        double* releaseRate_  = nullptr;

        Buffer<int16_t> stateBuffer_;
        int16_t* state_;

        bool sentinel_ = false;

        AudioInport audioInport_;
        AudioOutport audioOutport_;
        EventInport gateInport_;
        double* audioInportPointer_ = nullptr;

        enum State {
            StateAttack,
            StateDecay,
            StateSustain,
            StateRelease,
            StateDone    
        };
    };
} // namespace e3
