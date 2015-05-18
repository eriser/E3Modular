
#include <e3_Exception.h>
#include <e3_Trace.h>
#include "JuceHeader.h"

#include "core/Serializer.h"
#include "core/Settings.h"
#include "core/Instrument.h"
#include "core/Database.h"


namespace e3 {

    void Database::build( bool force )
    {
        File databaseFile = getDatabaseFilename();

        if (force == false) {
            databaseXml_ = Serializer::loadDatabase( databaseFile );
        } else {
            databaseXml_ = nullptr;
        }

        XmlElement* settingsXml = Settings::getInstance().getDatabaseXml();

        if (databaseXml_ != nullptr && databaseXml_->getNumChildElements() || settingsXml == nullptr) {
            return;
        }

        forEachXmlChildElementWithTagName( *settingsXml, e, "path" )
        {
            File directory(e->getAllSubText());
            DirectoryIterator iter( directory, true, "*.e3mi" );
            while (iter.next())
            {
                File instrumentFile( iter.getFile() );
                Serializer::scanInstrument( databaseXml_, instrumentFile );
            }
        }
        Serializer::storeDatabase( databaseXml_, databaseFile );
    }


    void Database::load( const std::string& path )
    {
        if (path.empty()) {
            createNewBank();
        }
        else {
            XmlElement* root = Serializer::loadBank( path );
            if (root != nullptr) {
                setPath( path );
                setXml( root );
            }
        }
    }


    void Database::save( const std::string& path, bool saveCurrent, bool makeBackup )
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

        Serializer::saveBank( p, getXml() );
        setPath( p );
    }


    void Database::createNewBank()
    {
        XmlElement* root = Serializer::createNewBank();
        setXml( root );
        setPath( "" );
    }


    Instrument* Database::createNewInstrument()
    {
        return nullptr;     // TODO: implement Database::createNewInstrument
    }


    void Database::append( Instrument* instrument )
    {
        bool isNull = (nullptr == instrument);
        ASSERT( !isNull );

        //if (!isNull) instrumentList_.push_back(instrument);
    }


    Instrument* Database::loadInstrument( int id )
    {
        id = (id == -1) ? getCurrentInstrumentId() : id;

        Instrument* instrument = Serializer::loadInstrument( getXml(), id );
        if (instrument != nullptr) {
            setCurrentInstrumentId( instrument->id_ );
        }
        return instrument;
    }


    Instrument* Database::loadInstrument( const std::string& path )
    {
        return path.empty() ? createNewInstrument() : Serializer::loadInstrument( path );
    }


    void Database::saveInstrument( Instrument* instrument )
    {
        Serializer::saveInstrument( getXml(), instrument );
    }


    void Database::saveInstrumentAttributes( Instrument* instrument )
    {
        Serializer::saveInstrumentAttributes( getXml(), instrument );
    }


    void Database::saveInstrumentAttribute( int instrumentId, const std::string& name, const var& value )
    {
        Serializer::saveInstrumentAttribute( getXml(), instrumentId, name, value );
    }


    void Database::saveInstrumentLinks( Instrument* instrument )
    {
        Serializer::saveInstrumentLinks( getXml(), instrument );
    }


    XmlElement* Database::getXml()
    {
        return bankXml_.get();
    }


    void Database::setXml( XmlElement* xml )
    {
        bankXml_.reset( xml );
    }


    void Database::setCurrentInstrumentId( int id )
    {
        if (bankXml_ != nullptr) {
            bankXml_->setAttribute( "instrument", id );
        }
    }


    int Database::getCurrentInstrumentId() const
    {
        return (bankXml_ != nullptr) ? bankXml_->getIntAttribute( "instrument" ) : 0;
    }


    void Database::setPath( const std::string& path )
    {
        XmlElement* e = Settings::getInstance().getElement( "application" );
        e->setAttribute( "RecentBank", path );
    }


    std::string Database::getPath()
    {
        XmlElement* e = Settings::getInstance().getElement( "application" );
        return e->getStringAttribute( "RecentBank" ).toStdString();
    }


    const std::string Database::getName() const
    {
        return (bankXml_ != nullptr) ? bankXml_->getStringAttribute( "name" ).toStdString() : "";
    }


    void Database::setName( const std::string& name )
    {
        if (bankXml_ != nullptr) {
            bankXml_->setAttribute( "name", name );
        }
    }


    bool Database::hasLoaded() const
    {
        return bankXml_ != nullptr;
    }


    int Database::getNumInstruments() const
    {
        return (bankXml_ != nullptr) ? bankXml_->getNumChildElements() : 0;
    }


    File Database::createDefaultFilename()
    {
        PropertiesFile::Options options;
        options.applicationName     = ProjectInfo::projectName;
        options.filenameSuffix      = "e3mdb";
        options.osxLibrarySubFolder = "Preferences";

        return options.getDefaultFile();
    }


    File Database::getDatabaseFilename()
    {
        File file;
        XmlElement* settingsXml = Settings::getInstance().getDatabaseXml();

        if (settingsXml != nullptr) {
            file = settingsXml->getStringAttribute( "path" );
        }
        if (file == File()) {
            file = createDefaultFilename();
            if (settingsXml != nullptr) {
                settingsXml->setAttribute( "path", file.getFullPathName() );
            }
        }
        return file;
    }


} // namespace e3
