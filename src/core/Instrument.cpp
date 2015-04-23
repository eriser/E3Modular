

#include "core/Polyphony.h"
#include "modules/Envelopes.h"
#include "modules/ModuleFactory.h"
#include "core/Instrument.h"


namespace e3 {

    Instrument::Instrument()
    {
        reserve(100);
    }

    Instrument::~Instrument()
    {
        deleteModules();
    }


    void Instrument::deleteModules()
    {
        for (iterator it = begin(); it != end(); it++)	{
            delete *it;
        }
        ModuleList::clear();
    }


    Module* Instrument::createAndAddModule(ModuleType type)
    {
        Module* module = ModuleFactory::create(type);
        push_back(module);
        module->id_ = createModuleId(type);

        return module;
    }


    void Instrument::initModules(Polyphony* polyphony, double sampleRate)
    {
        for (iterator it = begin(); it != end(); it++)
        {
            Module* m = *it;
            m->init(polyphony, sampleRate);
        }
    }


    void Instrument::resetModules()
    {
        for (iterator it = begin(); it != end(); it++) 
        {
            (*it)->reset();
        }
    }


    void Instrument::connectModules()
    {
        for (iterator it = begin(); it != end(); it++) 
        {
            connectModule(*it);
        }
    }


    void Instrument::connectModule(Module* target)
    {
        for (uint16_t i = 0; i < target->links_.size(); i++)
        {
            Link& link = target->getLink(i);
            Module* source = findModule(link.leftModule_);
            ASSERT(source);
            if (source) {
                source->connectPort(target, &link);
            }
        }
    }


    Module* Instrument::findModule(uint16_t moduleId)
    {
        for (iterator it = begin(); it != end(); it++)
        {
            if ((*it)->id_ == moduleId) {
                return *it;
            }
        }
        return nullptr;
    }


    void Instrument::setSampleRate(double sampleRate)
    {
        for (iterator it = begin(); it != end(); it++)
        {
            Module* m = *it;
            m->setSampleRate(sampleRate);
        }
    }


    void Instrument::setNumVoices(uint16_t numVoices)
    {
        //numVoices_ = (numVoices == 0) ? numVoices_ : numVoices;
        ASSERT(numVoices > 0);
        numVoices_ = numVoices;

        for (iterator it = begin(); it != end(); it++)
        {
            Module* m = *it;
            m->setNumVoices(numVoices_);
            m->update();
        }
    }


    uint16_t Instrument::createModuleId(ModuleType type)
    {
        if (type == kModuleMaster) {
            return (uint16_t)kModuleMaster;
        }

        uint16_t minId = kModuleMaster + 1;
        uint16_t maxId = (uint16_t)size();
        uint16_t id    = minId;

        for (; id <= maxId; id++) {
            if (nullptr == findModule(id)) break;
        }
        ASSERT(id <= size());
        return id;
    }


    void Instrument::resumeModules()
    {
        for (iterator it = begin(); it != end(); it++) {
            (*it)->resume();
        }
    }


    void Instrument::suspendModules()
    {
        for (iterator it = begin(); it != end(); it++) {
            (*it)->suspend();
        }
    }


    bool Instrument::checkSentinel(Module* module)
    {
        if (module->moduleType_ == kModuleAdsrEnv)
        {
            ADSREnv* adsr = dynamic_cast< ADSREnv* >(module);
            adsr->setSentinel(true);
            return true;
        }
        return false;
    }


}  // namespace e3
