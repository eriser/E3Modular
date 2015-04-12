
#pragma once

#include "JuceHeader.h"


namespace e3 {

    class Processor;
    class Settings;

    extern AudioProcessor* JUCE_CALLTYPE createPluginFilter();


    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        Processor* getProcessor() const;
        //Settings* getSettings();
        AudioDeviceManager* getDeviceManager()  { return &deviceManager_; }

        void startPlaying();
        void stopPlaying();

    private:
        void setupDevices();
        void shutdownDevices();
        void storeAudioSettings();
        //void loadDevices();
        //void loadAudioDevices(Settings* settings);
        //void loadMidiDevices(Settings* settings);
        void createProcessor();
        void deleteProcessor();

        ScopedPointer<Processor> processor_;
        AudioProcessorPlayer player_;
        AudioDeviceManager deviceManager_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
    };

} // namespace e3