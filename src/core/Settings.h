
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

#ifdef BUILD_TARGET_APP
        void loadAudioDevices(AudioDeviceManager* manager, int numInputChannels, int numOutputChannels);
        void loadMidiDevices(AudioDeviceManager* manager);
        void storeAudioDevices(AudioDeviceManager* manager);
        void storeMidiDevices(AudioDeviceManager* manager);
#endif

    protected:
        void parse(const File& file);
        void parse(const std::string& settings);

        static std::string createDefaultFilename();

        File file_;
        ScopedPointer<XmlElement> root_ = nullptr;
        bool needsStore_ = false;

        const char* rootTagname_ = "E3MSettings";
        std::string defaultXml_ = 
            "<E3MSettings>"
                "<Application Autosave='1' BankPath=''/>"
                "<Standalone>"
                    "<Window x='-1' y='-1' w='1000' h='700' />"
                    "<AudioDeviceState DeviceType = '' OutputDeviceName = '' InputDeviceName = ''"
                    "SampleRate = '' BufferSize = '' />"
                "</Standalone>"
                "<Plugin />"
            "</ E3MSettings>";
    };
} // namespace e3 
