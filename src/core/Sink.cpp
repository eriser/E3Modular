
#include <queue>

#include "core/Module.h"
#include "core/Instrument.h"
#include "modules/AudioOutTerminal.h"
#include "core/Sink.h"


namespace e3 {

    Sink::Sink() : ModuleList()
    {
        reserve(100);
    }


    void Sink::reset()
    {
        ModuleList::clear();
        audioOutPointer_ = &zero_;
    }

    
    void Sink::compile(Instrument* instrument)
    {
        reset();

        AudioOutTerminal* audioOut = dynamic_cast<AudioOutTerminal*>(instrument->getModule(ModuleTypeAudioOutTerminal));
        if (nullptr == audioOut)    // nothing to do
            return;

        bool sentinel  = false;
        Module* module = audioOut;

        std::queue< Module* > queue;
        queue.push(module);

        while (queue.empty() == false)
        {
            module = queue.front();

            if (sentinel == false) {
                sentinel = instrument->checkSentinel(module);
            }

            if (module->processingType_ & (ProcessAudio | ProcessEvent))
            {
                if (contains(module) == false) 
                {
                    push_back(module);

                    LinkList links;
                    instrument->getLinksForModule( module->id_, PortTypeInport, links );

                    for (LinkList::const_iterator it = links.begin(); it != links.end(); ++it)
                    {
                        const Link& link = *it;
                        Module* next = instrument->getModule( link.leftModule_ );
                        if (next && next != module) {
                            queue.push( next );                 // enqueue sources of current module
                        }
                    }
                }
            }
            queue.pop();
        }

        reverse(begin(), end());
        audioOutPointer_ = &(audioOut->value_);
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