
#include "modules/Delay.h"


namespace e3 {

    Delay::Delay() : Module(
        kModuleDelay,
        "Delay",
        kPolyphonic,
        kProcessAudio)
    {
        addInport(0, &audioInport_);
        addOutport(0, &audioOutport_);

        processFunction_ = static_cast< ProcessFunctionPointer >(&Delay::processAudio);

        Parameter paramTime(kParamDelaytime, "Time", kControlSlider, 0.5);
        paramTime.unit_ = "sec";
        paramTime.numberFormat_ = kNumberFloat;
        parameters_.add(paramTime);

        Parameter paramRepeats(kParamFeedback, "Repeats", kControlSlider, 0.5);
        paramRepeats.unit_ = "sec";
        paramRepeats.numberFormat_ = kNumberFloat;
        parameters_.add(paramRepeats);

        Parameter paramGain(kParamGain, "Gain", kControlSlider, 0.5);
        paramGain.unit_ = "db";
        paramGain.numberFormat_ = kNumberDecibel;
        parameters_.add(paramGain);
    }

    
    void Delay::initVoices()
    {
        updateBuffer();
    }


    void Delay::setSampleRate(double sampleRate)
    {
        Module::setSampleRate(sampleRate);
        updateBuffer();
    }


    void Delay::updateInports()
    {
        audioInport_.setNumVoices( numVoices_ );
        audioInportPointer_ = audioInport_.getBuffer();
    }


    void Delay::setParameter( uint16_t paramId, double value, double, int16_t) 
    {
        switch( paramId ) {
        case kParamDelaytime: delayTime_ = (uint32_t)( value * (double)(bufferSize_ - 1)); break;
        case kParamFeedback:  feedback_  = value; break;
        case kParamGain:	  gain_      = value; break;
        }
    }


    void Delay::updateBuffer()
    {
        bufferSize_          = (uint32_t)sampleRate_;
        delayBufferPointer_  = delayBuffer_.resize(numVoices_ * bufferSize_);  
        cursorBufferPointer_ = cursorBuffer_.resize(numVoices_);
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
        
        for( v = 0; v < numVoices_; v++ )
        {
            input = audioInportPointer_[v];
            audioInportPointer_[v] = 0.f;

            index  = v * bufferSize_ + cursorBufferPointer_[v];
            output = delayBufferPointer_[index];

            delayBufferPointer_[index] = input + output * feedback_;

            if( ++cursorBufferPointer_[v] >= delayTime_ ) {
                cursorBufferPointer_[v] = 0;
            }
            audioOutport_.putValue( input + output * gain_, v );
        }
    }
} // namespace e3
