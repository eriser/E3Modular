
#include <sstream>

#include "e3_Trace.h"
#include "e3_Exception.h"
#include "gui/Style.h"
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
            setPath("");

        if (file_.existsAsFile() == false)  {
            parse(getDefaultXml());
        } else {
            parse(file_);
        }

        XmlElement* styleXml = getStyleXml();
        style_ = new Style(styleXml);
    }


    void Settings::storeIfNeeded()
    {
        if (needsStore_) {
            store();
        }
    }


    void Settings::store()
    {
        root_->writeToFile(file_, String::empty);
        needsStore_ = false;
    }


    Style* Settings::getStyle() const
    {
        return style_.get();
    }


    XmlElement* Settings::getElement(const string& path) const
    {
        StringArray tokens;
        tokens.addTokens(StringRef(path), "/", "\"");

        XmlElement* e = root_;
        for (int i = 0; i < tokens.size(); i++)
        {
            VERIFY(e != nullptr);
            e = e->getChildByName(tokens[i]);
        }
        return e;
    }


    void Settings::setPath(const std::string& path)
    {
        if (path.empty()) 
            file_ = createDefaultFilename();
        else 
            file_ = File::getCurrentWorkingDirectory().getChildFile(path.c_str());
    }
    

    void Settings::parse(const File& file)
    {
        XmlDocument doc(file);
        root_ = doc.getDocumentElement();

        if (checkValid()) return;
        parse(getDefaultXml());
    }


    void Settings::parse(const string& settings)
    {
        XmlDocument doc(settings);
        root_ = doc.getDocumentElement();
        if (checkValid() == false) {
            THROW(std::invalid_argument, "XML error: %s", doc.getLastParseError().toUTF8());
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
        TRACE("Invalid settings file, using default settings");
        return false;
    }

    string Settings::createDefaultFilename()
    {
        PropertiesFile::Options options;
        options.applicationName     = ProjectInfo::projectName;
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        return options.getDefaultFile().getFullPathName().toStdString();
    }


    std::string Settings::getDefaultXml() 
    {
        std::ostringstream os;
        os << "<" << rootTagname_ << ">" << std::endl;
        os << defaultXml_ << std::endl;
        os << defaultStyleXml_ << std::endl;
        os << "</" << rootTagname_ << ">" << std::endl;

        return os.str();
    }



    std::string Settings::getWindowState(const std::string& context) const
    {
        std::string defaultState = "0 0 1000 700";
        std::string path = context + "/Window";
        XmlElement* e = getElement(path);

        return e ? e->getStringAttribute("state", defaultState).toStdString() : defaultState;
    }


    void Settings::setWindowState(const std::string& state, const std::string& context)
    {
        std::string path = context + "/Window";
        XmlElement* e = getElement(path);
        if (e) e->setAttribute("state", state);
    }


    std::string Settings::getRecentBankPath() const
    {
        XmlElement* e = getElement("Application");
        return e->getStringAttribute("RecentBank", "").toStdString();
    }


    void Settings::setRecentBankPath(const std::string& path)
    {
        XmlElement* e = getElement("Application");
        e->setAttribute("RecentBank", path);
    }


#ifdef BUILD_TARGET_APP

    void Settings::loadAudioDevices(AudioDeviceManager* manager, int numInputChannels, int numOutputChannels)
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        manager->getAudioDeviceSetup(setup);

        XmlElement* e          = getElement("Standalone/AudioDeviceState");
        setup.inputDeviceName  = e->getStringAttribute("InputDeviceName");
        setup.outputDeviceName = e->getStringAttribute("OutputDeviceName");
        setup.sampleRate       = e->getDoubleAttribute("SampleRate");
        setup.bufferSize       = e->getIntAttribute("BufferSize");

        String deviceType = e->getStringAttribute("DeviceType");
        manager->getAvailableDeviceTypes();             // force device scan
        manager->setCurrentAudioDeviceType(deviceType, true);

        String error = manager->initialise(numInputChannels, numOutputChannels, nullptr, true, deviceType, &setup);

        if (error.isNotEmpty())
        {
            TRACE("Error initializing audio devices from settings file: %s\n", error);

            error = manager->initialise(numInputChannels, numOutputChannels, nullptr, true);
            if (error.isNotEmpty()) {
                THROW(std::runtime_error, "Error initializing default audio devices: %s\n", error);
            }
        }
        ASSERT(manager->getCurrentAudioDevice() != nullptr);
    }


    void Settings::loadMidiDevices(AudioDeviceManager* manager) 
    {
        const StringArray allMidiIns(MidiInput::getDevices());
        StringArray midiInsFromXml;

        XmlElement* e = getElement("Standalone");
        forEachXmlChildElementWithTagName(*e, c, "MidiInput") {
            midiInsFromXml.add(c->getStringAttribute("name"));
        }

        for (int i = allMidiIns.size(); --i >= 0;) {
            manager->setMidiInputEnabled(allMidiIns[i], midiInsFromXml.contains(allMidiIns[i]));
        }
    }
    

    void Settings::storeAudioDevices(AudioDeviceManager* manager) 
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        manager->getAudioDeviceSetup(setup);

        XmlElement* e;
        e = getElement("Standalone/AudioDeviceState");
        e->setAttribute("DeviceType", manager->getCurrentAudioDeviceType());
        e->setAttribute("OutputDeviceName", setup.outputDeviceName);
        e->setAttribute("InputDeviceName", setup.inputDeviceName);
        e->setAttribute("SampleRate", setup.sampleRate);
        e->setAttribute("BufferSize", setup.bufferSize);
    }
    
    
    void Settings::storeMidiDevices(AudioDeviceManager* manager)
    {
        XmlElement* e = getElement("Standalone");
        e->deleteAllChildElementsWithTagName("MidiInput");

        const StringArray allMidiIns(MidiInput::getDevices());

        for (int i = allMidiIns.size(); --i >= 0;) {
            if (manager->isMidiInputEnabled(allMidiIns[i]))
            {
                XmlElement* c = e->createNewChildElement("MidiInput");
                c->setAttribute("name", allMidiIns[i]);
            }
        }
    }
#endif

    XmlElement* Settings::getStyleXml(const std::string& name)
    {
        if (root_ != nullptr) 
        {
            forEachXmlChildElementWithTagName(*root_, e, "Style") {
                if (e->getStringAttribute("name") == name) {
                    return e;
                }
            }

            XmlElement* e = XmlDocument::parse(defaultStyleXml_);
            ASSERT(e);
            root_->addChildElement(e);
            return e;
        }
        return nullptr;
    }

} // namespace e3