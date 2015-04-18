
#include <e3_Exception.h>
#include "JuceHeader.h"

#include "core/BankSerializer.h"
#include "core/Settings.h"
#include "core/Instrument.h"
#include "core/Bank.h"


namespace e3 {

    XmlElement* Bank::open(const std::string& path)
    {
        if (path.empty()) {
            return createNewBank();
        }
        else {
            XmlElement* root = BankSerializer::openBank(path);
            if (root != nullptr) {
                setXmlRoot(root);
                setPath(path);
            }
            return root;
        }
    }


    void Bank::store(const std::string& path, bool saveCurrent, bool makeBackup)
    {
        std::string p = path.empty() ? getPath() : path;
        ASSERT(p.empty() == false);

        UNUSED(saveCurrent);
        UNUSED(makeBackup);
        //if (saveCurrent)
            //saveCurrentInstrument();

        //if (makeBackup)
            //makeBackup();

        BankSerializer::storeBank(p, getXmlRoot());
        setPath(p);
    }


    XmlElement* Bank::createNewBank()
    {
        XmlElement* root = BankSerializer::createNewBank();
        setXmlRoot(root);
        setPath("");

        return root;
    }


    void Bank::append(Instrument* instrument)      
    { 
        bool isNull = (nullptr == instrument);
        ASSERT(!isNull);

        //if (!isNull) instrumentList_.push_back(instrument);
    }
    

    Instrument* Bank::loadInstrument(int hash)
    { 
        hash = (hash == 0) ? getInstrumentHash() : hash;

        Instrument* instrument = BankSerializer::loadInstrument(getXmlRoot(), hash);
        if (instrument != nullptr) {
            setInstrumentHash(instrument->hash_);
        }
        return instrument;
    }


    void Bank::storeInstrument(Instrument* instrument)
    {
        BankSerializer::storeInstrument(getXmlRoot(), instrument);
    }


    void Bank::setSettings(Settings* settings)
    { 
        settings_ = settings; 

        XmlElement* e = settings_->getElement("Application");
        autosave_     = e->getBoolAttribute("Autosave");
        makeBackup_   = e->getBoolAttribute("Backup");
    }


    XmlElement* Bank::getXmlRoot()
    {
        return xmlRoot_.get();
    }


    void Bank::setXmlRoot(XmlElement* newRoot)
    {
        xmlRoot_.reset(newRoot);
    }


    void Bank::setInstrumentHash(int hash)             
    { 
        if (xmlRoot_ != nullptr) {
            xmlRoot_->setAttribute("instrument", hash);
        }
    }


    int Bank::getInstrumentHash() const
    {
        return (xmlRoot_ != nullptr) ? xmlRoot_->getIntAttribute("instrument") : 0;
    }


    void Bank::setPath(const std::string& path)
    {
        XmlElement* e = settings_->getElement("Application");
        e->setAttribute("RecentBank", path);
    }


    std::string Bank::getPath()
    {
        XmlElement* e = settings_->getElement("Application");
        return e->getStringAttribute("RecentBank").toStdString();
    }


    const std::string Bank::getName() const           
    {
        return (xmlRoot_ != nullptr) ? xmlRoot_->getStringAttribute("name").toStdString() : "";
    }


    void Bank::setName(const std::string& name)        
    { 
        if (xmlRoot_ != nullptr) {
            xmlRoot_->setAttribute("name", name);
        }
    }



} // namespace e3
