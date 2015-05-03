
#include "JuceHeader.h"
#include <e3_Trace.h>

#include "core/Instrument.h"
#include "core/Bank.h"
#include "core/BankSerializer.h"


namespace e3 {

    std::string BankSerializer::defaultBankXml =
        "<bank name='New Bank' instrument='1'>"
        "<instrument name='Default Instrument' hash='1' category='' comment='' voices='32'>"
        "</instrument>"
        "</bank>";


    XmlElement* BankSerializer::readBank( const std::string& path )
    {
        File file = checkPath( path );
        XmlElement* root = XmlDocument::parse( file );
        checkRoot( root );

        return root;
    }


    XmlElement* BankSerializer::createNewBank()
    {
        XmlElement* root = XmlDocument::parse( defaultBankXml );
        checkRoot( root );

        return root;
    }


    void BankSerializer::saveBank( const std::string& path, XmlElement* root )
    {
        if (root->writeToFile( File( path ), "", "UTF-8", 1000 ) == false) {
            THROW( std::runtime_error, "error writing bank file" );
        }
    }


    Instrument* BankSerializer::loadInstrument( XmlElement* root, int hash )
    {
        forEachXmlChildElementWithTagName( *root, e, "instrument" )
        {
            int h = e->getIntAttribute( "hash" );
            if (h == hash) {
                try {
                    Instrument* instrument = new Instrument();  // processor will be owner of the instrument
                    readInstrument( e, instrument );
                    return instrument;
                }
                catch (const std::exception& e) {               // parse error, skip instrument
                    TRACE( e.what() );
                    return nullptr;
                }
            }
        }
        return nullptr;
    }


    void BankSerializer::saveInstrument( XmlElement* root, Instrument* instrument )
    {
        forEachXmlChildElementWithTagName( *root, e, "instrument" )
        {
            int hash = e->getIntAttribute( "hash" );
            if (hash == instrument->hash_) {
                try {
                    //e->deleteAllChildElements();
                    e->deleteAllChildElementsWithTagName( "modules" );
                    e->deleteAllChildElementsWithTagName( "links" );
                    writeInstrument( e, instrument );
                    return;
                }
                catch (const std::exception& e) {       // parse error
                    TRACE( e.what() );
                }
            }
        }
    }


    void BankSerializer::readInstrument( XmlElement* e, Instrument* instrument )
    {
        instrument->hash_         = e->getIntAttribute( "hash" );
        instrument->name_         = e->getStringAttribute( "name", "Unnamed Instrument" ).toStdString();
        instrument->category_     = e->getStringAttribute( "category" ).toStdString();
        instrument->comment_      = e->getStringAttribute( "comment" ).toStdString();
        instrument->hold_         = e->getBoolAttribute( "hold", instrument->hold_ );
        instrument->retrigger_    = e->getBoolAttribute( "retrigger", instrument->retrigger_ );
        instrument->legato_       = e->getBoolAttribute( "legato", instrument->legato_ );
        instrument->numVoices_    = (uint16_t)e->getIntAttribute( "voices", instrument->numVoices_ );
        instrument->numUnison_    = (uint16_t)e->getIntAttribute( "unison", instrument->numUnison_ );
        instrument->unisonSpread_ = (uint16_t)e->getIntAttribute( "spread", instrument->unisonSpread_ );

        readModules( e, instrument );
        readLinks( e, instrument );
    }

    void BankSerializer::readModules( XmlElement* parent, Instrument* instrument )
    {
        XmlElement* modules = parent->getChildByName( "modules" );
        if (modules == nullptr)
            return;

        forEachXmlChildElementWithTagName( *modules, e, "module" )
        {
            ModuleType type = (ModuleType)e->getIntAttribute( "type" );
            Module* module = instrument->createAndAddModule( type );

            try {
                module->setId( e->getIntAttribute( "id" ) );
                module->setLabel( e->getStringAttribute( "label", module->getLabel()).toStdString() );
                module->setVoicingType( (VoicingType)e->getIntAttribute( "voicing", module->getVoicingType() ) );

                readParameters( e, module );
            }
            catch (...) {
                TRACE( "module of type %d could not be created", type );
            }
        }
    }


    void BankSerializer::readLinks( XmlElement* parent, Instrument* instrument )
    {
        XmlElement* links = parent->getChildByName( "links" );
        if (links == nullptr)
            return;

        forEachXmlChildElementWithTagName( *links, e, "link" )
        {
            Link link;
            link.leftModule_  = (uint16_t)e->getIntAttribute( "left_module" );
            link.rightModule_ = (uint16_t)e->getIntAttribute( "right_module" );
            link.leftPort_    = (uint16_t)e->getIntAttribute( "left_port" );
            link.rightPort_   = (uint16_t)e->getIntAttribute( "right_port" );

            readParameter( e, link );
            instrument->addLink( link );
        }
    }


    void BankSerializer::readParameters( XmlElement* parent, Module* module )
    {
        forEachXmlChildElementWithTagName( *parent, e, "param" )
        {
            int16_t id = (uint16_t)e->getIntAttribute( "id", -1 );

            try
            {
                Parameter p = module->getParameter( id );
                readParameter( e, p );
                module->updateParameter( p );
            }
            catch (const std::out_of_range&) {
                TRACE( "parameter with id %d does not exist for module with id %d", id, module->getId());
            }
        }
    }


    void BankSerializer::readParameter( XmlElement* e, Parameter& p )
    {
        p.value_        = e->getDoubleAttribute( "value", p.value_ );
        p.defaultValue_ = e->getDoubleAttribute( "default", p.defaultValue_ );
        p.veloSens_     = e->getDoubleAttribute( "vsens", p.veloSens_ );
        p.keyTrack_     = e->getDoubleAttribute( "ktrack", p.keyTrack_ );
        p.resolution_   = e->getDoubleAttribute( "res", p.resolution_ );
        p.controlType_  = (ControlType)e->getIntAttribute( "control", p.controlType_ );
        p.label_        = e->getStringAttribute( "label", p.label_ ).toStdString();
        p.unit_         = e->getStringAttribute( "unit", p.unit_ ).toStdString();
        p.numberFormat_ = (NumberFormat)e->getIntAttribute( "fmt", p.numberFormat_ );

        p.valueShaper_.setMin( e->getDoubleAttribute( "min", p.valueShaper_.getMin() ) );
        p.valueShaper_.setMax( e->getDoubleAttribute( "max", p.valueShaper_.getMax() ) );
        p.valueShaper_.setSteps( (int16_t)e->getIntAttribute( "steps", p.valueShaper_.getSteps() ) );
        p.valueShaper_.setFactor( (int16_t)e->getIntAttribute( "factor", p.valueShaper_.getFactor() ) );

        p.midiShaper_.setControllerId( (int16_t)e->getIntAttribute( "cc", p.midiShaper_.getControllerId() ) );
        p.midiShaper_.setControllerMin( e->getDoubleAttribute( "ccmin", p.midiShaper_.getControllerMin() ) );
        p.midiShaper_.setControllerMax( e->getDoubleAttribute( "ccmax", p.midiShaper_.getControllerMax() ) );
        p.midiShaper_.setSoftTakeover( e->getBoolAttribute( "ccsoft", p.midiShaper_.getSoftTakeover() ) );
    }



    //--------------------------------------------------------------------------------
    // BankSerializer write methods
    //--------------------------------------------------------------------------------

    void BankSerializer::writeInstrument( XmlElement* const e, Instrument* instrument )
    {
        e->setAttribute( "name", instrument->name_ );
        e->setAttribute( "category", instrument->category_ );
        e->setAttribute( "comment", instrument->comment_ );
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

        XmlElement* modules = e->createNewChildElement( "modules" );
        const ModuleList& moduleList = instrument->getModules();
        for (ModuleList::const_iterator it = moduleList.begin(); it != moduleList.end(); it++)
        {
            XmlElement* const em = modules->createNewChildElement( "module" );
            writeModule( em, *it );
        }

        XmlElement* links = e->createNewChildElement( "links" );
        const LinkList& linkList = instrument->getLinks();
        for (LinkList::const_iterator it = linkList.begin(); it != linkList.end(); it++)
        {
            XmlElement* const el = links->createNewChildElement( "link" );
            writeLink( el, *it );
        }

        e->setAttribute( "hash", 0 );
        int hash = createHash( e );
        e->setAttribute( "hash", hash );
        instrument->hash_ = hash;
    }


    void BankSerializer::writeModule( XmlElement* const e, const Module* module )
    {
        e->setAttribute( "id", module->getId() );
        e->setAttribute( "label", module->getLabel() );
        e->setAttribute( "type", module->moduleType_ );
        e->setAttribute( "poly", module->getVoicingType() );

        writeParameters( e, module );
    }

    void BankSerializer::writeParameters( XmlElement* const e, const Module* module )
    {
        const ParameterMap& params = module->getParameters();

        for (ParameterMap::const_iterator it = params.begin(); it != params.end(); it++)
        {
            const Parameter& param = it->second;
            if (param.controlType_ > ControlHidden)
            {
                XmlElement* const ep = e->createNewChildElement( "param" );
                ep->setAttribute( "id", param.id_ );

                const Parameter& defaultParam = module->getParameter( param.id_ );
                writeParameter( ep, param, defaultParam );
            }
        }
    }


    void BankSerializer::writeLink( XmlElement* const e, const Link& link )
    {
        e->setAttribute( "left_module", link.leftModule_ );
        e->setAttribute( "right_module", link.rightModule_ );
        e->setAttribute( "left_port", link.leftPort_ );
        e->setAttribute( "right_port", link.rightPort_ );

        Parameter defaultParam;
        writeParameter( e, link, defaultParam );
    }


    void BankSerializer::writeParameter( XmlElement* const e, const Parameter& param, const Parameter& defaultParam )
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
        if (value.getSteps() != defaultValue.getSteps())
            e->setAttribute( "steps", value.getSteps() );
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


    File BankSerializer::checkPath( const std::string& path )
    {
        StringRef p = path.c_str();
        File f = File::isAbsolutePath( p ) ? File( path ) : File::getCurrentWorkingDirectory().getChildFile( p );

        if (f.existsAsFile() == false)  {
            THROW( std::invalid_argument, "bank file not found: %s", path );
        }
        return f;
    }


    void BankSerializer::checkRoot( XmlElement* root )
    {
        if (root == nullptr || root->hasTagName( "bank" ) == false) {
            if (root) delete root;
            THROW( std::runtime_error, "Bank file contains errors" );
        }
    }


    int BankSerializer::createHash( XmlElement* e )
    {
        String xml = e->createDocument( "", false, true, "UTF-8", 1000 );
        return xml.hashCode();
    }

} // namespace e3