
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>


namespace e3 {
    class Instrument;

    class Bank {
        friend class BankSerializer;

    public:
        void open(const std::string& path);
        void createNewBank();
        void save(const std::string& path, bool saveCurrent=true, bool makeBackup=true);

        const std::string getName() const;    
        void setName(const std::string& name); 
                                                     
        int getInstrumentHash() const;
        void setInstrumentHash(int hash);

        void setPath(const std::string& path);
        std::string getPath();

        Instrument* loadInstrument(int hash=0);
        void storeInstrument(Instrument* instrument);

        void append(Instrument* instrument);

        XmlElement* getXml();
        void setXml(XmlElement* e);
		bool hasLoaded() const;

    protected:
        std::unique_ptr<XmlElement> xml_          = nullptr;
    };
}  // namespace e3
