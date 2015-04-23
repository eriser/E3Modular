
#include <e3_Exception.h>
#include "core/Module.h"
#include "modules/Master.h"
#include "modules/MidiModules.h"
#include "modules/Envelopes.h"
#include "modules/SineOscil.h"
#include "modules/Delay.h"

#include "modules/ModuleFactory.h"


namespace e3 {


    Module* ModuleFactory::create(ModuleType type)
    {
        switch (type)
        {
            case kModuleMaster:	   return new Master();
            case kModuleMidiGate:  return new MidiGate();
            case kModuleMidiPitch: return new MidiPitch();
            case kModuleMidiInput: return new MidiInput();
            case kModuleSineOscil: return new SineOscil();
            case kModuleAdsrEnv:   return new ADSREnv();
            case kModuleDelay:	   return new Delay();

        default: THROW(std::domain_error, "module type %d does not exist", type);
        }
    }

} // namespace e3