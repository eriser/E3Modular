
#pragma once

#include <string>
#include "core/Preset.h"


namespace juce {
    class XmlElement;
}

namespace e3 {

    class Instrument;
    class Module;
    class Parameter;
    class Link;

    using juce::XmlElement;


    class InstrumentSerializer
    {
    public:

        static Instrument* loadInstrument( const std::string& path );

		static void saveInstrument( Instrument* instrument );
        static void saveAttributes( Instrument* instrument );
		static void saveAttribute( Instrument* instrument, const std::string& attrName, const var value );
        static void saveLinks( Instrument* instrument );

		static void saveModuleComponent( Instrument* instrument, int moduleId, Point<int> pos, bool isNewModule );
		static void clearModuleComponent( Instrument* instrument, int moduleId = -1 );


    protected:
        static void readAttributes( XmlElement* e, Instrument* instrument );
        static void readModules( XmlElement* e, Instrument* instrument );
        static void readLinks( XmlElement* e, Instrument* instrument );
        static void readPresets( XmlElement* e, Instrument* instrument );
        static void readParameter( XmlElement* e, const Parameter& p );

        static void writeAttributes( XmlElement* e, Instrument* instrument );
        static void writeModules( XmlElement* e, Instrument* instrument );
        static void writeLinks( XmlElement* e, Instrument* instrument );
        static void writePresets( XmlElement* e, Instrument* instrument );

        static void writeModule( XmlElement* e, Module* module );
        static void writePreset( XmlElement* e, Instrument* instrument, const Preset& preset );
        static void writeParameter( XmlElement* e, const Parameter& param, const Parameter& defaultParam );
        static void writeLink( XmlElement* e, const Link& link );

		static XmlElement* getPanelXml( Instrument* instrument );

        static XmlElement* getChildElement( XmlElement* e, const std::string& name );
        static XmlElement* getAndClearChildElement( XmlElement* e, const std::string& name );

        static File checkPath( const std::string& path );
        //static void checkPath( File& file );
        static void checkRoot( XmlElement* root );

		static std::string defaultInstrumentXml;
    };

} // namespace e3