
#pragma once

#include <string>
#include "JuceHeader.h"


namespace e3 {

    class Settings
    {
    public:
        static Settings& getInstance();

        void load();
        bool store();
        void storeIfNeeded();

        XmlElement* getRoot() { return root_; }
        XmlElement* getElement(const std::string& name) const;

        void setPath(const std::string& path);
        const std::string getPath()             { return file_.getFullPathName().toStdString(); }

        XmlElement* getStyleXml( const std::string& name = "Default" ) const;

        std::string getWindowState(const std::string& context) const;
        void setWindowState(const std::string& state, const std::string& context);

        std::string getRecentBankPath() const;
        void setRecentBankPath(const std::string& path);

#ifdef BUILD_TARGET_APP
        void loadAudioDevices(AudioDeviceManager* manager, int numInputChannels, int numOutputChannels);
        void loadMidiDevices(AudioDeviceManager* manager);
        void storeAudioDevices(AudioDeviceManager* manager);
        void storeMidiDevices(AudioDeviceManager* manager);
#endif

    protected:
        bool checkValid();
        void parse(const File& file);
        void parse(const std::string& settings);

        std::string getDefaultXml() const;
        static std::string createDefaultFilename();

        File file_;
        ScopedPointer<XmlElement> root_;

        bool needsStore_ = false;

        const char* rootTagname_ = "E3MSettings";
        std::string defaultXml_ =
            "<Application Autosave='1' RecentBank='' Style='Default' />"
            "<Standalone>"
            "<WindowState x='10' y='10' w='1000' h='700' />"
            "<AudioDeviceState DeviceType = '' OutputDeviceName = '' InputDeviceName = '' "
            "SampleRate = '' BufferSize = '' />"
            "</Standalone>"
            "<Plugin>"
            "<WindowState x='10' y='10' w='1000' h='700' />"
            "</Plugin>";
    };
} // namespace e3 
