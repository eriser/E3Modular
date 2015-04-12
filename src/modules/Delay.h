
#pragma once

#include "core/Module.h"


namespace e3 {

    class Delay : public Module
    {
    public:
        Delay() : Module(kModuleDelay) {}

        void initProcess() override;
        void initPorts() override;
        void initData() override;
        void updateInPorts() override;
        
        void processAudio() throw();
        void resume() override;
        void setParameter( uint16_t paramId, double value, double modulation=0.f, int16_t voice=-1 ) override;

        enum ParamId {
            kParamDelaytime = 1,
            kParamFeedback  = 2,
            kParamGain      = 3
        };

    protected:
        uint32_t delayTime_  = 0;
        double feedback_     = 0;
        double gain_         = 0;
        uint32_t bufferSize_ = 0;

        Buffer<uint32_t> cursorBuffer_;
        uint32_t* cursor_ = nullptr;
        
        Buffer< double > delayBuffer_;
        double* buffer_ = nullptr;

    protected:
        AudioInPort audioInPort_; 
        AudioOutPort audioOutPort_;
        double* audioInPortPointer_;
    };
}
