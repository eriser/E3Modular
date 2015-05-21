
#include <e3_Trace.h>

#include "gui/AudioEditor.h"
#include "core/Settings.h"
#include "core/InstrumentSerializer.h"
#include "core/CpuMeter.h"
#include "core/Polyphony.h"
#include "core/Instrument.h"
#include "core/Sink.h"

#include "core/Processor.h"


#ifdef BUILD_TARGET_VST
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new e3::Processor();
}
#endif


namespace e3 {

    Processor::Processor() : AudioProcessor(),
        polyphony_( new Polyphony() ),
        sink_( new Sink() ),
        cpuMeter_( new CpuMeter() )
    {
        Settings::getInstance().load();
        setState( ProcessorNotInitialized );
    }


    Processor::~Processor()
    {
        if (sink_ != nullptr)  delete sink_;
    }


    void Processor::prepareToPlay( double sampleRate, int )
    {
        sink_->setSampleRate( sampleRate );
        cpuMeter_->setSampleRate( (uint32_t)sampleRate );

        if (instrument_ == nullptr)
        {
            loadInstrument( Settings::getInstance().getRecentInstrumentPath() );
        }
        else {
            try {
                resetAndInitInstrument();
            }
            catch (const std::exception& e) {
                TRACE( e.what() );
            }
        }
    }


    void Processor::releaseResources()
    {
        saveInstrument();
    }


    bool Processor::isPlugin() const
    {
        ASSERT( wrapperType != wrapperType_Undefined );
        return wrapperType != wrapperType_Standalone;
    }


    AudioProcessorEditor* Processor::createEditor()
    {
        return new AudioEditor( this );
    }


    void Processor::getStateInformation( MemoryBlock& )
    {
        // You should use this method to store your parameters in the memory block.
        // You could do that either as raw data, or use the XML or ValueTree classes
        // as intermediaries to make it easy to save and load complex data.
    }

    void Processor::setStateInformation( const void*, int )
    {
        // You should use this method to restore your parameters from this memory block,
        // whose contents will have been created by the getStateInformation() call.
    }


    //-----------------------------------------------------
    // Database, Instrument, Modules
    //-----------------------------------------------------

    void Processor::saveInstrument( const std::string& path )
    {
		if( path.empty() == false ) {
			instrument_->setFilePath( path );
		}
		InstrumentSerializer::saveInstrument( instrument_ );
    }


    void Processor::loadInstrument( const std::string& path, bool saveCurrent )
    {
        suspend();

        if (instrument_ != nullptr && saveCurrent) {
			saveInstrument();
        }

		instrument_ = InstrumentSerializer::loadInstrument( path );        // this calls instrument::ctor first!
        if (instrument_ != nullptr)
        {
            polyphony_->setNumVoices( instrument_->numVoices_ );
            polyphony_->setNumUnison( instrument_->numUnison_ );
            polyphony_->setUnisonSpread( instrument_->unisonSpread_ );
            polyphony_->setHold( instrument_->hold_ );
            polyphony_->setRetrigger( instrument_->retrigger_ );
            polyphony_->setLegato( instrument_->legato_ );

            initInstrument();
        }

        resume();
    }


    void Processor::resetAndInitInstrument()
    {
        ASSERT( instrument_ );
        ASSERT( polyphony_ );
        ASSERT( instrument_->numVoices_ == polyphony_->getNumVoices() );

        polyphony_->allNotesOff( true );
        instrument_->resetModules();
        initInstrument();
    }


    void Processor::initInstrument()
    {
        instrument_->initModules( getSampleRate(), instrument_->numVoices_, polyphony_ );
        instrument_->loadPreset();
        instrument_->connectModules();
        instrument_->updateModules();

        sink_->compile( instrument_ );
    }


	bool Processor::addLink( Link& link )
    {
        suspend();
        try {
            instrument_->addLink( link );
			InstrumentSerializer::saveLinks( instrument_ );
            resetAndInitInstrument();
        }
        catch (const std::exception& e) {
            TRACE( e.what() );
            setState( ProcessorCrashed );
            return false;
        }
        resume();
        return true;
    }


    void Processor::removeLink( const Link& link )
    {
        suspend();
        try {
            instrument_->removeLink( link );
			InstrumentSerializer::saveLinks( instrument_ );
            resetAndInitInstrument();
        }
        catch (const std::exception& e) 
        {
            TRACE( e.what() );
            setState( ProcessorCrashed );
            return;
        }
        resume();
    }


    Module* Processor::addModule( int moduleType )
    {
        Module* module = nullptr;
        try {
            module = instrument_->createAndAddModule( (ModuleType)moduleType );
            saveInstrument();
        }
        catch (const std::exception& e) 
        {
            TRACE( e.what() );
            suspend();
        }
        return module;
    }


    void Processor::deleteModule( Module* module )
    {
        ASSERT( module );
        if (module == nullptr) return;

        suspend();
        try {
            instrument_->deleteModule( module );
            saveInstrument();
            resetAndInitInstrument();
        }
        catch (const std::exception& e) 
        {
            TRACE( e.what() );
            setState( ProcessorCrashed );
            return;
        }
        resume();
    }


    void Processor::setInstrumentAttribute( const std::string& name, const var& value )
    {
        ASSERT( instrument_ );
        if (instrument_ == nullptr) return;

        if (name == "name")          instrument_->name_ = value.toString().toStdString();

        else if (name == "numVoices") {
            setNumVoices( value );
        }
        else if (name == "numUnison") {
            instrument_->setNumUnison( value );
            polyphony_->setNumUnison( value );
        }
        else if (name == "unisonSpread") {
            instrument_->setUnisonSpread( value );
            polyphony_->setUnisonSpread( value );
        }
        else if (name == "hold") {
            instrument_->setHold( value );
            polyphony_->setHold( value );
        }
        else if (name == "retrigger") {
            instrument_->setRetrigger( value );
            polyphony_->setRetrigger( value );
        }
        else if (name == "legato") {
            instrument_->setLegato( value );
            polyphony_->setLegato( value );
        }
		InstrumentSerializer::saveAttribute( instrument_, name, value );
    }


    void Processor::setNumVoices( int numVoices )
    {
        suspend();
        try {
            polyphony_->setNumVoices( numVoices );
            instrument_->setNumVoices( numVoices );
            resetAndInitInstrument();
        }
        catch (const std::exception& e)
        {
            TRACE( e.what() );
            return;
        }
        resume();
    }


    //------------------------------------------------------------------------------
    // State
    //------------------------------------------------------------------------------

    void Processor::setState( ProcessorState state )
    {
        state_ = state;
        polyphony_->monitorProcessorStateEvent( state_ );
    }


    bool Processor::suspend()
    {
        bool nested = isSuspended();

        if( !nested )
        {
            suspendProcessing( true );
            if( instrument_ ) {
                instrument_->suspendModules();
            }
        }
        setState( ProcessorSuspended );
        return nested;
    }


    void Processor::resume( bool nested )
    {
        if( nested ) {
            return;
        }
        if( isSuspended() ) {
            suspendProcessing( false );
            if( instrument_ ) {
                instrument_->resumeModules();
            }
        }
        setState( ProcessorReady );
    }



    //-------------------------------------------------------
    // Processing
    //-------------------------------------------------------

    void Processor::processBlock( AudioSampleBuffer& audioBuffer, MidiBuffer& midiBuffer )
    {
        const ScopedLock scopedLock( lock_ );
        cpuMeter_->start();
        audioBuffer.clear();    // input not implemented

        int startSample  = 0;
        int totalSamples = audioBuffer.getNumSamples();
        int numSamples   = totalSamples;

        MidiBuffer::Iterator midiIterator( midiBuffer );
        midiIterator.setNextSamplePosition( startSample );
        MidiMessage msg( 0xf4, 0.0 );
        int midiEventPos;
        bool hasEvent;

        while (numSamples > 0)
        {
            hasEvent  = midiIterator.getNextEvent( msg, midiEventPos );
            hasEvent &= midiEventPos < startSample + numSamples;

            const int numSamplesNow = hasEvent ? midiEventPos - startSample : numSamples;

            if (numSamplesNow > 0) {
                sink_->process( audioBuffer, startSample, numSamplesNow );
            }

            if (hasEvent) {
                polyphony_->handleMidiMessage( msg );
            }

            startSample += numSamplesNow;
            numSamples  -= numSamplesNow;
        }

        if (cpuMeter_->stop( totalSamples )) {
            polyphony_->monitorCpuMeterEvent( cpuMeter_->getPercent() );
        }
    }


} // namespace e3