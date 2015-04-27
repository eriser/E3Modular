
#pragma once

#include <string>
#include "JuceHeader.h"


namespace e3 {

    class Style;

    class Settings
    {
    public:
        static Settings& getInstance();

        void load();
        bool store();
        void storeIfNeeded();

        XmlElement* getRoot() { return root_; }
        XmlElement* getElement(const std::string& name) const;

        Style* getStyle() const;
            
        void setPath(const std::string& path);
        const std::string getPath()             { return file_.getFullPathName().toStdString(); }

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

        std::string getDefaultXml();
        XmlElement* getStyleXml(const std::string& name = "Default");
        static std::string createDefaultFilename();

        File file_;
        ScopedPointer<XmlElement> root_;
        ScopedPointer<Style> style_;

        bool needsStore_ = false;

        const char* rootTagname_ = "E3MSettings";
        std::string defaultXml_ =
            "<Application Autosave='1' RecentBank='' Style='Default' />"
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
            "<Color argb = 'ff5f5f60' name = 'background' / >"
            "<Color argb = 'ffbebdbb' name = 'contentBackground1' / >"
            "<Color argb = 'ff808080' name = 'contentBackground2' / >"
            "<Color argb = 'ff000000' name = 'darkText' / >"
            "<Color argb = 'ffbdbcba' name = 'lightText' / >"
            "<Color argb = 'ffd08930' name = 'highlight' / >"
            "<Color argb = 'ff3e4042' name = 'buttonOffBackground' / >"
            "<Color argb = 'ffd08930' name = 'buttonOnBackground' / >"
            "<Color argb = 'ffbdbcba' name = 'buttonOffText' / >"
            "<Color argb = 'ff000000' name = 'buttonOnText' / >"
            "<Color argb = 'cc8e8e8e' name = 'module' / >"
            "<Color argb = 'ff4d5f53' name = 'moduleMono' / >"
            "<Color argb = 'ff666666' name = 'modulePort1' / >"
            "<Color argb = 'ffd08930' name = 'modulePort2' / >"
            "<Color argb = 'ff555555' name = 'moduleText1' / >"
            "<Color argb = 'ffA21D08' name = 'moduleText2' / >"
            "<Color argb = 'ff6f6f6f' name = 'moduleBorderNormal' / >"
            "<Color argb = 'ffd08930' name = 'moduleBorderSelected' / >"
            "<Color argb = 'ffa21d08' name = 'moduleBorderFocused' / >"
            "<Color argb = 'ff777777' name = 'wire1' / >"
            "<Color argb = 'ffd08930' name = 'wire2' / >"
            "<Color argb = 'ffa21d08' name = 'master' / >"
            "<Color argb = 'ffbebdbb' name = 'masterPort1' / >"
            "<Color argb = 'ffcb4f3b' name = 'masterPort2' / >"
            "<Color argb = 'ffbebdbb' name = 'masterText1' / >"
            "<Color argb = 'ffdedede' name = 'masterText2' / >"
            "<Color argb = 'ffa21d08' name = 'masterFrame1' / >"
            "<Color argb = 'ffdadada' name = 'masterFrame2' / >"
            "<Color argb = 'ffeeeeee' name = 'masterFrame3' / >"
            "< / Style>";
    };
} // namespace e3 
