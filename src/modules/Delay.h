
#pragma once

#include <string>
#include "core/Module.h"


namespace e3 {

    class Delay : public Module
    {
    public:
        Delay();

        void initData() override;
        
        void processAudio() throw();
        void resume() override;
        void setParameter(int paramId, double value, double modulation=0.f, int voice=-1) override;
        void setSampleRate(double sampleRate) override;

        enum ParamId {
            ParamDelaytime = 1,
            ParamFeedback  = 2,
            ParamGain      = 3
        };

        std::string debugLabel_ = "Delay";

    protected:
        void updateBuffer();

        uint32_t delayTime_  = 0;
        double feedback_     = 0;
        double gain_         = 0;
        uint32_t bufferSize_ = 0;

        Buffer<uint32_t> cursorBuffer_;
        uint32_t* cursorBufferPointer_ = nullptr;
        
        Buffer< double > delayBuffer_;
        double* delayBufferPointer_ = nullptr;

        Inport audioInport_; 
        Outport audioOutport_;
        double* audioInportPointer_;
    };
}
