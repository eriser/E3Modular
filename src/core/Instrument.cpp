

#include "core/Polyphony.h"
#include "modules/Envelopes.h"
#include "modules/ModuleFactory.h"
#include "core/Instrument.h"


namespace e3 {

    Instrument::Instrument()
    {
        modules_.reserve( 100 );
    }

    Instrument::~Instrument()
    {
        deleteModules();
    }


    void Instrument::deleteModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)	{
            delete *it;
        }
        modules_.clear();
        links_.clear();
    }


    Module* Instrument::createAndAddModule( ModuleType type )
    {
        Module* module = ModuleFactory::create( type );
        modules_.push_back( module );
        module->id_ = createModuleId( type );

        return module;
    }


    void Instrument::deleteModule( Module* module )       // TODO: remove all links to and from this module
    {
        //ModuleList targets;
        //getTargetModules(module, &targets);

        // remove the links at the targets
        //for (int16_t i = 0; i < (int16_t)targets.size(); i++)        
        //{
        //    Module* next = targets.at(i);
        //    for (uint16_t j = 0; j < next->links_.size();)
        //    {
        //        Link& link = next->getLink(j);
        //        if (link.leftModule_ == module->id_) {
        //            next->links_.remove(link);
        //        }
        //        else j++;
        //    }
        //}

        //// disconnect ports of own links
        //for (int16_t i = 0; i < (int16_t)module->links_.size();)     
        //{
        //    Link& link = module->getLink(i);
        //    if (link.leftModule_ == module->id_)
        //    {
        //        module->links_.remove(link);
        //    }
        //    else i++;
        //}

        // remove and delete module
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            if ((*it)->id_ == module->id_) {
                delete *it;
                modules_.erase( it );
                break;
            }
        }
    }


    Link* Instrument::addLink( Link* link )
    {
        links_.add( *link );
        return &links_.back();
    }


    void Instrument::removeLink( Link* link )
    {
        links_.remove( *link );
    }


    void Instrument::getLinksForModule( int moduleId, PortType portType, LinkPointerList& list)
    {
        list.clear();

        for (LinkList::iterator it = links_.begin(); it != links_.end(); ++it)
        {
            Link& link = *it;
            if ((portType == PortTypeInport || portType == PortTypeUndefined) && link.rightModule_ == moduleId) {
                list.add( &link );
            }
            else if ((portType == PortTypeOutport || portType == PortTypeUndefined) && link.leftModule_ == moduleId) {
                list.add( &link );
            }
        }
    }


    void Instrument::initModules( Polyphony* polyphony, double sampleRate )
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            m->init( polyphony, sampleRate );
        }
    }


    void Instrument::resetModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            (*it)->reset();
        }
    }


    void Instrument::connectModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* target = (*it);
            for (LinkList::iterator it = links_.begin(); it != links_.end(); ++it)
            {
                Link* link = &(*it);
                if (link->rightModule_ == target->id_)
                {
                    Module* source = getModule( link->leftModule_ );
                    ASSERT( source );
                    if (source) {
                        source->connectPort( target, link );
                    }
                }
            }
        }
    }



    Module* Instrument::getModule( int moduleId ) const
    {
        for (ModuleList::const_iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            if ((*it)->id_ == moduleId) {
                return *it;
            }
        }
        return nullptr;
    }


    void Instrument::setSampleRate( double sampleRate )
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            m->setSampleRate( sampleRate );
        }
    }


    void Instrument::setNumVoices( uint16_t numVoices )
    {
        ASSERT( numVoices > 0 );
        numVoices_ = numVoices;

        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            m->setNumVoices( numVoices_ );
            m->update();
        }
    }


    int Instrument::createModuleId( ModuleType type )
    {
        if (type == ModuleTypeAudioOutTerminal) {
            return (int)ModuleTypeAudioOutTerminal;
        }

        int minId = ModuleTypeAudioOutTerminal + 1;
        int maxId = (uint16_t)modules_.size();
        int id    = minId;

        for (; id <= maxId; id++) {
            if (nullptr == getModule( id )) break;
        }
        ASSERT( id <= maxId );
        return id;
    }


    void Instrument::resumeModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++) {
            (*it)->resume();
        }
    }


    void Instrument::suspendModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++) {
            (*it)->suspend();
        }
    }


    bool Instrument::checkSentinel( Module* module )
    {
        if (module->moduleType_ == ModuleTypeAdsrEnvelope)
        {
            ADSREnvelope* adsr = dynamic_cast<ADSREnvelope*>(module);
            adsr->setSentinel( true );
            return true;
        }
        return false;
    }


    bool Instrument::hasAudioOutTerminal()
    {
        return (getModule( ModuleTypeAudioOutTerminal ) != nullptr);
    }



}  // namespace e3
