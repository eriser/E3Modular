
#include <sstream>

#include "e3_Trace.h"
#include "e3_Exception.h"
#include "core/Settings.h"

using std::string;

namespace e3 {

    Settings& Settings::getInstance()
    {
        static Settings instance;
        return instance;
    }


    void Settings::load()
    {
        if (file_ == File())
            setPath( "" );

        if (file_.existsAsFile() == false)  {
            parse( getDefaultXml() );
        }
        else {
            parse( file_ );
        }
    }


    void Settings::storeIfNeeded()
    {
        if (needsStore_) {
            store();
        }
    }


    bool Settings::store()
    {
        if (file_ == File() || file_.isDirectory() || !file_.getParentDirectory().createDirectory())
            return false;

        needsStore_ = false;
        return root_->writeToFile( file_, String::empty );
    }


    XmlElement* Settings::getElement( const string& path ) const
    {
        StringArray tokens;
        tokens.addTokens( StringRef( path ), "/", "\"" );

        XmlElement* e = root_;
        for (int i = 0; i < tokens.size(); i++)
        {
            VERIFY( e != nullptr );
            e = e->getChildByName( tokens[i] );
        }
        return e;
    }


    void Settings::setPath( const std::string& path )
    {
        if (path.empty())
            file_ = createDefaultFilename();
        else
            file_ = File::getCurrentWorkingDirectory().getChildFile( path.c_str() );
    }


    void Settings::parse( const File& file )
    {
        XmlDocument doc( file );
        root_ = doc.getDocumentElement();

        if (checkValid()) return;
        parse( getDefaultXml() );
    }


    void Settings::parse( const string& settings )
    {
        XmlDocument doc( settings );
        root_ = doc.getDocumentElement();
        if (checkValid() == false) {
            THROW( std::invalid_argument, "XML error: %s", doc.getLastParseError().toUTF8() );
        }
        storeIfNeeded();
        needsStore_ = true;
    }


    bool Settings::checkValid()
    {
        if (root_ != nullptr) {
            if (root_->getTagName() == rootTagname_) {
                return true;
            }
        }
        TRACE( "Invalid settings file, using default settings" );
        return false;
    }


    File Settings::createDefaultFilename()
    {
        PropertiesFile::Options options;
        options.applicationName     = ProjectInfo::projectName;
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        return options.getDefaultFile();
    }


    std::string Settings::getDefaultXml() const
    {
        std::ostringstream os;
        os << "<" << rootTagname_ << ">" << std::endl;
        os << defaultXml_ << std::endl;
        os << "</" << rootTagname_ << ">" << std::endl;

        return os.str();
    }


    XmlElement* Settings::getStyleXml( const std::string& name ) const
    {
        if (root_ != nullptr)
        {
            forEachXmlChildElementWithTagName( *root_, e, "style" ) {
                if (e->getStringAttribute( "name" ) == name) {
                    return e;
                }
            }
        }
        return nullptr;
    }

    
    XmlElement* Settings::getDatabaseXml() const
    {
        if (root_ != nullptr)
        {
            XmlElement* databaseXml = getElement( "database" );
            if (databaseXml == nullptr) 
            {
                databaseXml = new XmlElement( "database" );
                databaseXml->setAttribute( "path", "" );
                root_->addChildElement( databaseXml );
            }
            return databaseXml;
        }
        return nullptr;
    }


    std::string Settings::getWindowState( const std::string& context ) const
    {
        std::string defaultState = "0 0 1000 700";
        std::string path = context + "/window";
        XmlElement* e = getElement( path );

        return e ? e->getStringAttribute( "state", defaultState ).toStdString() : defaultState;
    }


    void Settings::setWindowState( const std::string& state, const std::string& context )
    {
        std::string path = context + "/window";
        XmlElement* e = getElement( path );
        if (e) e->setAttribute( "state", state );
    }


    std::string Settings::getRecentInstrumentPath() const
    {
        XmlElement* e = getElement( "application" );
        return e->getStringAttribute( "recent-instrument", "" ).toStdString();
    }


    void Settings::setRecentInstrumentPath( const std::string& path )
    {
        XmlElement* e = getElement( "application" );
        e->setAttribute( "recent-instrument", path );
    }


    bool Settings::getAutosavePresets() const
    {
        XmlElement* e = getElement( "application" );
        return e->getBoolAttribute( "autosave-presets" );
    }


    bool Settings::getAutosaveInstruments() const
    {
        XmlElement* e = getElement( "application" );
        return e->getBoolAttribute( "autosave-instruments" );
    }


#ifdef BUILD_TARGET_APP

    void Settings::loadAudioDevices( AudioDeviceManager* manager, int numInputChannels, int numOutputChannels )
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        manager->getAudioDeviceSetup( setup );

        XmlElement* e          = getElement( "standalone/audio-devices" );
        setup.inputDeviceName  = e->getStringAttribute( "input-device" );
        setup.outputDeviceName = e->getStringAttribute( "output-device" );
        setup.sampleRate       = e->getDoubleAttribute( "samplerate" );
        setup.bufferSize       = e->getIntAttribute( "buffersize" );

        String deviceType = e->getStringAttribute( "device-type" );
        manager->getAvailableDeviceTypes();             // force device scan
        manager->setCurrentAudioDeviceType( deviceType, true );

        String error = manager->initialise( numInputChannels, numOutputChannels, nullptr, true, deviceType, &setup );

        if (error.isNotEmpty())
        {
            TRACE( "Error initializing audio devices from settings file: %s\n", error );

            error = manager->initialise( numInputChannels, numOutputChannels, nullptr, true );
            if (error.isNotEmpty()) {
                THROW( std::runtime_error, "Error initializing default audio devices: %s\n", error );
            }
        }
    }


    void Settings::loadMidiDevices( AudioDeviceManager* manager )
    {
        const StringArray allMidiIns( MidiInput::getDevices() );
        StringArray midiInsFromXml;

        XmlElement* e = getElement( "standalone" );
        forEachXmlChildElementWithTagName( *e, c, "midi-input" ) {
            midiInsFromXml.add( c->getStringAttribute( "name" ) );
        }

        for (int i = allMidiIns.size(); --i >= 0;) {
            manager->setMidiInputEnabled( allMidiIns[i], midiInsFromXml.contains( allMidiIns[i] ) );
        }
    }


    void Settings::storeAudioDevices( AudioDeviceManager* manager )
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        manager->getAudioDeviceSetup( setup );

        XmlElement* e;
        e = getElement( "standalone/audio-devices" );
        e->setAttribute( "device-type", manager->getCurrentAudioDeviceType() );
        e->setAttribute( "output-device", setup.outputDeviceName );
        e->setAttribute( "input-device", setup.inputDeviceName );
        e->setAttribute( "samplerate", setup.sampleRate );
        e->setAttribute( "buffersize", setup.bufferSize );
    }


    void Settings::storeMidiDevices( AudioDeviceManager* manager )
    {
        XmlElement* e = getElement( "standalone" );
        e->deleteAllChildElementsWithTagName( "midi-input" );

        const StringArray allMidiIns( MidiInput::getDevices() );

        for (int i = allMidiIns.size(); --i >= 0;) {
            if (manager->isMidiInputEnabled( allMidiIns[i] ))
            {
                XmlElement* c = e->createNewChildElement( "midi-input" );
                c->setAttribute( "name", allMidiIns[i] );
            }
        }
    }
#endif

} // namespace e3