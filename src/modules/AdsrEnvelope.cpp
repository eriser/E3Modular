
#include "core/Polyphony.h"
#include "modules/AdsrEnvelope.h"


namespace e3 {


    AdsrEnvelope::AdsrEnvelope() : Module(
        ModuleTypeAdsrEnvelope,
        "ADSR",
        Polyphonic,
        (ProcessingType)(ProcessAudio | ProcessControl) )
    {
        addInport( 0, "In", &audioInport_ );
        addInport( 1, "Gate", &gateInport_ );
        addOutport( 0, "Out", &audioOutport_, PortTypeAudio );

        processFunction_ = static_cast<ProcessFunctionPointer>(&AdsrEnvelope::processAudio);
    }


    ParameterSet& AdsrEnvelope::getDefaultParameters() const
    {
        static ParameterSet set;
        set.clear();

        const Parameter& paramAttack = set.addModuleParameter( ParamAttack, id_, "Attack", ControlSlider, 0.1 );
        paramAttack.valueShaper_ ={ 0, 1.2, 96, 6 };
        paramAttack.unit_ = "sec";
        paramAttack.numberFormat_ = NumberFloat;

        const Parameter& paramDecay = set.addModuleParameter( ParamDecay, id_, "Decay", ControlSlider );
        paramDecay.valueShaper_ ={ 0, 1, 100, 6 };
        paramDecay.unit_ = "sec";
        paramDecay.numberFormat_ = NumberFloat;

        const Parameter& paramSustain = set.addModuleParameter( ParamSustain, id_, "Sustain", ControlSlider, 0.75 );
        paramSustain.valueShaper_ ={ 0, 1, 100, 6 };
        paramSustain.unit_ = "db";
        paramSustain.numberFormat_ = NumberDecibel;

        const Parameter& paramRelease = set.addModuleParameter( ParamRelease, id_, "Release", ControlSlider );
        paramRelease.valueShaper_ ={ 0, 1, 100, 6 };
        paramRelease.unit_ = "sec";
        paramRelease.numberFormat_ = NumberFloat;

        return set;
    }
    
    
    void AdsrEnvelope::initData()
    {
        Module::initData();

        value_        = valueBuffer_.resize( numVoices_, 0 );
        target_       = targetBuffer_.resize( numVoices_, 0 );
        delta_        = deltaBuffer_.resize( numVoices_, 0 );
        state_        = stateBuffer_.resize( numVoices_, 0 );
        velocity_     = velocityBuffer_.resize( numVoices_, 0 );

        attackRate_   = attackRateBuffer_.resize( numVoices_, 0 );
        decayRate_    = decayRateBuffer_.resize( numVoices_, 0 );
        sustainLevel_ = sustainLevelBuffer_.resize( numVoices_, 0 );
        releaseRate_  = releaseRateBuffer_.resize( numVoices_, 0 );

        audioInportPointer_ = audioInport_.getAudioBuffer();
    }


    void AdsrEnvelope::setSampleRate( double sampleRate )
    {
        double oldRate = sampleRate_;
        Module::setSampleRate( sampleRate );

        for (uint16_t i=0; i < numVoices_; i++) {
            attackRate_[i]  = oldRate * attackRate_[i] / sampleRate_;
            decayRate_[i]   = oldRate * decayRate_[i] / sampleRate_;
            releaseRate_[i] = oldRate * releaseRate_[i] / sampleRate_;
        }
    }


    void AdsrEnvelope::setParameter( int paramId, double value, double modulation, int voice )
    {
        voice = std::min<int>( numVoices_ - 1, voice );

        switch (paramId)
        {
        case ParamGate:
            if (voice > -1)
            {
                double velo = value * modulation + (1 - modulation);
                value > 0 ? keyOn( velo, voice ) : keyOff( voice );
                break;
            }
        case ParamAttack:  setAttackRate( value, voice ); break;
        case ParamDecay:   setDecayRate( value, voice ); break;
        case ParamSustain: setSustainLevel( value, voice ); break;
        case ParamRelease: setReleaseRate( value, voice ); break;
        }
    }


    void AdsrEnvelope::setAttackRate( double time, int voice )
    {
        double rate = calcRate( time * 0.71 );

        if (voice > -1) {
            attackRate_[voice] = rate;
        }
        else for (int i = 0; i < numVoices_; i++) {
            attackRate_[i] = rate;

            if (target_[i] == 2) {
                delta_[i] = attackRate_[i];
            }
        }
    }


    void AdsrEnvelope::setDecayRate( double time, int voice )
    {
        double rate = calcRate( time * 0.089 );

        if (voice > -1) {
            decayRate_[voice] = rate;
        }
        else for (int i = 0; i < numVoices_; i++) {
            decayRate_[i] = rate;
        }
    }


    void AdsrEnvelope::setSustainLevel( double level, int voice )
    {
        if (voice > -1) {
            sustainLevel_[voice] = level;
        }
        else for (int i=0; i < numVoices_; i++) {
            sustainLevel_[i] = level;
        }
    }


    void AdsrEnvelope::setReleaseRate( double time, int voice )
    {
        double rate = calcRate( time*0.106f );

        if (voice > -1) {
            releaseRate_[voice] = rate;
        }
        else for (int i = 0; i < numVoices_; i++) {
            releaseRate_[i] = rate;

            if (target_[i] == 0) {
                delta_[i] = releaseRate_[i];
            }
        }
    }


    void AdsrEnvelope::keyOn( double amplitude, int voice )
    {
        value_[voice]    = 0.0f;
        target_[voice]   = 1.f;
        state_[voice]    = StateAttack;

        delta_[voice]    = attackRate_[voice];
        velocity_[voice] = amplitude;
    }


    void AdsrEnvelope::keyOff( int voice )
    {
        target_[voice] = 0.0f;
        state_[voice]  = StateRelease;
        delta_[voice]  = releaseRate_[voice];
    }


    double AdsrEnvelope::calcRate( double time )
    {
        time        = std::max<double>( 0.0001, time );
        double rate = 1 / ((double)sampleRate_ * time * 2);

        return rate;
    }


    void AdsrEnvelope::processAudio() throw()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );

        for (int_fast32_t i = 0; i < maxVoices; i++)
        {
            int_fast32_t v         = polyphony_->soundingVoices_[i];
            double input           = audioInportPointer_[v];
            audioInportPointer_[v] = 0;

            value_[v] += delta_[v] * (target_[v] - value_[v]);

            audioOutport_.putAudio( input * value_[v] * velocity_[v], v );
        }
    }


    void AdsrEnvelope::processControl() throw()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );

        for (int_fast32_t i = 0; i < maxVoices; i++)
        {
            int_fast32_t v = polyphony_->soundingVoices_[i];

            __assume(state_[v] <= 4);
            switch (state_[v])
            {
            case StateAttack:
                if (value_[v] >= target_[v])
                {
                    delta_[v]  = decayRate_[v];
                    target_[v] = sustainLevel_[v];
                    state_[v]  = StateDecay;
                }
                break;
            case StateDecay:
                if (value_[v] <= sustainLevel_[v])
                {
                    delta_[v] = 0.f;
                    state_[v] = StateSustain;
                }
                break;
            case StateRelease:
                if (value_[v] <= 0.0001f)
                {
                    state_[v] = StateDone;

                    if (sentinel_) {
                        polyphony_->endVoice( v );
                    }
                }
                break;
            }
        }
    }

} // namespace e3
