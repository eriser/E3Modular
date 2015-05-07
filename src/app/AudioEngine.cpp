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
		deleteProcessor();
		shutdownDevices();
	}


	Processor* AudioEngine::getProcessor() const { return processor_; }


	void AudioEngine::startPlaying()
	{
		player_.setProcessor( processor_ );
	}


	void AudioEngine::stopPlaying()
	{
		player_.setProcessor( nullptr );
	}


	void AudioEngine::setupDevices()
	{
		ASSERT( processor_ );
		Settings& settings = Settings::getInstance();
		settings.loadAudioDevices( &deviceManager_, processor_->getNumInputChannels(), processor_->getNumOutputChannels() );
		settings.loadMidiDevices( &deviceManager_ );

		deviceManager_.addAudioCallback( &player_ );
		deviceManager_.addMidiInputCallback( String::empty, &player_ );
	}


	void AudioEngine::shutdownDevices()
	{
		deviceManager_.removeMidiInputCallback( String::empty, &player_ );
		deviceManager_.removeAudioCallback( &player_ );
	}


	void AudioEngine::createProcessor()
	{
		AudioProcessor::setTypeOfNextNewPlugin( AudioProcessor::wrapperType_Standalone );
		processor_ = dynamic_cast<Processor*>( createPluginFilter() );
		ASSERT( processor_ != nullptr );
		AudioProcessor::setTypeOfNextNewPlugin( AudioProcessor::wrapperType_Undefined );

		processor_->setPlayConfigDetails(
			JucePlugin_MaxNumInputChannels,
			JucePlugin_MaxNumOutputChannels,
			44100, 512 );
	}


	void AudioEngine::deleteProcessor()
	{
		stopPlaying();
		processor_ = nullptr;
	}


	void AudioEngine::storeAudioSettings()
	{
		ASSERT( processor_ );
		Settings& settings = Settings::getInstance();
		settings.storeAudioDevices( &deviceManager_ );
		settings.storeMidiDevices( &deviceManager_ );
	}

} // namespace e3