
#pragma once

#include <string>
#include "core/Port.h"
#include "core/Module.h"


namespace e3 {

    class AudioOutTerminal : public Module 
    {
    public:
        AudioOutTerminal();
        ~AudioOutTerminal() override {}

        ParameterSet& getDefaultParameters() const override;
        void initData() override;
        void setParameter(int paramId, double value, double modulation = 0, int voice = -1) override;

        void processAudio() throw();

        enum {
            ParamVolume,
        };

        std::string debugLabel_ = "AudioOutTerminal";
        double value_ = 0;

    protected:
        Inport audioInport_;

        double volume_ = 0.1;
        double* audioInportPointer_ = nullptr;
    };
} // namespace e3
