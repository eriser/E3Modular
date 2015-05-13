
#include "core/Polyphony.h"
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
    }


    ParameterSet& Delay::getDefaultParameters() const
    {
        static ParameterSet set;
        set.clear();

        Parameter paramTime = set.addModuleParameter( ParamDelaytime, id_, "Time", ControlSlider, 0.5 );
        paramTime.unit_ = "sec";
        paramTime.numberFormat_ = NumberFloat;

        Parameter paramRepeats = set.addModuleParameter( ParamFeedback, id_, "Repeats", ControlSlider, 0.5 );
        paramRepeats.unit_ = "sec";
        paramRepeats.numberFormat_ = NumberFloat;

        Parameter paramGain = set.addModuleParameter( ParamGain, id_, "Gain", ControlSlider, 0.5 );
        paramGain.unit_ = "db";
        paramGain.numberFormat_ = NumberDecibel;

        return set;
    }


    void Delay::initData()
    {
        Module::initData();
        ASSERT( audioInport_.getNumVoices() > 0 );
        ASSERT( audioOutport_.getNumVoices() > 0 );

        audioInportPointer_ = audioInport_.getAudioBuffer();

        updateBuffer();
    }


    void Delay::setSampleRate( double sampleRate )
    {
        Module::setSampleRate( sampleRate );
        updateBuffer();
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
        bufferSize_          = (uint_fast32_t)sampleRate_;
        delayBufferPointer_  = delayBuffer_.resize( numVoices_ * bufferSize_ );
        cursorBufferPointer_ = cursorBuffer_.resize( numVoices_ );
    }


    void Delay::resume()
    {
        delayBuffer_.set( 0 );
    }


    void Delay::processAudio() throw()
    {
        //int_fast32_t maxVoices = std::min<int_fast32_t>( numVoices_, polyphony_->numSounding_ );
        double output, input;
        int_fast32_t index, v;

        for (v = 0; v < numVoices_; v++)        // TODO: use maxVoices
        {
            input = audioInportPointer_[v];
            audioInportPointer_[v] = 0;

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
