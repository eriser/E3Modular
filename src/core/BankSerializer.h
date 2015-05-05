
#pragma once

#include <string>


namespace juce {
    class XmlElement;
}

namespace e3 {

    class Bank;
    class Instrument;
    class Module;
    class Parameter;
    class Link;

    using juce::XmlElement;


    class BankSerializer
    {
    public:
        static XmlElement* readBank( const std::string& path );
        static XmlElement* createNewBank();
        static void saveBank( const std::string& path, XmlElement* root );

        static Instrument* loadInstrument( XmlElement* root, int id );
        static void saveInstrument( XmlElement* root, Instrument* instrument );
        static void saveInstrumentAttributes( XmlElement* root, Instrument* instrument );
        static void saveInstrumentAttribute( XmlElement* root, int id, const std::string& name, const var value );
        static void saveInstrumentLinks( XmlElement* root, Instrument* instrument );

    protected:
        static void readInstrument( XmlElement* element, Instrument* instrument );
        static void readModules( XmlElement* parent, Instrument* instrument );
        static void readLinks( XmlElement* parent, Instrument* instrument );
        static void readParameters( XmlElement* parent, Module* module );
        static void readParameter( XmlElement* parent, Parameter& p );

        static void writeBank( XmlElement* const e, const Bank* const bank );
        static void writeInstrument( XmlElement* const e, Instrument* instrument );
        static void writeInstrumentAttributes( XmlElement* const e, Instrument* instrument );
        static void BankSerializer::writeInstrumentLinks( XmlElement* const e, Instrument* instrument );
        static void writeModule( XmlElement* const e, const Module* module );
        static void writeParameters( XmlElement* const e, const Module* module );
        static void writeParameter( XmlElement* const e, const Parameter& param, const Parameter& defaultParam );
        static void writeLink( XmlElement* const e, const Link& link );

        static File checkPath( const std::string& path );
        static void checkRoot( XmlElement* root );

        static std::string defaultBankXml;
    };


} // namespace e3