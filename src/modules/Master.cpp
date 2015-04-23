

#include "modules/Master.h"


namespace e3 {

    Master::Master() : Module(
        kModuleMaster,
        "Master",
        kMonophonic,
        kProcessAudio) 
    {
        addInport(0, &audioInport_);
        processFunction_ = static_cast< ProcessFunctionPointer >(&Master::processAudio);
       
        Parameter paramVolume(kParamVolume, "Volume", kControlSlider, 0.35);
        paramVolume.numberFormat_ = kNumberDecibel;
        paramVolume.unit_ = "db";
        parameters_.add(paramVolume);
    }

    
    void Master::updateInports()
    {
        ASSERT(numVoices_ == 1);
        audioInport_.setNumVoices(numVoices_);
        audioInportPointer_ = audioInport_.getBuffer();
    }


    void Master::setParameter(uint16_t paramId, double value, double, int16_t)
    {
        switch (paramId) {
        case kParamVolume: volume_ = value; break;
        }
    }


    void Master::processAudio() throw()
    {
        double input = *audioInportPointer_;
        *audioInportPointer_ = 0.0f;
        value_ = std::max<double>(-1, std::min<double>(1, input * volume_));
    }
}
