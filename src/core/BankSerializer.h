
#pragma once

#include <string>


namespace juce {
    class XmlElement;
}

namespace e3 {

    class Bank;
    class Instrument;
    class ModuleModel;
    class ParameterModel;

    using juce::XmlElement;


    class BankSerializer
    {
    public:
        static void loadBank(Bank& bank, bool append);
        static void storeBank(Bank& bank, const char* path);

        static void loadInstrument(XmlElement* root, const std::string& hash, Instrument* instrument);
        static void storeInstrument(XmlElement* root, Instrument* instrument);

        //static void toString(const Bank& bank, std::string& result);

    protected:
        static void readInstrument(XmlElement* element, Instrument* instrument);
        static void readModules(XmlElement* parent, Instrument* instrument);
        static void readParameters(XmlElement* parent, ModuleModel* module);
        static void readParameter(XmlElement* parent, ParameterModel& p);
        static void readLinks(XmlElement* parent, ModuleModel* module);

        static void writeBank(XmlElement* const e, const Bank* const bank);
        static void writeInstrument(XmlElement* const e, const Instrument* instrument, int& hash);
        static void writeModule(XmlElement* const e, const ModuleModel* module);
        static void writeParameters(XmlElement* const e, const ModuleModel* module);
        static void writeParameter(XmlElement* const e, const ParameterModel& param, const ParameterModel& defaultParam);
        static void writeLinks(XmlElement* const e, const ModuleModel* module);

        static File checkPath(const std::string& path);
        static XmlElement* parse(const File& file);
        static int createHash(XmlElement* e);
    };


} // namespace e3