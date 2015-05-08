
#include <e3_Math.h>
#include "core/Polyphony.h"
#include "modules/SineOscillator.h"


namespace e3 {

    Buffer<double> SineOscillator::tableBuffer_;
    double* SineOscillator::table_;
    uint_fast32_t SineOscillator::tableSize_ = 2048;


    SineOscillator::SineOscillator() : Module(
        ModuleTypeSineOscillator,
        "Sine",
        Polyphonic,
        ProcessAudio )
    {
        makeWaveTable();

        addOutport( 0, "Out", &audioOutport_, PortTypeAudio );
        addInport( 0, "Freq", &freqInport_ );
        addInport( 1, "Amp",  &ampInport_ );

        //Parameter paramFreq( ParamFrequency, "Frequency", ControlHidden );
        //parameters_.add( paramFreq );

        //Parameter paramAmp( ParamAmplitude, "Amplitude", ControlHidden );
        //parameters_.add( paramAmp );

        Parameter paramTune( ParamTuning, "Tune", ControlBiSlider );
        paramTune.valueShaper_ ={ -48, 48, 96 };
        parameters_.add( paramTune );

        Parameter paramFinetune( ParamFinetuning, "Finetune", ControlBiSlider );
        paramFinetune.valueShaper_ ={ -1, 1, 200 };
        parameters_.add( paramFinetune );
    }


    ParameterSet& SineOscillator::getDefaultParameters() const
    {
        static ParameterSet set;
        set.clear();

        Parameter paramTune( ParamTuning, id_, "Tune", ControlBiSlider, 0 );
        paramTune.valueShaper_ = { -48, 48, 96 };
        set.add( paramTune );

        Parameter paramFinetune( ParamFinetuning, id_, "Finetune", ControlBiSlider, 0 );
        paramFinetune.valueShaper_ = { -1, 1, 200 };
        set.add( paramFinetune );

        return set;
    }
    
    
    void SineOscillator::initData()
    {
        Module::initData();

        phaseIndex_ = phaseIndexBuffer_.resize( numVoices_, 0. );
        amplitude_  = amplitudeBuffer_.resize( numVoices_, 1 );
        increment_  = incrementBuffer_.resize( numVoices_, 20.43356 );	// 440 Hz
        freq_       = frequencyBuffer_.resize( numVoices_, 440 );

        freqInportPointer_ = freqInport_.getAudioBuffer();
        ampInportPointer_  = ampInport_.getAudioBuffer();
    }


    void SineOscillator::updatePorts()
    {
        bool fm = freqInport_.getNumAudioConnections() > 0;
        bool am = ampInport_.getNumAudioConnections() > 0;

        if (fm == false && am == false) {
            processFunction_ = static_cast<ProcessFunctionPointer>(&SineOscillator::processAudio);
        }
        else if (fm == true && am == false) {
            processFunction_ = static_cast<ProcessFunctionPointer>(&SineOscillator::processAudioFm);
        }
        else if (fm == false && am == true) {
            processFunction_ = static_cast<ProcessFunctionPointer>(&SineOscillator::processAudioAm);
        }
        else if (fm == true && am == true) {
            processFunction_ = static_cast<ProcessFunctionPointer>(&SineOscillator::processAudioFmAm);
        }
    }


    void SineOscillator::setSampleRate( double sampleRate )
    {
        double oldRate = sampleRate_;
        Module::setSampleRate( sampleRate );

        for (uint16_t i = 0; i < numVoices_; i++) {
            increment_[i] = oldRate * increment_[i] / sampleRate_;
        }
    }


    void SineOscillator::setParameter( int paramId, double value, double modulation, int voice )
    {
        voice = std::min<int>( numVoices_ - 1, voice );

        __assume(paramId < 4);
        switch (paramId)
        {
        case ParamFrequency:
            if (voice >= 0)
            {
                double freq;
                if (modulation < 0) {
                    double pitch = FreqToPitch( value );
                    pitch        = (pitch - 60) * modulation + 60;
                    freq         = (double)PitchToFreq( pitch );
                }
                else {
                    freq = (value - 261.62557f) * modulation + 261.62557f;
                }
                freq_[voice] = freq;
                setIncrement( voice );
            }
            break;
        case ParamAmplitude:
            if (voice >= 0)
                amplitude_[voice] = value * modulation;
            break;

        case ParamTuning:     setTuning( value ); break;
        case ParamFinetuning: setFineTuning( value ); break;
        }
    }


    void SineOscillator::setTuning( double paramValue )
    {
        double pitch = (double)(int32_t)paramValue;
        pitch        = std::min<double>( 84, pitch );              // -48: C0 (0Hz), C4 (261.63Hz), 84: C11 (33.488Hz)
        tuning_      = pow( 2, pitch / 12 );

        for (int i = 0; i < numVoices_; i++) {
            setIncrement( i );
        }
    }


    void SineOscillator::setFineTuning( double paramValue )
    {
        //double pitch = std::max<double>( -48, std::min<double>( 84, paramValue ));  // -48: C0 (0Hz), C4 (261.63Hz), 84: C11 (33.488Hz)
        fineTuning_  = pow( 2, paramValue / 12 );

        for (uint16_t i=0; i < numVoices_; i++) {
            setIncrement( i );
        }
    }


    void SineOscillator::makeWaveTable()
    {
        if (tableBuffer_.empty())
        {
            table_      = tableBuffer_.resize( tableSize_ + 1 );
            double temp = (double)(1.0 / tableSize_);

            for (uint16_t i=0; i <= tableSize_; i++) {
                table_[i] = sin( TWO_PI * i * temp );
            }
        }
        ASSERT( table_ );
    }


    void SineOscillator::processAudio() throw()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );
        int_fast32_t v;
        int_fast32_t index, i;
        double tick, frac, pos;

        for (i = 0; i < maxVoices; i++)
        {
            v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
            pos = phaseIndex_[v];

            while (pos < 0.0) pos += tableSize_;         // Check limits of table address
            while (pos >= tableSize_) pos -= tableSize_;

            index = (uint32_t)pos;
            frac  = pos - index;
            tick  = table_[index];
            tick += amplitude_[v] * frac * (table_[index + 1] - tick);

            phaseIndex_[v] = pos + increment_[v];
            audioOutport_.putAudio( tick, v );
        }
    }


    void SineOscillator::processAudioFm()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );
        int_fast32_t v;
        double tick, pos, frac;
        int_fast32_t index, i;

        for (i = 0; i < maxVoices; i++)
        {
            v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
            pos = phaseIndex_[v];

            pos += freqInportPointer_[v];							// FM
            freqInportPointer_[v] = 0.f;

            while (pos < 0.0) pos += tableSize_;         // Check limits of table address
            while (pos >= tableSize_) pos -= tableSize_;

            index = (int_fast32_t)pos;
            frac  = pos - index;
            tick  = table_[index];
            tick += amplitude_[v] * frac * (table_[index + 1] - tick);

            phaseIndex_[v] = pos + increment_[v];
            audioOutport_.putAudio( tick, v );
        }
    }


    void SineOscillator::processAudioAm()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );
        int_fast32_t v;
        double tick, pos, frac;
        int_fast32_t index, i;

        for (i = 0; i < maxVoices; i++)
        {
            v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
            pos = phaseIndex_[v];

            while (pos < 0) pos += tableSize_;         // Check limits of table address
            while (pos >= tableSize_) pos -= tableSize_;

            index = (int_fast32_t)pos;
            frac  = pos - index;
            tick  = table_[index];
            tick += frac * (table_[index + 1] - tick);
            tick *= amplitude_[v] + ampInportPointer_[v];
            ampInportPointer_[v] = 0;

            phaseIndex_[v] = pos + increment_[v];                     // table position, which can be negative.
            audioOutport_.putAudio( tick, v );
        }
    }


    void SineOscillator::processAudioFmAm()
    {
        int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );
        double tick, pos, frac;
        int_fast32_t index, i;
        int_fast32_t v;

        for (i = 0; i < maxVoices; i++)
        {
            v = mono_ ? 0 : polyphony_->soundingVoices_[i];
            pos = phaseIndex_[v];

            pos += freqInportPointer_[v];                         // FM
            freqInportPointer_[v] = 0.f;

            while (pos < 0.0) pos += tableSize_;         // Check limits of table address
            while (pos >= tableSize_) pos -= tableSize_;

            index = (int_fast32_t)pos;
            frac = pos - index;
            tick = table_[index];
            tick += frac * (table_[index + 1] - tick);
            tick *= amplitude_[v] + ampInportPointer_[v];
            ampInportPointer_[v] = 0.f;

            phaseIndex_[v] = pos + increment_[v];
            audioOutport_.putAudio( tick, v );
        }
    }
} // namespace e3
