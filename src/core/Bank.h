
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
        void load( const std::string& path );
        void save( const std::string& path, bool saveCurrent=true, bool makeBackup=true );
        void createNewBank();

        const std::string getName() const;
        void setName( const std::string& name );

        int getCurrentInstrumentId() const;
        void setCurrentInstrumentId( int id );

        void setPath( const std::string& path );
        std::string getPath();

        Instrument* loadInstrument( int id = -1 );
        void saveInstrument( Instrument* instrument );
        void saveInstrumentAttributes( Instrument* instrument );
        void saveInstrumentAttribute( int instrumentId, const std::string& name, const var& value );
        void saveInstrumentLinks( Instrument* instrument );

        void append( Instrument* instrument );

        XmlElement* getXml();
        void setXml( XmlElement* e );
        bool hasLoaded() const;

    protected:
        std::unique_ptr<XmlElement> xml_          = nullptr;
    };
}  // namespace e3
