

#include "core/Polyphony.h"
#include "modules/AdsrEnvelope.h"
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
        resetModules();

        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++) {
            Module* m = *it;
            delete m;
        }
        modules_.clear();
        links_.clear();
    }


    Module* Instrument::createAndAddModule( ModuleType type )
    {
        Module* module = ModuleFactory::create( type );
        modules_.push_back( module );
        module->id_ = createUniqueId( type );

        return module;
    }


    void Instrument::deleteModule( Module* module )       // TODO: remove all links to and from this module
    {
        ASSERT( module );
        if (module)
        {
            LinkList list;
            getLinksForModule( module->getId(), PortTypeUndefined, list );
            module->reset();
            for (LinkList::const_iterator it = list.begin(); it != list.end(); ++it) {
                removeLink( *it );
            }

            modules_.erase( std::remove( modules_.begin(), modules_.end(), module ), modules_.end() );
            ASSERT( std::find( modules_.begin(), modules_.end(), module ) == modules_.end() );
            delete module;
        }
    }


    void Instrument::loadPreset()
    {
        const Preset& preset     = getPreset();
        ParameterSet& parameters = preset.getModuleParameters();

        for (ModuleList::iterator mit = modules_.begin(); mit != modules_.end(); mit++)
        {
            Module* m = *mit;
            int id    = m->getId();

            for (ParameterSet::iterator pit = parameters.moduleFirst( id ); pit != parameters.moduleLast( id ); ++pit)
            {
                const Parameter& p = *pit;
                m->setParameter( p.getId(), p.value_, 0, -1 );
            }
        }
    }


    const Preset& Instrument::getPreset()
    {
        if (presetSet_.empty()) {
            createDefaultPreset();
            presetId_ = 0;
        }
        return presetSet_.get( presetId_ );
    }


    const Preset& Instrument::addPreset( int id, const std::string& name )   
    { 
        return presetSet_.addPreset( id, name ); 
    }



    void Instrument::createDefaultPreset()
    {
        const Preset& preset = presetSet_.addPreset(0, "Default Preset");

        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            const ParameterSet& parameters = m->getDefaultParameters();
            preset.addParameterSet( parameters );
        }
    }


    void Instrument::initModules( double sampleRate, int numVoices, Polyphony* polyphony )
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            m->init( sampleRate, numVoices, polyphony );
        }
    }


    void Instrument::resetModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = *it;
            m->reset();
        }
    }


    void Instrument::connectModules()
    {
        ParameterSet& linkParameters = getPreset().getLinkParameters();

        for (LinkSet::const_iterator it = links_.begin(); it != links_.end(); ++it)
        {
            const Link& link = *it;

            Module* source = getModule( link.leftModule_ );
            Module* target = getModule( link.rightModule_ );
            ASSERT( source );
            ASSERT( target );
            if (source && target) 
            {
                const Parameter& parameter = linkParameters.get( link.getId(), source->getId() );
                source->connect( target, PortData( link, parameter ) );
            }
        }
    }


    //void Instrument::disconnectModules( const Link& link )
    //{
    //    Module* source = getModule( link.leftModule_ );
    //    Module* target = getModule( link.rightModule_ );
    //    ASSERT( source );
    //    ASSERT( target );
    //    if (source && target) {
    //        source->disconnect( target, link );
    //    }
    //}


    void Instrument::updateModules()
    {
        for (ModuleList::iterator it = modules_.begin(); it != modules_.end(); it++)
        {
            Module* m = (*it);
            m->update();
        }
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


    void Instrument::addLink( Link& link, bool addParameter )
    {
        links_.add( link );

        if (addParameter) {
            const Parameter& p = getPreset().getLinkParameters().addLinkParameter( link.getId(), link.leftModule_ );
            p.label_ = createParameterLabel( link );
        }
    }


    void Instrument::removeLink( const Link& link )
    {
        getPreset().getLinkParameters().remove( link.getId(), link.leftModule_ );
        links_.remove( link );
    }


    void Instrument::getLinksForModule( int moduleId, PortType portType, LinkList& list )
    {
        list.clear();

        for (LinkSet::iterator it = links_.begin(); it != links_.end(); ++it)
        {
            const Link& link = *it;
            if ((portType == PortTypeInport || portType == PortTypeUndefined) && link.rightModule_ == moduleId) {
                list.push_back( link );
            }
            else if ((portType == PortTypeOutport || portType == PortTypeUndefined) && link.leftModule_ == moduleId) {
                list.push_back( link );
            }
        }
    }


    int Instrument::createUniqueId( ModuleType type )
    {
        if (type == ModuleTypeAudioOutTerminal) {
            return (int)ModuleTypeAudioOutTerminal;
        }

        int minId = ModuleTypeAudioOutTerminal + 1;
        int maxId = modules_.size();
        int id    = minId;

        for (; id <= maxId; id++) {
            if (nullptr == getModule( id )) break;
        }
        ASSERT( id <= maxId );
        return id;
    }


    bool Instrument::checkSentinel( Module* module )
    {
        if (module->moduleType_ == ModuleTypeAdsrEnvelope)
        {
            AdsrEnvelope* adsr = dynamic_cast<AdsrEnvelope*>(module);
            adsr->setSentinel( true );
            return true;
        }
        return false;
    }


    bool Instrument::hasAudioOutTerminal()
    {
        return (getModule( ModuleTypeAudioOutTerminal ) != nullptr);
    }


    std::string Instrument::createParameterLabel( const Link& link )
    {
        Module* source = getModule( link.leftModule_ );
        Module* target = getModule( link.rightModule_ );
        ASSERT( source );
        ASSERT( target );
        if (source && target)
        {
            std::ostringstream os;
            os << source->label_ << "->" << target->label_;
            return os.str();
        }
        return "";
    }


}  // namespace e3
