

#include "core/Polyphony.h"
#include "core/Settings.h"
#include "modules/ModuleFactory.h"
#include "core/InstrumentSerializer.h"
#include "core/Instrument.h"


namespace e3 {

    Instrument::Instrument()
    {
        modules_.reserve( 100 );
    }


    Instrument::Instrument( XmlElement* xml, const std::string& path )
    {
        setXml( xml );
        file_ = path;
        modules_.reserve( 100 );
    }


    Instrument::~Instrument()
    {
        deleteModules();
    }


    void Instrument::save( const std::string& path )
    {
        bool autosave = Settings::getInstance().getAutosavePresets();
        if (autosave) {
            saveCurrentPreset();
        }
        if (!path.empty()) {
            setFilePath( path );
        }
        InstrumentSerializer::saveInstrument( this );
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
        presetSet_.clearCurrentPreset();
        InstrumentSerializer::clearModuleComponent( this );
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
            InstrumentSerializer::clearModuleComponent( this, module->getId() );
            removeModuleParameters( module->getId() );

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


    void Instrument::loadPreset( int id, bool allowSaving )
    {
        bool autosave = Settings::getInstance().getAutosavePresets();
        if (autosave && allowSaving) {
            saveCurrentPreset();
        }
        
        currentPreset_ = setCurrentPreset( id );
        ParameterSet& parameters = currentPreset_.getModuleParameters();

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


    void Instrument::saveCurrentPreset()
    {
        if (currentPreset_.empty() == false)  {
            presetSet_.updateCurrentPreset( currentPreset_ );
        }
    }


    const Preset& Instrument::setCurrentPreset( int id )
    {
        if (presetSet_.empty()) {
            createDefaultPreset();
        }
        presetSet_.setCurrentPresetId( id );

        try {
            return presetSet_.getCurrentPreset();
        }
        catch (const std::out_of_range&) // selected presetId was invalid
        {
            presetSet_.setCurrentPresetId( 0 );
            return presetSet_.getCurrentPreset();
        }
    }


    const Preset& Instrument::addPreset()
    {
        presetSet_.addPreset( currentPreset_ );
        presetSet_.setCurrentPresetId( currentPreset_.getId() );

        return currentPreset_;
    }


    const Preset& Instrument::deleteCurrentPreset()
    {
        int id = currentPreset_.getId();
        presetSet_.removePreset( id );
        
        id = presetSet_.findClosestId( id );
        loadPreset( id );

        return currentPreset_;
    }


    void Instrument::renameCurrentPreset(const std::string& name)
    {
        currentPreset_.setName( name );
        presetSet_.renamePreset( currentPreset_.getId(), name );
    }


    void Instrument::createDefaultPreset()
    {
        const Preset& preset = presetSet_.addPreset( 0, "Default Preset" );
        presetSet_.setCurrentPresetId( 0 );

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
        ParameterSet& linkParameters = getCurrentPreset().getLinkParameters();

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
            const Parameter& p = getCurrentPreset().getLinkParameters().addLinkParameter( link.getId(), link.leftModule_ );
            p.label_ = createParameterLabel( link );
        }
    }


    void Instrument::removeLink( const Link& link )
    {
        removeLinkParameters( link.getId(), link.leftModule_ );
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


    void Instrument::removeModuleParameters( int moduleId )
    {
        getCurrentPreset().getModuleParameters().removeAllByModule( moduleId );
        
        // currentPreset_ is a copy of the original preset. We must remove from original preset too.
        if (presetSet_.empty() == false) {
            presetSet_.getCurrentPreset().getModuleParameters().removeAllByModule( moduleId );
        }
    }


    void Instrument::removeLinkParameters( int linkId, int moduleId )
    {
        getCurrentPreset().getLinkParameters().remove( linkId, moduleId );
        if (presetSet_.empty() == false) {
            presetSet_.getCurrentPreset().getLinkParameters().remove( linkId, moduleId );
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


    void Instrument::storeFilePath() const
    {
        if (file_ != File()) 
        {
            Settings& settings = Settings::getInstance();
            XmlElement* e = settings.getElement( "application" );
            e->setAttribute( "recent-instrument", file_.getFullPathName() );
        }
    }


    //std::string Instrument::getFilePath() const
    //{
    //    XmlElement* e = Settings::getInstance().getElement( "application" );
    //    return e->getStringAttribute( "recent-instrument" ).toStdString();
    //}

}  // namespace e3
