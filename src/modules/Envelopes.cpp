
#include "core/Polyphony.h"
#include "modules/Envelopes.h"


namespace e3 {


    ADSREnv::ADSREnv() : Module(
        kModuleAdsrEnv,
        "ADSR",
        kPolyphonic,
        (ProcessingType)(kProcessAudio | kProcessControl))
    {
        addInport(0, "In", &audioInport_);
        addInport(1, "Gate", &gateInport_);
        addOutport(0, "Out", &audioOutport_);

        processFunction_ = static_cast< ProcessFunctionPointer >(&ADSREnv::processAudio);
        
        Parameter paramIn(kParamIn, "In", kControlHidden); // TODO: what is this good for?
        parameters_.add(paramIn);

        Parameter paramGate(kParamGate, "Gate", kControlHidden);
        parameters_.add(paramGate);

        Parameter paramAttack(kParamAttack, "Attack", kControlSlider, 0.1);
        paramAttack.valueShaper_ = { 0, 1.2, 96, 6 };
        paramAttack.unit_ = "sec";
        paramAttack.numberFormat_ = kNumberFloat;
        parameters_.add(paramAttack);

        Parameter paramDecay(kParamDecay, "Decay", kControlSlider);
        paramDecay.valueShaper_ = { 0, 1, 100, 6 };
        paramDecay.unit_ = "sec";
        paramDecay.numberFormat_ = kNumberFloat;
        parameters_.add(paramDecay);

        Parameter paramSustain(kParamSustain, "Sustain", kControlSlider, 0.75);
        paramSustain.valueShaper_ = { 0, 1, 100, 6 };
        paramSustain.unit_ = "db";
        paramSustain.numberFormat_ = kNumberDecibel;
        parameters_.add(paramSustain);

        Parameter paramRelease(kParamRelease, "Release", kControlSlider);
        paramRelease.valueShaper_ = { 0, 1, 100, 6 };
        paramRelease.unit_ = "sec";
        paramRelease.numberFormat_ = kNumberFloat;
        parameters_.add(paramRelease);
    }


    void ADSREnv::initVoices()
    {
        value_        = valueBuffer_.resize( numVoices_, 0 );
        target_       = targetBuffer_.resize( numVoices_, 0 );
        delta_        = deltaBuffer_.resize( numVoices_, 0 );
        state_        = stateBuffer_.resize( numVoices_, 0 );
        velocity_     = velocityBuffer_.resize( numVoices_, 0 );
        
        attackRate_   = attackRateBuffer_.resize( numVoices_, 0 );
        decayRate_    = decayRateBuffer_.resize(numVoices_, 0);
        sustainLevel_ = sustainLevelBuffer_.resize(numVoices_, 0);
        releaseRate_  = releaseRateBuffer_.resize(numVoices_, 0);
    }


    void ADSREnv::updateInports()
    {
        audioInport_.setNumVoices(numVoices_);
        audioInportPointer_ = audioInport_.getBuffer();
    }


    void ADSREnv::setSampleRate( double sampleRate )
    {
        double oldRate = sampleRate_;
        Module::setSampleRate(sampleRate);

        for( uint16_t i=0; i<numVoices_; i++ ) {
            attackRate_[i]  = oldRate * attackRate_[i]  / sampleRate_;
            decayRate_[i]   = oldRate * decayRate_[i]   / sampleRate_;
            releaseRate_[i] = oldRate * releaseRate_[i] / sampleRate_;
        }
    }


    void ADSREnv::setParameter( uint16_t paramId, double value, double modulation, int16_t voice )
    {
        voice = std::min<int16_t>( numVoices_-1, voice );

        switch( paramId )
        {
        case kParamGate:
            if( voice > -1 )
            {
                double velo = value * modulation + ( 1 - modulation );
                value > 0 ? keyOn( velo, voice ) : keyOff( voice );
                break;
            }
        case kParamAttack:  setAttackRate( value, voice ); break;
        case kParamDecay:   setDecayRate( value, voice ); break;
        case kParamSustain: setSustainLevel( value, voice ); break;
        case kParamRelease: setReleaseRate( value, voice ); break;
        }
    }


    void ADSREnv::setAttackRate( double time, int16_t voice )
    {
        double rate = calcRate( time * 0.71 );

        if( voice > -1 ) {
            attackRate_[voice] = rate;
        }
        else for( uint16_t i=0; i<numVoices_; i++ ) {	
            attackRate_[i] = rate;

            if( target_[i] == 2 ) {
                delta_[i] = attackRate_[i];
            }
        }
    }


    void ADSREnv::setDecayRate( double time, int16_t voice )
    {
        double rate = calcRate( time * 0.089 );

        if( voice > -1 ) {
            decayRate_[voice] = rate;
        }
        else for( uint16_t i = 0; i < numVoices_; i++ ) {	
            decayRate_[i] = rate;
        }
    }


    void ADSREnv::setSustainLevel( double level, int16_t voice )
    {
        if( voice > -1 ) {
            sustainLevel_[voice] = level;
        }
        else for( uint16_t i=0; i<numVoices_; i++ ) {
            sustainLevel_[i] = level;
        }
    }


    void ADSREnv::setReleaseRate( double time, int16_t voice )
    {
        double rate = calcRate( time*0.106f );

        if( voice > -1 ) {
            releaseRate_[voice] = rate;
        }
        else for( uint16_t i = 0; i < numVoices_; i++ ) {	
            releaseRate_[i] = rate;

            if( target_[i] == 0 ) {
                delta_[i] = releaseRate_[i];
            }
        }
    }


    void ADSREnv::keyOn( double amplitude, uint16_t voice )
    {
        value_[voice]    = 0.0f;
        target_[voice]   = 1.f;
        state_[voice]    = kStateAttack;

        delta_[voice]    = attackRate_[voice];
        velocity_[voice] = amplitude;
    }


    void ADSREnv::keyOff( uint16_t voice )
    {
        target_[voice] = 0.0f;
        state_[voice]  = kStateRelease;
        delta_[voice]  = releaseRate_[voice];
    }


    double ADSREnv::calcRate( double time )
    {
        time        = std::max<double>( 0.0001, time );
        double rate = 1 / ((double)sampleRate_ * time * 2);

        return rate;
    }


    void ADSREnv::processAudio() throw()
    {
        uint32_t maxVoices = std::min<uint32_t>( numVoices_, polyphony_->numSounding_ );
        
        for( uint32_t i = 0; i < maxVoices; i++ )
        {
            uint16_t v             = polyphony_->soundingVoices_[i];
            double input           = audioInportPointer_[v];
            audioInportPointer_[v] = 0;

            value_[v] += delta_[v] * ( target_[v] - value_[v] );

            audioOutport_.putValue( input * value_[v] * velocity_[v], v );
        }
    }


    void ADSREnv::processControl() throw()
    {
        uint16_t maxVoices = std::min<uint16_t>( numVoices_, polyphony_->numSounding_ );

        for( uint16_t i = 0; i < maxVoices; i++ )
        {
            uint16_t v = polyphony_->soundingVoices_[ i ];

            __assume( state_[v] <= 4 );
            switch( state_[v] ) 
            {
            case kStateAttack:
                if( value_[v] >= target_[v] ) 
                {
                    delta_[v]  = decayRate_[v];
                    target_[v] = sustainLevel_[v];
                    state_[v]  = kStateDecay;
                }
                break;
            case kStateDecay:
                if( value_[v] <= sustainLevel_[v] ) 
                {
                    delta_[v] = 0.f;
                    state_[v] = kStateSustain;
                }
                break;
            case kStateRelease:
                if( value_[v] <= 0.0001f ) 
                {
                    state_[v] = kStateDone;

                    if( sentinel_ ) {
                        polyphony_->endVoice( v );
                    }
                }
                break;
            }
        }
    }


}
