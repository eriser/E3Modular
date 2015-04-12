
#include "modules/Master.h"


namespace e3 {

    void Master::initPorts()
    {
        inPorts_.push_back(&audioInPort_);
    }


    void Master::initProcess()
    {
        processFunction_ = static_cast< ProcessFunctionPointer >(&Master::processAudio);
    }


    void Master::updateInPorts()
    {
        ASSERT(numVoices_ == 1);
        audioInPortPointer_ = audioInPort_.setNumVoices(numVoices_);
    }


    void Master::setParameter(uint16_t paramId, double value, double, int16_t)
    {
        switch (paramId) {
        case kParamVolume: volume_ = value; break;
        }
    }


    void Master::processAudio() throw()
    {
        double input = *audioInPortPointer_;
        *audioInPortPointer_ = 0.0f;
        value_ = std::max<double>(-1, std::min<double>(1, input * volume_));
    }
}
