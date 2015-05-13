
#pragma once

#include <string>
#include "core/Module.h"
#include "core/Polyphony.h"


namespace e3 {

    class AdsrEnvelope : public Module
    {
    public:
        AdsrEnvelope();

        ParameterSet& getDefaultParameters() const override;
        void initData() override;

        void processAudio() throw();

        void setParameter( int paramId, double value, double modulation=0.f, int voice=-1 ) override;
        void makeOutputEnvelope( bool value ) { isOutputEnvelope_ = value; }

        enum ParamId {                 
            ParamAudioIn  = 0,         // TODO: what's the use?
            ParamGate     = 1,
            ParamAttack   = 2,
            ParamDecay    = 3,
            ParamSustain  = 4,
            ParamRelease  = 5,
            ParamNumParams
        };

    protected:
        void keyOn( double amplitude, int voice );
        void keyOff( int voice );

        void setSampleRate( double sampleRate );

        void calculateAttackTime();
        void calculateDecayTime();
        void calculateReleaseTime();


        std::string debugLabel_ = "AdsrEnvelope";

        double attackTime_    = 0;
        double attackCoeff_   = 0;
        double attackOffset_  = 0;
        double attackTCO_     = 0.99999;

        double decayTime_     = 0;
        double decayCoeff_    = 0;
        double decayOffset_   = 0;
        double decayTCO_      = exp( -11.05 );

        double sustainLevel_  = 1;

        double releaseTime_   = 0;
        double releaseCoeff_  = 0;
        double releaseOffset_ = 0;
        double releaseTCO_    = decayTCO_;

        bool isOutputEnvelope_ = false;

        Buffer<double> valueBuffer_, velocityBuffer_;
        double *value_, *velocity_;

        Buffer<int> stateBuffer_;
        int* state_;

        Inport audioInport_;
        Outport audioOutport_;
        Inport gateInport_;
        double* audioInportPointer_ = nullptr;

        enum State {
            StateAttack,
            StateDecay,
            StateSustain,
            StateRelease,
            StateDone
        };
    };

    inline void AdsrEnvelope::processAudio() throw()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );

        for (int_fast32_t i = 0; i < maxVoices; i++)
        {
            int_fast32_t v         = polyphony_->soundingVoices_[i];
            double input           = audioInportPointer_[v];
            audioInportPointer_[v] = 0;

            __assume(state_[v] <= 4);
            switch (state_[v])
            {
            case StateAttack:
            {
                value_[v] = attackOffset_ + value_[v] * attackCoeff_;
                if (value_[v] >= 1.0)
                {
                    value_[v] = 1.0;
                    state_[v]  = StateDecay;
                }
                break;
            }
            case StateDecay:
            {
                value_[v] = decayOffset_ + value_[v] * decayCoeff_;

                if (value_[v] <= sustainLevel_)
                {
                    value_[v] = sustainLevel_;
                    state_[v] = StateSustain;
                }
                break;
            }
            case StateSustain:
            {
                value_[v] = sustainLevel_;
                break;
            }
            case StateRelease:
            {
                value_[v] = releaseOffset_ + value_[v] * releaseCoeff_;

                if (value_[v] <= 0.0)
                {
                    value_[v] = 0.0;
                    state_[v] = StateDone;

                    if (isOutputEnvelope_) {
                        polyphony_->endVoice( v );
                    }
                }
                break;
            }
            }

            audioOutport_.putAudio( input * value_[v] * velocity_[v], v );
        }
    }


} // namespace e3
