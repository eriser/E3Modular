

#include "modules/AudioOutTerminal.h"


namespace e3 {

    AudioOutTerminal::AudioOutTerminal() : Module(
        ModuleTypeAudioOutTerminal,
        "AudioOut",
        Monophonic,
        ProcessAudio) 
    {
        addInport(0, "In", &audioInport_);
        processFunction_ = static_cast< ProcessFunctionPointer >(&AudioOutTerminal::processAudio);
       
        Parameter paramVolume(ParamVolume, "Volume", ControlSlider, 0.35);
        paramVolume.numberFormat_ = NumberDecibel;
        paramVolume.unit_ = "db";
        parameters_.add(paramVolume);
    }

    
    void AudioOutTerminal::updateInports()
    {
        ASSERT(numVoices_ == 1);
        audioInport_.setNumVoices(numVoices_);
        audioInportPointer_ = audioInport_.getBuffer();
    }


    void AudioOutTerminal::setParameter(int paramId, double value, double, int16_t)
    {
        switch (paramId) {
        case ParamVolume: volume_ = value; break;
        }
    }


    void AudioOutTerminal::processAudio() throw()
    {
        double input = *audioInportPointer_;
        *audioInportPointer_ = 0.0f;
        value_ = std::max<double>(-1, std::min<double>(1, input * volume_));
    }
}
