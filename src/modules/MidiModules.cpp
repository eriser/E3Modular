
#include <e3_Math.h>
#include "core/Polyphony.h"
#include "modules/MidiModules.h"

#include <e3_Trace.h>

namespace e3 {

    //--------------------------------------------------------
    // class MidiGate
    //--------------------------------------------------------


    MidiGate::MidiGate() : Module(
        ModuleTypeMidiGate,
        "Midi Gate",
        Polyphonic,
        ProcessEvent )
    {
        addOutport( 0, "Gate", &gateOutport_, PortTypeEvent );
    }

    
    void MidiGate::connectSignals()
    {
        if (polyphony_) {
            polyphony_->midiGateSignal.Connect( this, &MidiGate::onMidiGate );
        }
    }


    void MidiGate::disconnectSignals()
    {
        if (polyphony_) {
            if (polyphony_->midiGateSignal.Empty() == false)
                polyphony_->midiGateSignal.Disconnect( this, &MidiGate::onMidiGate );
        }
    }


    void MidiGate::onMidiGate( double gate, int voice )
    {
        if (gate > -1) {
            gateOutport_.putEvent( gate, voice );
        }
    }


    //--------------------------------------------------------
    // class MidiFrequency
    //--------------------------------------------------------

    MidiFrequency::MidiFrequency() : Module(
        ModuleTypeMidiFrequency,
        "Midi Frequency",
        Polyphonic,
        (ProcessingType)(ProcessEvent | ProcessControl) )
    {
        addOutport( 0, "Freq", &freqOutport_, PortTypeEvent );
        createParameters();
    }


    MidiFrequency::MidiFrequency(
        ModuleType moduleType,
        const std::string& label,
        VoicingType voicingType,
        ProcessingType processingType )
        :
        Module( moduleType, label, voicingType, processingType )
    {
        createParameters();
    }


    ParameterSet& MidiFrequency::getDefaultParameters() const
    {
        static ParameterSet set;
        set.clear();

        Parameter paramBend( ParamBendRange, id_, "BendRange", ControlBiSlider, 2 );
        paramBend.valueShaper_ ={ -48, 48, 96 };
        set.add( paramBend );

        Parameter paramTime( ParamGlideTime, id_, "Portamento Time", ControlSlider, 0 );
        paramTime.valueShaper_ ={ 0, 2000 };
        paramTime.unit_ = "msec";
        paramTime.numberFormat_ = NumberFloat;
        set.add( paramTime );

        Parameter paramAuto( ParamGlideAuto, id_, "Portamento Auto", ControlCheckbox, 0 );
        paramBend.valueShaper_ ={ 0, 1 };
        set.add( paramAuto );

        return set;
    }
    
    
    void MidiFrequency::createParameters()
    {
        Parameter paramBend( ParamBendRange, "BendRange", ControlBiSlider, 2 );
        paramBend.valueShaper_ ={ -48, 48, 96 };
        parameters_.add( paramBend );

        Parameter paramTime( ParamGlideTime, "Portamento Time", ControlSlider, 0 );
        paramTime.valueShaper_ ={ 0, 2000 };
        paramTime.unit_ = "msec";
        paramTime.numberFormat_ = NumberFloat;
        parameters_.add( paramTime );

        Parameter paramAuto( ParamGlideAuto, "Portamento Auto", ControlCheckbox, 0 );
        paramBend.valueShaper_ ={ 0, 1 };
        parameters_.add( paramAuto );
    }


    void MidiFrequency::connectSignals()
    {
        if (polyphony_)
        {
            polyphony_->midiNoteSignal.Connect( this, &MidiFrequency::onMidiNote );
            polyphony_->midiPitchbendSignal.Connect( this, &MidiFrequency::onMidiPitchbend );
        }
    }


    void MidiFrequency::disconnectSignals()
    {
        if (polyphony_) {
            if (polyphony_->midiNoteSignal.Empty() == false)
                polyphony_->midiNoteSignal.Disconnect( this, &MidiFrequency::onMidiNote );
            if (polyphony_->midiPitchSignal.Empty() == false)
                polyphony_->midiPitchbendSignal.Disconnect( this, &MidiFrequency::onMidiPitchbend );
        }
    }


    void MidiFrequency::initData()
    {
        Module::initData();

        glideDelta_   = glideDeltaBuffer_.resize( numVoices_ );
        glideTarget_  = glideTargetBuffer_.resize( numVoices_ );
        freq_         = freqBuffer_.resize( numVoices_ );
    }


    void MidiFrequency::setParameter( int paramId, double value, double, int )
    {
        switch (paramId)
        {
        case ParamBendRange: bendRange_ = (int)value;           break;
        case ParamGlideTime: setGlideTime( value );             break;
        case ParamGlideAuto: glideAuto_ = value ? true : false; break;
        }
    }


    void MidiFrequency::onMidiNote( double pitch, double gate, int voice )
    {
        double freq  = PitchToFreq( pitch );
        freq_[voice] = freq;

        if (gate != 0)
        {
            calcGlide( freq, voice );
            freqOutport_.putEvent( freq_[voice] * bendFactor_, voice );
        }
    }


    void MidiFrequency::onMidiPitchbend( int value )
    {
        double cents = ((value - 0x2000) / (double)0x2000) * bendRange_ * 100;
        bendFactor_  = pow( 2.f, cents / 1200.f );

        int maxVoices = std::min<int>( numVoices_, polyphony_->numSounding_ );
        for (int i = 0; i < maxVoices; i++)
        {
            int v = polyphony_->soundingVoices_[i];
            freqOutport_.putEvent( freq_[v] * bendFactor_, v );
        }
    }


    void MidiFrequency::processControl() throw()
    {
        if (glideFrames_ == 0)
            return;

        int maxVoices = std::min<int>( numVoices_, polyphony_->numSounding_ );
        for (int i = 0; i < maxVoices; i++)
        {
            int v = mono_ ? 0 : polyphony_->soundingVoices_[i];

            if (glideDelta_[v] > 0 && freq_[v] >= glideTarget_[v] || glideDelta_[v] < 0 && freq_[v] <= glideTarget_[v]) {
                glideDelta_[v] = 0.0f;
            }
            else {
                freq_[v] += glideDelta_[v];
                freqOutport_.putEvent( freq_[v] * bendFactor_, v );
            }
        }
    }


    void MidiFrequency::calcGlide( double freq, int voice )
    {
        if (glideFrames_ > 0)
        {
            glideTarget_[voice] = freq;
            double prevPitch    = polyphony_->getPreviousPitch( voice );

            if (prevPitch == -1 || glideAuto_ && polyphony_->numActive_ <= polyphony_->numUnison_) {
                glideDeltaBuffer_.set( 0 );
            }
            else {
                double prevFreq     = PitchToFreq( prevPitch );
                double diff         = freq - prevFreq;
                glideDelta_[voice] = diff / glideFrames_;
                freq_[voice]       = prevFreq;
            }
        }
    }


    void MidiFrequency::setGlideTime( double time )
    {
        double msPerFrame = (1 / (double)INITIAL_CONTROLRATE) * 1000;
        glideFrames_     = time / msPerFrame;
    }


    //--------------------------------------------------------
    // class MidiInput
    //--------------------------------------------------------

    MidiInput::MidiInput() : MidiFrequency(
        ModuleTypeMidiInput,
        "Midi Input",
        Polyphonic,
        (ProcessingType)(ProcessEvent | ProcessControl) )
    {
        addOutport( 0, "Freq", &freqOutport_, PortTypeEvent );
        addOutport( 1, "Gate", &gateOutport_, PortTypeEvent );
    }


    void MidiInput::connectSignals()
    {
        if (polyphony_) {
            polyphony_->midiNoteSignal.Connect( this, &MidiInput::onMidiNote );
            polyphony_->midiPitchbendSignal.Connect( this, &MidiFrequency::onMidiPitchbend );
        }
    }


    void MidiInput::disconnectSignals()
    {
        if (polyphony_) {
            if (polyphony_->midiNoteSignal.Empty() == false)
                polyphony_->midiNoteSignal.Disconnect( this, &MidiInput::onMidiNote );
            if (polyphony_->midiPitchbendSignal.Empty() == false)
                polyphony_->midiPitchbendSignal.Disconnect( this, &MidiFrequency::onMidiPitchbend );
        }
    }


    void MidiInput::onMidiNote( double pitch, double gate, int voice )
    {
        MidiFrequency::onMidiNote( pitch, gate, voice );

        if (gate > -1) {
            gateOutport_.putEvent( gate, voice );
        }
    }
}
