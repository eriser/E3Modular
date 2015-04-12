
#include <e3_Exception.h>
#include "JuceHeader.h"

#include "core/BankSerializer.h"
#include "core/Settings.h"
#include "core/Instrument.h"
#include "core/Bank.h"


namespace e3 {

    Bank::~Bank()
    {
        deleteInstruments();
    }


    void Bank::close(bool doStore, bool makeBackup)
    {
        if (doStore) {
            store(path_, true, makeBackup);
        }
        deleteInstruments();
    }


    void Bank::deleteInstruments()
    {
        //instrument_.resetModules();

        for (uint32_t i = 0; i < getNumInstruments(); i++)
        {
            Instrument* instrument = instrumentList_[i];
            if (instrument) delete instrument;
        }
        instrumentList_.clear();
    }

    
    void Bank::load(const std::string& filepath, bool append)
    {
        XmlElement* e = settings_->getElement("Application");
        std::string path = filepath.empty() ? e->getStringAttribute("BankPath").toStdString() : filepath;

        if (path.empty()) {
            createNewBank(path); // TODO: path
        }
        else {
            if (append == false) {
                deleteInstruments();
            }
            path_ = path;
            BankSerializer::loadBank(*this, append);
            e->setAttribute("BankPath", path_);
        }
    }


    void Bank::store(const std::string& path, bool saveCurrent, bool makeBackup)
    {
        if (path.empty())
            return;

        UNUSED(saveCurrent);
        UNUSED(makeBackup);
        //if (saveCurrent)
            //saveCurrentInstrument();

        //if (makeBackup)
            //makeBackup();

        BankSerializer::storeBank(*this, path.c_str());
    }


    void Bank::createNewBank(const std::string& path)
    {
        deleteInstruments();

        path_ = path;
        name_ = "New Bank";
        //currentInstrumentId_ = -1;

        //Instrument* instrument = BankSerializer::generateDefaultInstrument(0);
        //append(instrument);
        //
        //setCurrentInstrument(0, false);
    }


    void Bank::append(Instrument* instrument)      
    { 
        bool isNull = (nullptr == instrument);
        ASSERT(!isNull);

        if (!isNull) instrumentList_.push_back(instrument);
    }
    

    Instrument* Bank::loadInstrument(int hash)
    { 
        hash = (hash == 0) ? instrumentHash_ : hash;

        for (InstrumentList::const_iterator it = instrumentList_.begin(); it != instrumentList_.end(); it++)
        {
            Instrument* i = *it;
            if (i->hash_ == hash) {
                instrumentHash_ = hash;
                return i;
            }
        }
        return nullptr;
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
        //xmlRoot_.reset(xmlDocument_->getDocumentElement());
        return xmlRoot_.get();
    }


    XmlElement* Bank::resetXmlRoot(XmlElement* newRoot)
    {
        xmlRoot_.reset(newRoot);
        return getXmlRoot();
    }

} // namespace e3
