
#include <e3_Exception.h>
#include "JuceHeader.h"

#include "core/BankSerializer.h"
#include "core/Settings.h"
#include "core/Instrument.h"
#include "core/Bank.h"


namespace e3 {

    void Bank::load( const std::string& path )
    {
        if (path.empty()) {
            createNewBank();
        }
        else {
            XmlElement* root = BankSerializer::readBank( path );
            if (root != nullptr) {
                setPath( path );
                setXml( root );
            }
        }
    }


    void Bank::save( const std::string& path, bool saveCurrent, bool makeBackup )
    {
        std::string p = path.empty() ? getPath() : path;
        if (hasLoaded() == false || p.empty()) {
            return;
        }

        UNUSED( saveCurrent );
        UNUSED( makeBackup );
        //if (saveCurrent)
        //saveCurrentInstrument();

        //if (makeBackup)
        //makeBackup();

        BankSerializer::saveBank( p, getXml() );
        setPath( p );
    }


    void Bank::createNewBank()
    {
        XmlElement* root = BankSerializer::createNewBank();
        setXml( root );
        setPath( "" );
    }


    void Bank::append( Instrument* instrument )
    {
        bool isNull = (nullptr == instrument);
        ASSERT( !isNull );

        //if (!isNull) instrumentList_.push_back(instrument);
    }


    Instrument* Bank::loadInstrument( int id )
    {
        id = (id == -1) ? getCurrentInstrumentId() : id;

        Instrument* instrument = BankSerializer::loadInstrument( getXml(), id );
        if (instrument != nullptr) {
            setCurrentInstrumentId( instrument->id_ );
        }
        return instrument;
    }


    void Bank::saveInstrument( Instrument* instrument )
    {
        BankSerializer::saveInstrument( getXml(), instrument );
    }


    XmlElement* Bank::getXml()
    {
        return xml_.get();
    }


    void Bank::setXml( XmlElement* e )
    {
        xml_.reset( e );
    }


    void Bank::setCurrentInstrumentId( int id )
    {
        if (xml_ != nullptr) {
            xml_->setAttribute( "instrument", id );
        }
    }


    int Bank::getCurrentInstrumentId() const
    {
        return (xml_ != nullptr) ? xml_->getIntAttribute( "instrument" ) : 0;
    }


    void Bank::setPath( const std::string& path )
    {
        XmlElement* e = Settings::getInstance().getElement( "Application" );
        e->setAttribute( "RecentBank", path );
    }


    std::string Bank::getPath()
    {
        XmlElement* e = Settings::getInstance().getElement( "Application" );
        return e->getStringAttribute( "RecentBank" ).toStdString();
    }


    const std::string Bank::getName() const
    {
        return (xml_ != nullptr) ? xml_->getStringAttribute( "name" ).toStdString() : "";
    }


    void Bank::setName( const std::string& name )
    {
        if (xml_ != nullptr) {
            xml_->setAttribute( "name", name );
        }
    }


    bool Bank::hasLoaded() const
    {
        return xml_ != nullptr;
    }



} // namespace e3
