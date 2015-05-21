
#include "JuceHeader.h"
#include <e3_Trace.h>

#include "core/Instrument.h"
#include "core/Database.h"
#include "core/InstrumentSerializer.h"


namespace e3 {

	std::string InstrumentSerializer::defaultInstrumentXml =
		"<instrument name='Empty Instrument' id='0' preset='0' voices='32'>"
		"<modules />"
		"<links />"
		"<panels/>"
		"<presets><preset id='0' /></presets>"
		"</instrument>";


    Instrument* InstrumentSerializer::loadInstrument( const std::string& path )
    {
		XmlElement* root = nullptr;
		if( path.empty() )
		{
			root = XmlDocument::parse( defaultInstrumentXml );
		}
		else {
			File file = checkPath( path );
			root = XmlDocument::parse( file );
		}

        if (root != nullptr)
        {
			checkRoot( root );
			try {
                Instrument* instrument = new Instrument( root, path );  // processor will be owner of the instrument
                
                readAttributes( root, instrument );
                readModules( root, instrument );
                readLinks( root, instrument );
                readPresets( root, instrument );

                return instrument;
            }
            catch (const std::exception& e) {               // parse error, skip instrument
                TRACE( e.what() );
            }
        }
        return nullptr;
    }


	void InstrumentSerializer::saveInstrument( Instrument* instrument )
	{
		XmlElement* root = instrument->getXml();
		if( root != nullptr )
		{
			try {
				writeAttributes( root, instrument );
				writeModules( root, instrument );
				writeLinks( root, instrument );
				writePresets( root, instrument );

				if( root->writeToFile( File( instrument->getFilePath() ), "", "UTF-8", 1000 ) == false ) {
					THROW( std::runtime_error, "Error writing instrument file" );
				}
			}
			catch( const std::exception& e ) {       // parse error
				TRACE( e.what() );
			}
		}
	}


    void InstrumentSerializer::saveAttributes( Instrument* instrument )
    {
		XmlElement* root = instrument->getXml();
        if (root != nullptr)
        {
            try {
                writeAttributes( root, instrument );
            }
            catch (const std::exception& e) {       // xml error
                TRACE( e.what() );
            }
        }
    }


    void InstrumentSerializer::saveAttribute( Instrument* instrument, const std::string& attrName, const var value )
    {
		XmlElement* root = instrument->getXml();
        if (root != nullptr)
        {
            root->setAttribute( Identifier(attrName), value.toString() );
        }
    }


    void InstrumentSerializer::saveLinks( Instrument* instrument )
    {
		XmlElement* root = instrument->getXml();
        if (root != nullptr)
        {
            try {
                writeLinks( root, instrument );
            }
            catch (const std::exception& e) {       // xml error
                TRACE( e.what() );
            }
        }
    }


    void InstrumentSerializer::readAttributes( XmlElement* e, Instrument* instrument )
    {
        instrument->id_           = e->getIntAttribute( "id" );
        instrument->name_         = e->getStringAttribute( "name", "Unnamed Instrument" ).toStdString();
        instrument->hold_         = e->getBoolAttribute( "hold", instrument->hold_ );
        instrument->retrigger_    = e->getBoolAttribute( "retrigger", instrument->retrigger_ );
        instrument->legato_       = e->getBoolAttribute( "legato", instrument->legato_ );
        instrument->numVoices_    = (uint16_t)e->getIntAttribute( "voices", instrument->numVoices_ );
        instrument->numUnison_    = (uint16_t)e->getIntAttribute( "unison", instrument->numUnison_ );
        instrument->unisonSpread_ = (uint16_t)e->getIntAttribute( "spread", instrument->unisonSpread_ );
    }


    void InstrumentSerializer::readModules( XmlElement* parent, Instrument* instrument )
    {
        XmlElement* modules = getChildElement( parent, "modules" );

        forEachXmlChildElementWithTagName( *modules, e, "module" )
        {
            ModuleType type = (ModuleType)e->getIntAttribute( "type" );
            Module* module = instrument->createAndAddModule( type );

            try {
                module->setId( e->getIntAttribute( "id" ) );
                module->setLabel( e->getStringAttribute( "label", module->getLabel()).toStdString() );
                module->setVoicingType( (VoicingType)e->getIntAttribute( "voicing", module->getVoicingType() ) );
            }
            catch (...) {
                TRACE( "module of type %d could not be created", type );
            }
        }
    }


    void InstrumentSerializer::readLinks( XmlElement* parent, Instrument* instrument )
    {
        XmlElement* links = getChildElement( parent, "links" );

        forEachXmlChildElementWithTagName( *links, e, "link" )
        {
            Link link;
            link.setId( e->getIntAttribute( "id" ));
            link.leftModule_  = e->getIntAttribute( "left_module" );
            link.rightModule_ = e->getIntAttribute( "right_module" );
            link.leftPort_    = e->getIntAttribute( "left_port" );
            link.rightPort_   = e->getIntAttribute( "right_port" );

            instrument->addLink( link, false );
        }
    }


    void InstrumentSerializer::readPresets( XmlElement* parent, Instrument* instrument )
    {
        XmlElement* presetsXml = getChildElement( parent, "presets" );
		PresetSet& presetSet   = const_cast<PresetSet&>(instrument->getPresets());
		presetSet.selectPreset( presetsXml->getIntAttribute( "selected", -1 ) );

        forEachXmlChildElementWithTagName( *presetsXml, presetXml, "preset" )
        {
            int id                         = presetXml->getIntAttribute( "id" );
            std::string name               = presetXml->getStringAttribute( "name" ).toStdString();
            const Preset& preset           = presetSet.addPreset( id, name );

            ParameterSet& moduleParameters = preset.getModuleParameters();
            ParameterSet& linkParameters   = preset.getLinkParameters();

            forEachXmlChildElementWithTagName( *presetXml, paramXml, "param" )
            {
                int paramId  = paramXml->getIntAttribute( "id", -1 );
                int moduleId = paramXml->getIntAttribute( "module", -1 );
                int linkId   = paramXml->getIntAttribute( "link", -1 );

                if (moduleId >= 0 )
                {
                    Module* module = instrument->getModule( moduleId );
                    ASSERT( module );
                    if (module) 
                    {
                        const Parameter& param = module->getDefaultParameter( paramId );
                        readParameter( paramXml, param );
                        moduleParameters.add( param );
                    }
                }
                else if (linkId >= 0) 
                {
                    const LinkSet& links   = instrument->getLinks();
                    const Link& link       = links.get( linkId );
                    moduleId               = link.leftModule_;
                    const Parameter& param = linkParameters.addLinkParameter( linkId, moduleId );
                    
                    readParameter( paramXml, param );
                }
            }
        }
    }


    void InstrumentSerializer::readParameter( XmlElement* e, const Parameter& p )
    {
        p.value_        = e->getDoubleAttribute( "value", p.value_ );
        p.defaultValue_ = e->getDoubleAttribute( "default", p.defaultValue_ );
        p.veloSens_     = e->getDoubleAttribute( "vsens", p.veloSens_ );
        p.keyTrack_     = e->getDoubleAttribute( "ktrack", p.keyTrack_ );
        p.resolution_   = e->getDoubleAttribute( "res", p.resolution_ );
        p.label_        = e->getStringAttribute( "label", p.label_ ).toStdString();
        p.unit_         = e->getStringAttribute( "unit", p.unit_ ).toStdString();
        p.numberFormat_ = (NumberFormat)e->getIntAttribute( "fmt", p.numberFormat_ );

        p.valueShaper_.setMin( e->getDoubleAttribute( "min", p.valueShaper_.getMin() ) );
        p.valueShaper_.setMax( e->getDoubleAttribute( "max", p.valueShaper_.getMax() ) );
        p.valueShaper_.setNumSteps( (int16_t)e->getIntAttribute( "steps", p.valueShaper_.getNumSteps() ) );
        p.valueShaper_.setFactor( (int16_t)e->getIntAttribute( "factor", p.valueShaper_.getFactor() ) );

        p.midiShaper_.setControllerId( (int16_t)e->getIntAttribute( "cc", p.midiShaper_.getControllerId() ) );
        p.midiShaper_.setControllerMin( e->getDoubleAttribute( "ccmin", p.midiShaper_.getControllerMin() ) );
        p.midiShaper_.setControllerMax( e->getDoubleAttribute( "ccmax", p.midiShaper_.getControllerMax() ) );
        p.midiShaper_.setSoftTakeover( e->getBoolAttribute( "ccsoft", p.midiShaper_.getSoftTakeover() ) );
    }



    //--------------------------------------------------------------------------------
    // InstrumentSerializer write methods
    //--------------------------------------------------------------------------------

    void InstrumentSerializer::writeAttributes( XmlElement* e, Instrument* instrument )
    {
        e->removeAllAttributes();

        e->setAttribute( "id", instrument->id_ );
        e->setAttribute( "name", instrument->name_ );
        e->setAttribute( "voices", instrument->numVoices_ );

        if (instrument->numUnison_ > 1) {
            e->setAttribute( "unison", instrument->numUnison_ );
            e->setAttribute( "spread", instrument->unisonSpread_ );
        }
        if (instrument->hold_)
            e->setAttribute( "hold", instrument->hold_ );
        if (instrument->retrigger_)
            e->setAttribute( "retrigger", instrument->retrigger_ );
        if (instrument->legato_)
            e->setAttribute( "legato", instrument->legato_ );
    }


    void InstrumentSerializer::writeModules( XmlElement* e, Instrument* instrument )
    {
        XmlElement* modules = getAndClearChildElement( e, "modules" );

        const ModuleList& moduleList = instrument->getModules();
        for (ModuleList::const_iterator it = moduleList.begin(); it != moduleList.end(); it++)
        {
            XmlElement* m = modules->createNewChildElement( "module" );
            writeModule( m, *it );
        }
    }


    void InstrumentSerializer::writeLinks( XmlElement* e, Instrument* instrument )
    {
        XmlElement* links = getAndClearChildElement( e, "links" );

        const LinkSet& linkSet = instrument->getLinks();
        for (LinkSet::const_iterator it = linkSet.begin(); it != linkSet.end(); it++)
        {
            XmlElement* l = links->createNewChildElement( "link" );
            writeLink( l, *it );
        }
    }


    void InstrumentSerializer::writePresets( XmlElement* e, Instrument* instrument )
    {
        XmlElement* presetsXml = getAndClearChildElement( e, "presets" );

        const PresetSet& presetSet = instrument->getPresets();
		presetsXml->setAttribute( "selected", presetSet.getSelectedPresetId() );

        for (PresetSet::const_iterator it = presetSet.begin(); it != presetSet.end(); ++it)
        {
            XmlElement* p = presetsXml->createNewChildElement( "preset" );
            writePreset( p, instrument, *it );
        }
    }


    void InstrumentSerializer::writeModule( XmlElement* e, Module* module )
    {
        e->setAttribute( "id", module->getId() );
        e->setAttribute( "label", module->getLabel() );
        e->setAttribute( "type", module->moduleType_ );
        e->setAttribute( "poly", module->getVoicingType() );
    }


    void InstrumentSerializer::writePreset( XmlElement* e, Instrument* instrument, const Preset& preset )
    {
        e->setAttribute( "name", preset.getName() );
        e->setAttribute( "id", preset.getId() );

        const ParameterSet& moduleParameters = preset.getModuleParameters();
        for (ParameterSet::const_iterator it = moduleParameters.begin(); it != moduleParameters.end(); it++)
        {
            const Parameter& param = *it;
            ASSERT( param.controlType_ > ControlHidden );

            Module* module = instrument->getModule( param.getModuleId() );
            ASSERT( module );
            if (module != nullptr)
            {
                XmlElement* p = e->createNewChildElement( "param" );
                p->setAttribute( "module", param.getModuleId() );
                p->setAttribute( "id", param.getId() );

                const Parameter& defaultParam = module->getDefaultParameter( param.getId() );
                writeParameter( p, param, defaultParam );
            }
        }

        const ParameterSet& linkParameters = preset.getLinkParameters();
        for (ParameterSet::const_iterator it = linkParameters.begin(); it != linkParameters.end(); it++)
        {
            const Parameter& param = *it;

            XmlElement* p = e->createNewChildElement( "param" );
            p->setAttribute( "link", param.getId() );

            Parameter defaultParam( param.getId(), param.getModuleId() );
            writeParameter( p, param, defaultParam );
        }
    }


    void InstrumentSerializer::writeLink( XmlElement* e, const Link& link )
    {
        e->setAttribute( "id", link.getId() );
        e->setAttribute( "left_module", link.leftModule_ );
        e->setAttribute( "right_module", link.rightModule_ );
        e->setAttribute( "left_port", link.leftPort_ );
        e->setAttribute( "right_port", link.rightPort_ );
    }


    void InstrumentSerializer::writeParameter( XmlElement* e, const Parameter& param, const Parameter& defaultParam )
    {
        e->setAttribute( "value", param.value_ );

        if (param.defaultValue_ != defaultParam.defaultValue_)
            e->setAttribute( "default", param.defaultValue_ );
        if (param.veloSens_ != defaultParam.veloSens_)
            e->setAttribute( "vsens", param.veloSens_ );
        if (param.keyTrack_ != defaultParam.keyTrack_)
            e->setAttribute( "ktrack", param.keyTrack_ );
        if (param.resolution_ != defaultParam.resolution_)
            e->setAttribute( "res", param.resolution_ );
        if (param.controlType_ != defaultParam.controlType_)
            e->setAttribute( "control", param.controlType_ );
        if (param.label_ != defaultParam.label_)
            e->setAttribute( "label", param.label_ );
        if (param.unit_ != defaultParam.unit_)
            e->setAttribute( "unit", param.unit_ );
        if (param.numberFormat_ != defaultParam.numberFormat_)
            e->setAttribute( "fmt", param.numberFormat_ );

        const ParameterShaper& value = param.valueShaper_;
        const ParameterShaper& defaultValue = defaultParam.valueShaper_;
        if (value.getMin() != defaultValue.getMin())
            e->setAttribute( "min", value.getMin() );
        if (value.getMax() != defaultValue.getMax())
            e->setAttribute( "max", value.getMax() );
        if (value.getNumSteps() != defaultValue.getNumSteps())
            e->setAttribute( "steps", value.getNumSteps() );
        if (value.getFactor() != defaultValue.getFactor())
            e->setAttribute( "log", value.getFactor() );

        const MidiParameterShaper& midiValue = param.midiShaper_;
        const MidiParameterShaper& defaultMidiValue = defaultParam.midiShaper_;
        if (midiValue.getControllerId() != defaultMidiValue.getControllerId())
            e->setAttribute( "cc", midiValue.getControllerId() );
        if (midiValue.getControllerMin() != defaultMidiValue.getControllerMin())
            e->setAttribute( "ccmin", midiValue.getControllerMin() );
        if (midiValue.getControllerMax() != defaultMidiValue.getControllerMax())
            e->setAttribute( "ccmax", midiValue.getControllerMax() );
        if (midiValue.getSoftTakeover() != defaultMidiValue.getSoftTakeover())
            e->setAttribute( "ccsoft", midiValue.getSoftTakeover() );
    }


	void InstrumentSerializer::saveModuleComponent( Instrument* instrument, int moduleId, Point<int> pos, bool isNewModule )
	{
		XmlElement* panelXml = getPanelXml( instrument );
		if( panelXml == nullptr ) return;

		XmlElement* e = panelXml->getChildByAttribute( "id", String( moduleId ) );
		ASSERT( ( isNewModule && e == nullptr ) || e != nullptr );

		if( e == nullptr && isNewModule )
		{
			e = panelXml->createNewChildElement( "module" );
			e->setAttribute( "id", moduleId );
		}
		if( e != nullptr ) {
			e->setAttribute( "pos", pos.toString() );
		}
	}


	void InstrumentSerializer::clearModuleComponent( Instrument* instrument, int moduleId )
	{
		XmlElement* panel = getPanelXml( instrument );
		if( panel == nullptr ) return;

		if( moduleId == -1 ) {
			panel->deleteAllChildElements();
		}
		else {
			XmlElement* e = panel->getChildByAttribute( "id", String(moduleId));
			panel->removeChildElement( e, true );
		}
	}


	XmlElement* InstrumentSerializer::getPanelXml( Instrument* instrument )
	{
		XmlElement* root = instrument->getXml();
		if( root == nullptr ) return nullptr;

		XmlElement* panel = root->getChildByName( "panel" );
		return panel;
	}


    XmlElement* InstrumentSerializer::getChildElement( XmlElement* e, const std::string& name )
    {
        XmlElement* c = e->getChildByName( StringRef(name) );

        if (c == nullptr) {
            c = e->createNewChildElement( StringRef(name) );
        }
        return c;
    }


    XmlElement* InstrumentSerializer::getAndClearChildElement( XmlElement* e, const std::string& name )
    {
        XmlElement* c = getChildElement( e, name );
        c->deleteAllChildElements();
        return c;
    }


    File InstrumentSerializer::checkPath( const std::string& path )
    {
        StringRef p = path.c_str();
        File f = File::isAbsolutePath( p ) ? File( path ) : File::getCurrentWorkingDirectory().getChildFile( p );

        if (f.existsAsFile() == false)  {
            THROW( std::invalid_argument, "Instrument file not found: %s", path.c_str() );
        }
        return f;
    }


	void InstrumentSerializer::checkRoot( XmlElement* root )
	{
		if( root == nullptr || root->hasTagName( "instrument" ) == false ) {
			if( root ) delete root;
			THROW( std::runtime_error, "Incompatible instrument file" );
		}
	}


} // namespace e3