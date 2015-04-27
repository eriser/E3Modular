
#include <e3_Math.h>
#include "core/Polyphony.h"
#include "modules/MidiModules.h"

#include <e3_Trace.h>

namespace e3 {

    //--------------------------------------------------------
    // class MidiGate
    //--------------------------------------------------------


    MidiGate::MidiGate() : Module(
        kModuleMidiGate,
        "Midi Gate",
        kPolyphonic,
        kProcessEvent)
    {
        addOutport(0, "Gate", &gateOutport_);
    }


    void MidiGate::initSignals()
    {
        polyphony_->midiGateSignal.Connect(this, &MidiGate::onMidiGate);
    }


    void MidiGate::resetSignals()
    {
        polyphony_->midiGateSignal.Disconnect(this, &MidiGate::onMidiGate);
    }


    void MidiGate::onMidiGate(double gate, uint16_t voice)
    {
        if (gate > -1) {
            gateOutport_.putEvent(gate, voice);
        }
    }


    //--------------------------------------------------------
    // class MidiPitch
    //--------------------------------------------------------

    MidiPitch::MidiPitch() : Module(
        kModuleMidiPitch,
        "Midi Pitch",
        kPolyphonic,
        (ProcessingType)(kProcessEvent | kProcessControl))
    {
        addOutport(0, "Pitch", &pitchOutport_);
        createParameters();
    }


    MidiPitch::MidiPitch(
        ModuleType moduleType, 
        const std::string& label,
        VoicingType voicingType,
        ProcessingType processingType)
        :
        Module(moduleType, label, voicingType, processingType)
    {
        createParameters();
    }


    void MidiPitch::createParameters()
    {
        Parameter paramBend(kParamBendRange, "BendRange", kControlBiSlider, 2);
        paramBend.valueShaper_ = { -48, 48, 96 };
        parameters_.add(paramBend);

        Parameter paramTime(kParamGlideTime, "Portamento Time", kControlSlider, 0);
        paramTime.valueShaper_ = { 0, 2000 };
        paramTime.unit_ = "msec";
        paramTime.numberFormat_ = kNumberFloat;
        parameters_.add(paramTime);

        Parameter paramAuto(kParamGlideAuto, "Portamento Auto", kControlCheckbox, 0);
        paramBend.valueShaper_ = { 0, 1 };
        parameters_.add(paramAuto);
    }


    void MidiPitch::initSignals()
    {
        polyphony_->midiNoteSignal.Connect(this, &MidiPitch::onMidiNote);
        polyphony_->midiPitchbendSignal.Connect(this, &MidiPitch::onMidiPitchbend);
    }


    void MidiPitch::resetSignals()
    {
        polyphony_->midiNoteSignal.Disconnect(this, &MidiPitch::onMidiNote);
        polyphony_->midiPitchbendSignal.Disconnect(this, &MidiPitch::onMidiPitchbend);
    }


    void MidiPitch::initVoices()
    {
        glideDelta_   = glideDeltaBuffer_.resize( numVoices_ );
        glideTarget_  = glideTargetBuffer_.resize( numVoices_ );
        freq_         = freqBuffer_.resize( numVoices_ );
    }


    void MidiPitch::setParameter( uint16_t paramId, double value, double, int16_t ) 
    {
        switch( paramId ) 
        {
        case kParamBendRange: bendRange_ = (int16_t)value;       break;
        case kParamGlideTime: setGlideTime( value );             break;
        case kParamGlideAuto: glideAuto_ = value ? true : false; break;
        }
    }


    void MidiPitch::onMidiNote( double pitch, double gate, uint16_t voice )
    {
        double freq  = PitchToFreq( pitch );
        freq_[voice] = freq;

        if( gate != 0 )
        {
            calcGlide( freq, voice );
            pitchOutport_.putEvent( freq_[voice] * bendFactor_, voice );
        }
    }


    void MidiPitch::onMidiPitchbend(int value)
    {
        double cents = ((value - 0x2000) / (double)0x2000) * bendRange_ * 100;
        bendFactor_  = pow( 2.f, cents / 1200.f );

        uint16_t maxVoices = std::min<uint16_t>( numVoices_, polyphony_->numSounding_ );
        for( uint16_t i=0; i<maxVoices; i++ )
        {
            uint16_t v = polyphony_->soundingVoices_[i];
            pitchOutport_.putEvent( freq_[v] * bendFactor_, v );
        }
    }


    void MidiPitch::processControl() throw()
    {
        if( glideFrames_ == 0 )
            return;

        uint16_t maxVoices = std::min<uint16_t>( numVoices_, polyphony_->numSounding_ );
        for( uint16_t i=0; i<maxVoices; i++ )
        {
            uint16_t v = mono_ ? 0 : polyphony_->soundingVoices_[i];

            if( glideDelta_[v] > 0 && freq_[v] >= glideTarget_[v] || glideDelta_[v] < 0 && freq_[v] <= glideTarget_[v] ) {
                glideDelta_[v] = 0.0f;
            } 
            else {
                freq_[v] += glideDelta_[v];
                pitchOutport_.putEvent( freq_[v] * bendFactor_, v );
            }
        }
    }


    void MidiPitch::calcGlide( double freq, uint16_t voice )
    {
        if( glideFrames_ > 0) 
        {
            glideTarget_[voice] = freq;
            //double prevPitch    = polyphony_->getPreviousPitch( voice );
            double prevPitch = 0;
            ASSERT(prevPitch == 1);

            if( prevPitch == -1 || glideAuto_ && polyphony_->numActive_ <= polyphony_->numUnison_ ) {
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


    void MidiPitch::setGlideTime( double time )
    {
        double msPerFrame = ( 1 / (double)INITIAL_CONTROLRATE ) * 1000;
        glideFrames_     = time / msPerFrame;
    }


    //--------------------------------------------------------
    // class MidiInput
    //--------------------------------------------------------

    MidiInput::MidiInput() : MidiPitch(
        kModuleMidiInput,
        "Midi Input",
        kPolyphonic,
        (ProcessingType)(kProcessEvent | kProcessControl))
    {
        addOutport(0, "Pitch", &pitchOutport_);
        addOutport(1, "Gate", &gateOutport_);
    }


    void MidiInput::initSignals()
    {
        polyphony_->midiNoteSignal.Connect(this, &MidiInput::onMidiNote);
        polyphony_->midiPitchbendSignal.Connect(this, &MidiPitch::onMidiPitchbend);
    }


    void MidiInput::resetSignals()
    {
        polyphony_->midiNoteSignal.Disconnect(this, &MidiInput::onMidiNote);
        polyphony_->midiPitchbendSignal.Disconnect(this, &MidiPitch::onMidiPitchbend);
    }


    void MidiInput::onMidiNote( double pitch, double gate, uint16_t voice )
    {
        MidiPitch::onMidiNote( pitch, gate, voice );

        if( gate > -1 ) {
            gateOutport_.putEvent( gate, voice );
        }
    }
}
