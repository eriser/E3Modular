
#pragma once

#include <memory>
#include "JuceHeader.h"


namespace e3 {

    class AudioEngine;
    class AppWindow;


    class Application : public JUCEApplication
    {
    public:
        Application();
        ~Application();

        void initialise(const String& commandLine) override;
        void shutdown() override;
        void systemRequestedQuit() override         { quit(); }

        const String getApplicationName()           { return ProjectInfo::projectName; }
        const String getApplicationVersion()        { return ProjectInfo::versionString; }

        AudioEngine* getAudioEngine() const;
        AudioDeviceManager* getDeviceManager() const;
        AudioProcessor* getProcessor() const;

        static Application* getInstance();

        void anotherInstanceStarted(const String&) override {}
        bool moreThanOneInstanceAllowed() override          { return true; }

    protected:
        ScopedPointer<AppWindow> window_;
        ScopedPointer<AudioEngine> audioEngine_;
    };

} // namespace e3