
#pragma once

#include <string>
#include "JuceHeader.h"


namespace e3 {

    class Settings
    {
    public:
        void load();
        void store();
        void storeIfNeeded();
        
        XmlElement* getRoot() { return root_; }
        XmlElement* getElement(const std::string& name) const;

        void setPath(const std::string& path);
        const std::string getPath()             { return file_.getFullPathName().toStdString(); }

        std::string getWindowState(const std::string& context) const;
        void setWindowState(const std::string& state, const std::string& context);

        XmlElement* getStyle(const std::string& name = "Default");

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

        std::string getDefaultXml();

        static std::string createDefaultFilename();

        File file_;
        ScopedPointer<XmlElement> root_ = nullptr;
        bool needsStore_ = false;

        const char* rootTagname_ = "E3MSettings";
        std::string defaultXml_ =
            "<Application Autosave='1' BankPath='' Style='Default' />"
            "<Standalone>"
            "<WindowState x='10' y='10' w='1000' h='700' />"
            "<AudioDeviceState DeviceType = '' OutputDeviceName = '' InputDeviceName = ''"
            "SampleRate = '' BufferSize = '' />"
            "</Standalone>"
            "<Plugin>"
            "<WindowState x='10' y='10' w='1000' h='700' />"
            "</Plugin>";

        std::string defaultStyleXml_ =
            "<Style name = 'Default'>"
            "<Color name = 'background' argb = 'ff5f5f60' />"
            "<Color name = 'contentBackground' argb = 'ffbebdbb' />"
            "<Color name = 'darkText' argb = 'ffffffff' />"
            "<Color name = 'lightText' argb = 'ffbdbcba' />"
            "<Color name = 'highlight' argb = 'ffcf732f' />"
            "<Color name = 'buttonOffBackground' argb = 'ff3e4042' />"
            "<Color name = 'buttonOnBackground' argb = 'ffd08930' />"
            "<Color name = 'buttonOffText' argb = 'ffbdbcba' />"
            "<Color name = 'buttonOnText' argb = 'ff000000' />"
            "</ Style>";
    };
} // namespace e3 
