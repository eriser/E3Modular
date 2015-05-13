
#include <cmath>
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
        paramAttack.valueShaper_ = { 0, 10, 100, 6 };
        paramAttack.unit_ = "sec";
        paramAttack.numberFormat_ = NumberFloat;

        const Parameter& paramDecay = set.addModuleParameter( ParamDecay, id_, "Decay", ControlSlider );
        paramDecay.valueShaper_ = { 0, 10, 100, 6 };
        paramDecay.unit_ = "sec";
        paramDecay.numberFormat_ = NumberFloat;

        const Parameter& paramSustain = set.addModuleParameter( ParamSustain, id_, "Sustain", ControlSlider, 0.75 );
        paramSustain.valueShaper_ = { 0, 1, 100, 6 };
        paramSustain.unit_ = "db";
        paramSustain.numberFormat_ = NumberDecibel;

        const Parameter& paramRelease = set.addModuleParameter( ParamRelease, id_, "Release", ControlSlider );
        paramRelease.valueShaper_ =  { 0, 20, 100, 6 };
        paramRelease.unit_ = "sec";
        paramRelease.numberFormat_ = NumberFloat;

        return set;
    }
    
    
    void AdsrEnvelope::initData()
    {
        Module::initData();

        value_    = valueBuffer_.resize( numVoices_, 0 );
        state_    = stateBuffer_.resize( numVoices_, 0 );
        velocity_ = velocityBuffer_.resize( numVoices_, 0 );

        audioInportPointer_ = audioInport_.getAudioBuffer();
    }


    void AdsrEnvelope::setSampleRate( double sampleRate )
    {
        Module::setSampleRate( sampleRate );

        calculateAttackTime();
        calculateDecayTime();
        calculateReleaseTime();
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
        case ParamAttack:  attackTime_   = value; calculateAttackTime(); break;
        case ParamDecay:   decayTime_    = value; calculateDecayTime(); break;
        case ParamSustain: sustainLevel_ = value; calculateDecayTime(); break;
        case ParamRelease: releaseTime_  = value; calculateReleaseTime(); break;
        }
    }


    void AdsrEnvelope::keyOn( double amplitude, int voice )
    {
        value_[voice]    = 0;
        state_[voice]    = StateAttack;
        velocity_[voice] = amplitude;
    }


    void AdsrEnvelope::keyOff( int voice )
    {
        state_[voice] = (value_[voice] > 0) ? StateRelease : StateDone;
    }


    void AdsrEnvelope::calculateAttackTime()
    {
        double numSamples = sampleRate_ * attackTime_;

        attackCoeff_      = exp( -log( (1.0 + attackTCO_) / attackTCO_ ) / numSamples );
        attackOffset_     = (1.0 + attackTCO_) * (1.0 - attackCoeff_);
    }


    void AdsrEnvelope::calculateDecayTime()
    {
        double numSamples = sampleRate_ * decayTime_;

        decayCoeff_       = exp( -log( (1.0 + decayTCO_) / decayTCO_ ) / numSamples );
        decayOffset_      = (sustainLevel_ - decayTCO_) * (1.0 - decayCoeff_);
    }


    void AdsrEnvelope::calculateReleaseTime()
    {
        double numSamples = sampleRate_ * releaseTime_;

        releaseCoeff_     = exp( -log( (1.0 + releaseTCO_) / releaseTCO_ ) / numSamples );
        releaseOffset_    = -releaseTCO_ * (1.0 - releaseCoeff_);
    }

} // namespace e3
