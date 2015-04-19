
#pragma once

#include "core/Module.h"

namespace e3 {

    class ADSREnv : public Module
    {
    public:
        enum ParamId {
            kParamGate     = 1,
            kParamAttack   = 2,
            kParamDecay    = 3,
            kParamSustain  = 4,
            kParamRelease  = 5,
            kParamNumParams
        };

        ADSREnv() : Module(kModuleAdsrEnv) {}

        void initPorts() override;
        void initProcess() override;
        void initVoices() override;
        void updateInPorts() override;

        void processAudio() throw();
        void processControl() throw() override;

        void setParameter( uint16_t paramId, double value, double modulation=0.f, int16_t voice=-1 );
        void setSentinel( bool sentinel ) { sentinel_ = sentinel; }

    protected:  
        void keyOn( double amplitude, uint16_t voice );
        void keyOff( uint16_t voice );

        void setSampleRate(double sampleRate);

        // ranges for all times: 0-1, 1=10sec
        void setAttackRate( double time, int16_t voice=-1 );
        void setDecayRate( double time, int16_t voice=-1 );
        void setSustainLevel( double level, int16_t voice=-1 );
        void setReleaseRate( double time, int16_t voice=-1 );
        
        double calcRate( double time );

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

        AudioInPort audioInPort_;
        AudioOutPort audioOutPort_;
        double* audioInPortPointer_ = nullptr;

        enum State {
            kStateAttack,
            kStateDecay,
            kStateSustain,
            kStateRelease,
            kStateDone    
        };
    };
} // namespace e3
