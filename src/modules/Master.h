
#pragma once

#include "core/Port.h"
#include "core/Module.h"


namespace e3 {

    class Master : public Module 
    {
    public:
        Master();
        ~Master() override {}

        void updateInports() override;
        void setParameter(uint16_t paramId, double value, double modulation = 0, int16_t voice = -1) override;

        void processAudio() throw();

        enum {
            kParamVolume,
        };
        double value_ = 0;

    protected:
        AudioInport audioInport_;
        AudioOutport audioOutport_;

        double volume_ = 0.1;
        double* audioInportPointer_ = nullptr;
    };
} // namespace e3
