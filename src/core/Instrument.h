
#pragma once

#include <string>
#include <vector>
#include "JuceHeader.h"
#include "core/Module.h"


namespace e3 {

    class Polyphony;

    class Instrument : public ModuleList
    {
    public:
        Instrument();
        ~Instrument();

        void deleteModules();
        void initModules(Polyphony* polyphony, double sampleRate);
        //void updateModules(double sampleRate, uint16_t numVoices=0);
        void resetModules();
        void connectModules();
        void resumeModules();
        void suspendModules();

        void setSampleRate(double sampleRate);
        void setNumVoices(uint16_t numVoices);

        Module* getModule(uint16_t moduleId);
        bool checkSentinel(Module* module);

        Module* createAndAddModule(ModuleType type);

        void setNumUnison(uint16_t numUnison)         { numUnison_    = numUnison; }
        void setUnisonSpread(uint16_t unisonSpread)   { unisonSpread_ = unisonSpread; }
        void setHold(bool hold)                       { hold_         = hold; }
        void setRetrigger(bool retrigger)             { retrigger_    = retrigger; }
        void setLegato(bool legato)                   { legato_       = legato; }

        uint16_t numVoices_    = 1;
        uint16_t numUnison_    = 1;
        uint16_t unisonSpread_ = 5;
        bool hold_             = false;
        bool retrigger_        = false;
        bool legato_           = false;
        bool ready_            = false;
        std::string name_      = "Default";
        std::string category_;
        std::string comment_;
        int hash_;

    protected:
        bool hasMaster()    { return nullptr != getModule(kModuleMaster); }
        void connectModule(Module* target);
        uint16_t createModuleId(ModuleType type);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
    };
}  // namespace e3
