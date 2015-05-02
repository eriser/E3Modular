
#include "modules/Delay.h"


namespace e3 {

    Delay::Delay() : Module(
        ModuleTypeDelay,
        "Delay",
        Polyphonic,
        ProcessAudio )
    {
        addInport( 0, "In", &audioInport_ );
        addOutport( 0, "Out", &audioOutport_, PortTypeAudio );

        processFunction_ = static_cast<ProcessFunctionPointer>( &Delay::processAudio );

        Parameter paramTime( ParamDelaytime, "Time", ControlSlider, 0.5 );
        paramTime.unit_ = "sec";
        paramTime.numberFormat_ = NumberFloat;
        parameters_.add( paramTime );

        Parameter paramRepeats( ParamFeedback, "Repeats", ControlSlider, 0.5 );
        paramRepeats.unit_ = "sec";
        paramRepeats.numberFormat_ = NumberFloat;
        parameters_.add( paramRepeats );

        Parameter paramGain( ParamGain, "Gain", ControlSlider, 0.5 );
        paramGain.unit_ = "db";
        paramGain.numberFormat_ = NumberDecibel;
        parameters_.add( paramGain );
    }


    void Delay::initVoices()
    {
        updateBuffer();
    }


    void Delay::setSampleRate( double sampleRate )
    {
        Module::setSampleRate( sampleRate );
        updateBuffer();
    }


    void Delay::updateInports()
    {
        audioInport_.setNumVoices( numVoices_ );
        audioInportPointer_ = audioInport_.getAudioBuffer();
    }


    void Delay::setParameter( int paramId, double value, double, int )
    {
        switch (paramId) {
        case ParamDelaytime: delayTime_ = (uint32_t)(value * (double)(bufferSize_ - 1)); break;
        case ParamFeedback:  feedback_  = value; break;
        case ParamGain:	     gain_      = value; break;
        }
    }


    void Delay::updateBuffer()
    {
        bufferSize_          = (uint32_t)sampleRate_;
        delayBufferPointer_  = delayBuffer_.resize( numVoices_ * bufferSize_ );
        cursorBufferPointer_ = cursorBuffer_.resize( numVoices_ );
    }


    void Delay::resume()
    {
        delayBuffer_.set( 0 );
    }


    void Delay::processAudio() throw()
    {
        double output, input;
        uint32_t index;
        uint16_t v;

        for (v = 0; v < numVoices_; v++)
        {
            input = audioInportPointer_[v];
            audioInportPointer_[v] = 0.f;

            index  = v * bufferSize_ + cursorBufferPointer_[v];
            output = delayBufferPointer_[index];

            delayBufferPointer_[index] = input + output * feedback_;

            if (++cursorBufferPointer_[v] >= delayTime_) {
                cursorBufferPointer_[v] = 0;
            }
            audioOutport_.putAudio( input + output * gain_, v );
        }
    }
} // namespace e3
