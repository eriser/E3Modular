
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
//#include "core/Instrument.h"


namespace e3 {
    class Settings;
    class Instrument;

    class Bank {
        friend class BankSerializer;

    public:
        ~Bank();
        void close(bool doStore, bool makeBackup);
        void load(const std::string& filepath="", bool append=false);
        void store(const std::string& path, bool saveCurrent, bool makeBackup=true);

        void createNewBank(const std::string& path);

        const std::string& getPath() const           { return path_; }
        const std::string& getName() const           { return name_; }
        void setName(const std::string& name)        { name_ = name; }
                                                     
        int getInstrumentId() const                  { return instrumentId_; }
        int getInstrumentHash() const                { return instrumentHash_; }
        void setInstrumentHash(int hash)             { instrumentHash_ = hash; }
        //Instrument& getInstrument()                  { return instrument_; }
        //Instrument* const getInstrument(uint32_t id) const;
        //Instrument* const getInstrument(const std::string& hash) const;

        Instrument* loadInstrument(int hash=0);
        void storeInstrument(Instrument* instrument);

        uint32_t getNumInstruments() const          { return (int)instrumentList_.size(); }
        void append(Instrument* instrument);

        void setSettings(Settings* settings);

        XmlElement* getXmlRoot();
        XmlElement* resetXmlRoot(XmlElement* newRoot);

    protected:
        void deleteInstruments();
        bool isValidId(int id) const        { return (id >= 0 && id < (int)getNumInstruments()); }

        std::string path_;
        std::string name_;
        Settings* settings_ = nullptr;
        bool autosave_ = true;
        bool makeBackup_ = false;

        typedef std::vector<Instrument*> InstrumentList;
        InstrumentList instrumentList_;

        int instrumentId_ = 0;
        int instrumentHash_;

        std::unique_ptr<XmlElement> xmlRoot_ = nullptr;
        std::unique_ptr<XmlDocument> xmlDocument_ = nullptr;
    };
}  // namespace e3
