
#include "modules/Delay.h"


namespace e3 {

    void Delay::initProcess() 
    {
        processFunction_ = static_cast< ProcessFunctionPointer >(&Delay::processAudio);
    }

    
    void Delay::initPorts()
    {
        inPorts_.push_back(&audioInPort_);
        outPorts_.push_back(&audioOutPort_);
    }


    void Delay::initData()
    {
        bufferSize_ = (uint32_t)sampleRate_;
        buffer_     = delayBuffer_.resize(numVoices_ * bufferSize_);
        cursor_     = cursorBuffer_.resize( numVoices_ );
    }


    void Delay::updateInPorts()
    {
        audioInPortPointer_ = audioInPort_.setNumVoices( numVoices_ );
    }


    void Delay::setParameter( uint16_t paramId, double value, double, int16_t) 
    {
        switch( paramId ) {
        case kParamDelaytime: delayTime_ = (uint32_t)( value * (double)(bufferSize_ - 1)); break;
        case kParamFeedback:  feedback_  = value; break;
        case kParamGain:	  gain_      = value; break;
        }
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
            input          = audioInPortPointer_[v];
            audioInPortPointer_[v] = 0.f;

            index          = v * bufferSize_ + cursor_[v];
            output         = buffer_[index];

            buffer_[index] = input + output * feedback_;

            if( ++cursor_[v] >= delayTime_ ) {
                cursor_[v] = 0;
            }
            audioOutPort_.putAudio( input + output * gain_, v );
        }
    }
} // namespace e3
