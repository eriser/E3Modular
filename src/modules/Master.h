
#pragma once

#include "core/Ports.h"
#include "core/Module.h"


namespace e3 {

    class Master : public Module {
    public:
        Master() : Module(kModuleMaster) {}

        ~Master() {
            int i = 0;
            i = i;
        }

        void initPorts() override;
        void initProcess() override;
        void updateInPorts() override;
        void setParameter(uint16_t paramId, double value, double modulation = 0, int16_t voice = -1);

        void processAudio() throw();

        enum {
            kParamVolume,
        };
        double value_ = 0;

    protected:
        AudioInPort audioInPort_;
        AudioOutPort audioOutPort_;

        double volume_ = 0.1;
        double* audioInPortPointer_ = nullptr;
    };
} // namespace e3
