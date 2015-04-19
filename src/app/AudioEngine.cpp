#include <e3_Exception.h>

#include "core/Settings.h"
#include "core/Processor.h"
#include "app/Application.h"
#include "app/AudioEngine.h"


namespace e3 {

    AudioEngine::AudioEngine()
    {
        createProcessor();
        setupDevices();
        startPlaying();
    }


    AudioEngine::~AudioEngine()
    {
        storeAudioSettings();
        deleteProcessor();
        shutdownDevices();
    }


    Processor* AudioEngine::getProcessor() const { return processor_; }


    void AudioEngine::startPlaying()
    {
        player_.setProcessor(processor_);
    }


    void AudioEngine::stopPlaying()
    {
        player_.setProcessor(nullptr);
    }


    void AudioEngine::setupDevices()
    {
        ASSERT(processor_);
        Settings* settings = processor_->getSettings();
        settings->loadAudioDevices(&deviceManager_, processor_->getNumInputChannels(), processor_->getNumOutputChannels());
        settings->loadMidiDevices(&deviceManager_);

        deviceManager_.addAudioCallback(&player_);
        deviceManager_.addMidiInputCallback(String::empty, &player_);
    }


    void AudioEngine::shutdownDevices()
    {
        deviceManager_.removeMidiInputCallback(String::empty, &player_);
        deviceManager_.removeAudioCallback(&player_);
    }


    void AudioEngine::createProcessor()
    {
        AudioProcessor::setTypeOfNextNewPlugin(AudioProcessor::wrapperType_Standalone);
        processor_ = dynamic_cast<Processor*>(createPluginFilter());
        jassert(processor_ != nullptr);
        AudioProcessor::setTypeOfNextNewPlugin(AudioProcessor::wrapperType_Undefined);

        processor_->setPlayConfigDetails(
            JucePlugin_MaxNumInputChannels,
            JucePlugin_MaxNumOutputChannels,
            44100, 512);
    }


    void AudioEngine::deleteProcessor()
    {
        stopPlaying();
        processor_ = nullptr;
    }


    void AudioEngine::storeAudioSettings()
    {
        ASSERT(processor_);
        Settings* settings = processor_->getSettings();
        settings->storeAudioDevices(&deviceManager_);
        settings->storeMidiDevices(&deviceManager_);
        //settings->store();
    }

    /*
    // Reads the audio configuration from the settings xml
    // The xml format is different from the juce xml format,
    // so we need custom code here.
    //
    void AudioEngine::loadDevices()
    {
        Settings* settings = Application::getSettings();
        loadAudioDevices(settings);
        loadMidiDevices(settings);
    }


    void AudioEngine::loadAudioDevices(Settings* settings)
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager_.getAudioDeviceSetup(setup);
        int numInputs   = processor_->getNumInputChannels();
        int numOutputs  = processor_->getNumOutputChannels();

        XmlElement* e          = settings->getElement("Standalone/AudioDeviceState");
        setup.inputDeviceName  = e->getStringAttribute("InputDeviceName");
        setup.outputDeviceName = e->getStringAttribute("OutputDeviceName");
        setup.sampleRate       = e->getDoubleAttribute("SampleRate");
        setup.bufferSize       = e->getIntAttribute("BufferSize");

        String deviceType = e->getStringAttribute("DeviceType");
        deviceManager_.getAvailableDeviceTypes();             // force device scan
        deviceManager_.setCurrentAudioDeviceType(deviceType, true);

        String error = deviceManager_.initialise(numInputs, numOutputs, nullptr, true, deviceType, &setup);

        if (error.isNotEmpty()) 
        {
            TRACE("Error initializing audio devices from settings file: %s\n", error);

            error = deviceManager_.initialise(numInputs, numOutputs, nullptr, true);
            if (error.isNotEmpty()) {                
                THROW(std::runtime_error, "Error initializing default audio devices: %s\n", error);
            }
        }
        ASSERT(deviceManager_.getCurrentAudioDevice() != nullptr);
    }


    void AudioEngine::loadMidiDevices(Settings* settings)
    {
        const StringArray allMidiIns(MidiInput::getDevices());
        StringArray midiInsFromXml;

        XmlElement* e = settings->getElement("Standalone");
        forEachXmlChildElementWithTagName(*e, c, "MidiInput") {
            midiInsFromXml.add(c->getStringAttribute("name"));
        }

        for (int i = allMidiIns.size(); --i >= 0;) {
            deviceManager_.setMidiInputEnabled(allMidiIns[i], midiInsFromXml.contains(allMidiIns[i]));
        }
    }


    void AudioEngine::updateSettings()
    {
        Settings* settings = Application::getSettings();

        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager_.getAudioDeviceSetup(setup);

        XmlElement* e;

        // AudioDeviceState
        //
        e = settings->getElement("Standalone/AudioDeviceState");
        e->setAttribute("DeviceType", deviceManager_.getCurrentAudioDeviceType());
        e->setAttribute("OutputDeviceName", setup.outputDeviceName);
        e->setAttribute("InputDeviceName", setup.inputDeviceName);
        e->setAttribute("SampleRate", setup.sampleRate);
        e->setAttribute("BufferSize", setup.bufferSize);

        // MidiInputs
        //
        e = settings->getElement("Standalone");
        e->deleteAllChildElementsWithTagName("MidiInput");
        const StringArray allMidiIns(MidiInput::getDevices());
        for (int i = allMidiIns.size(); --i >= 0;) {
            if (deviceManager_.isMidiInputEnabled(allMidiIns[i]))
            {
                XmlElement* c = e->createNewChildElement("MidiInput");
                c->setAttribute("name", allMidiIns[i]);
            }
        }
        settings->store();
    }
    */

} // namespace e3