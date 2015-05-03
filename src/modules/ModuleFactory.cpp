
#include <e3_Exception.h>
#include "core/Module.h"
#include "modules/AudioOutTerminal.h"
#include "modules/MidiModules.h"
#include "modules/Envelopes.h"
#include "modules/SineOscillator.h"
#include "modules/Delay.h"

#include "modules/ModuleFactory.h"


namespace e3 {


    Module* ModuleFactory::create( ModuleType type )
    {
        switch (type)
        {
        case ModuleTypeAudioOutTerminal: return new AudioOutTerminal();
        case ModuleTypeMidiGate:         return new MidiGate();
        case ModuleTypeMidiFrequency:    return new MidiFrequency();
        case ModuleTypeMidiInput:        return new MidiInput();
        case ModuleTypeSineOscillator:   return new SineOscillator();
        case ModuleTypeAdsrEnvelope:     return new ADSREnvelope();
        case ModuleTypeDelay:	         return new Delay();

        default: THROW( std::domain_error, "module type %d does not exist", type );
        }
    }


    ModuleCatalog ModuleFactory::catalog_ 
    {
        { ModuleTypeAudioOutTerminal, "Audio Out" },
        { ModuleTypeMidiGate,         "Midi Gate" },
        { ModuleTypeMidiFrequency,    "Midi Frequency" },
        { ModuleTypeMidiInput,        "Midi Input" },
        { ModuleTypeSineOscillator,   "Sine" },
        { ModuleTypeAdsrEnvelope,     "ADSR" },
        { ModuleTypeDelay,            "Delay" }
    };


} // namespace e3