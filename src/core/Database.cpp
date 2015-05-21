
#include <e3_Exception.h>
#include <e3_Trace.h>
#include "JuceHeader.h"

#include "core/Settings.h"
#include "core/Database.h"


namespace e3 {

    Database& Database::getInstance()
    {
        static Database instance;
        return instance;
    }


    void Database::build( bool force )
    {
        instruments_.clear();
        presets_.clear();
        category1_.clear();
        category2_.clear();
        category3_.clear();
        
        File databaseFile = getDatabaseFilename();

        UNUSED( force );
        //if (force == false) {
        //    databaseXml_ = load( databaseFile );
        //} else {
        //    databaseXml_ = nullptr;
        //}

        databaseXml_ = nullptr;
        XmlElement* settingsXml = Settings::getInstance().getDatabaseXml();

        if (databaseXml_ != nullptr && databaseXml_->getNumChildElements() || settingsXml == nullptr) {
            return;
        }

        int count = 0;
        forEachXmlChildElementWithTagName( *settingsXml, e, "path" )
        {
            File directory( e->getAllSubText() );
            DirectoryIterator iter( directory, true, "*.e3mi" );
            while (iter.next())
            {
                File instrumentFile( iter.getFile() );
                //scanInstrument( databaseXml_, instrumentFile, count );
                scanInstrument( instrumentFile, count );
                count++;
            }
        }
        databaseXml_ != nullptr ? databaseXml_->writeToFile( databaseFile, "", "UTF-8", 1000 ) : false;
    }


    XmlElement* Database::load( File& file )
    {
        XmlElement* root = XmlDocument::parse( file );

        if( root == nullptr || root->hasTagName( "e3m-database" ) == false ) {
            if( root ) delete root;
            root = nullptr;
        }
        if( root == nullptr ) {
            root = new XmlElement( "e3m-database" );
        }
        return root;
    }


    void Database::scanInstrument( XmlElement* databaseXml, const File& file, int count )
    {
        XmlElement* instrumentXml = nullptr;
        ScopedPointer<XmlElement> root = XmlDocument::parse( file );

        if( root != nullptr )
        {
            try {
                instrumentXml = new XmlElement( "instrument" );
                instrumentXml->setAttribute( "path", file.getFullPathName() );
                instrumentXml->setAttribute( "name", root->getStringAttribute( "name" ) );
                instrumentXml->setAttribute( "id", count );

                XmlElement* presetsXml = root->getChildByName( "presets" );
                if( presetsXml != nullptr )
                {
                    forEachXmlChildElementWithTagName( *presetsXml, e, "preset" )
                    {
                        XmlElement* presetXml = new XmlElement( "preset" );
                        presetXml->setAttribute( "instrumentId", count );
                        presetXml->setAttribute( "presetId", e->getIntAttribute( "id" ) );
                        presetXml->setAttribute( "name", e->getStringAttribute( "name" ) );
                        presetXml->setAttribute( "category1", e->getStringAttribute( "category1" ) );
                        presetXml->setAttribute( "category2", e->getStringAttribute( "category2" ) );
                        presetXml->setAttribute( "category3", e->getStringAttribute( "category3" ) );
                        
                        instrumentXml->addChildElement( presetXml );
                    }
                }
            }
            catch( const std::exception& e ) {  // parse error
                TRACE( e.what() );
                if( instrumentXml != nullptr ) delete instrumentXml;
                return;
            }
            databaseXml->addChildElement( instrumentXml );
        }
    }


    void Database::scanInstrument( const File& file, int count )
    {
        ScopedPointer<XmlElement> root = XmlDocument::parse( file );

        if( root != nullptr )
        {
            try {
                Instrument i;
                i.id   = count;
                i.file = file;
                i.name = root->getStringAttribute( "name" );

                auto result = instruments_.insert( i );
                ASSERT( result.second == true );

                XmlElement* presetsXml = root->getChildByName( "presets" );
                if( presetsXml != nullptr )
                {
                    forEachXmlChildElementWithTagName( *presetsXml, e, "preset" )
                    {
                        Preset p;
                        p.instrumentId = i.id;
                        p.presetId     = e->getIntAttribute( "id" );
                        p.name         = e->getStringAttribute( "name" );

                        p.category1.name = e->getStringAttribute( "category1" );
                        p.category2.name = e->getStringAttribute( "category2" );
                        p.category3.name = e->getStringAttribute( "category3" );

                        auto result = presets_.insert( p );
                        if( result.second ) {
                            category1_.insert( p.category1 );
                            category2_.insert( p.category2 );
                            category3_.insert( p.category3 );
                        }
                    }
                }
            }
            catch( const std::exception& e ) {  // parse error
                TRACE( e.what() );
                return;
            }
        }
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
