
#include <queue>

#include "core/Module.h"
#include "core/Instrument.h"
#include "modules/Master.h"
#include "core/Sink.h"

namespace e3 {

    Sink::Sink() : ModuleList()
    {
        reserve(100);
    }


    void Sink::reset()
    {
        ModuleList::clear();
        master_ = &zero_;
    }

    
    void Sink::compile(Instrument* instrument)
    {
        reset();
        instrument->connectModules();

        Master* master = dynamic_cast<Master*>(instrument->findModule(kModuleMaster));
        if (nullptr == master)    // nothing to do
            return;

        bool sentinel = false;
        Module* module = master;
        std::queue< Module* > queue;
        queue.push(module);

        while (queue.empty() == false)
        {
            module = queue.front();

            if (sentinel == false) {
                sentinel = instrument->checkSentinel(module);
            }

            if (module->processingType_ & (kProcessAudio | kProcessEvent))
            {
                if (contains(module) == false)
                    push_back(module);

                for (size_t i = 0; i < module->links_.size(); i++)
                {
                    LinkModel& link = module->links_.at(i);
                    Module* next = instrument->findModule(link.leftModule_);
                    if (next && next != module)
                        queue.push(next);                 // enqueue sources of current node
                }
            }
            queue.pop();
        }

        reverse(begin(), end());
        master_ = &master->value_;
        //master_ = &(dynamic_cast< Master* >(master)->value_);
    }


    bool Sink::contains(Module* module)
    {
        for (size_t i = 0; i < size(); i++) {
            if (operator[](i) == module)
                return true;
        }
        return false;
    }


    void Sink::setSampleRate(double sampleRate)
    {
        controlRateDivisor_ = (uint16_t)(sampleRate / INITIAL_CONTROLRATE);
    }


} // namespace e3