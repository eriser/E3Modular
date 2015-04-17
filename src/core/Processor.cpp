

#include "gui/AudioEditor.h"
#include "core/CpuMeter.h"
#include "core/Polyphony.h"
#include "core/Bank.h"
#include "core/Instrument.h"
#include "core/Processor.h"



#ifdef BUILD_TARGET_VST
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new e3::Processor();
}
#endif


namespace e3 {

    Processor::Processor() : AudioProcessor(),
        polyphony_(new Polyphony()),
        bank_(new Bank()),
        cpuMeter_(new CpuMeter())
    {
        settings_.load();
        bank_->setSettings(&settings_);
    }

    Processor::~Processor()
    {
        settings_.store();
    }


    void Processor::prepareToPlay(double sampleRate, int)
    {
        if (nullptr == instrument_) {
            loadBank();
        }

        sink_.setSampleRate(sampleRate);
        cpuMeter_->setSampleRate((uint32_t)sampleRate);
    }


    void Processor::loadBank()
    {
        suspend();
        bank_->load();
        loadInstrument();
        resume();
    }

    
    void Processor::loadInstrument(int hash)
    {
        if (instrument_ != nullptr) {
            instrument_->resetModules();
            bank_->storeInstrument(instrument_);
        }

        instrument_ = bank_->loadInstrument(hash);
        if (instrument_ != nullptr)
        {
            uint16_t numVoices = instrument_->numVoices_;
            polyphony_->setNumVoices(numVoices);

            instrument_->initModules(polyphony_.get());
            instrument_->updateModules(getSampleRate(), numVoices);
            sink_.compile(instrument_);
        }
    }


    bool Processor::isPlugin() const
    {
        ASSERT(wrapperType != wrapperType_Undefined);
        return wrapperType != wrapperType_Standalone;
    }


    AudioProcessorEditor* Processor::createEditor()
    {
        return new AudioEditor(this);
    }


    void Processor::processBlock(AudioSampleBuffer& audioBuffer, MidiBuffer& midiBuffer)
    {
        const ScopedLock scopedLock(lock_);
        cpuMeter_->start();
        audioBuffer.clear();    // input not implemented

        int startSample  = 0;
        int totalSamples = audioBuffer.getNumSamples();
        int numSamples   = totalSamples;

        MidiBuffer::Iterator midiIterator(midiBuffer);
        midiIterator.setNextSamplePosition(startSample);
        MidiMessage msg(0xf4, 0.0);
        int midiEventPos;
        bool hasEvent;

        while (numSamples > 0)
        {
            hasEvent  = midiIterator.getNextEvent(msg, midiEventPos);
            hasEvent &= midiEventPos < startSample + numSamples;

            const int numSamplesNow = hasEvent ? midiEventPos - startSample : numSamples;

            if (numSamplesNow > 0) {
                sink_.process(audioBuffer, startSample, numSamplesNow);
            }

            if (hasEvent) {
                polyphony_->handleMidiMessage(msg);
            }

            startSample += numSamplesNow;
            numSamples  -= numSamplesNow;
        }

        if (cpuMeter_->stop(totalSamples)) {
            polyphony_->monitorCpuMeterEvent(cpuMeter_->getPercent());
        }
    }


    //==============================================================================
    void Processor::getStateInformation(MemoryBlock&)
    {
        // You should use this method to store your parameters in the memory block.
        // You could do that either as raw data, or use the XML or ValueTree classes
        // as intermediaries to make it easy to save and load complex data.
    }

    void Processor::setStateInformation(const void*, int)
    {
        // You should use this method to restore your parameters from this memory block,
        // whose contents will have been created by the getStateInformation() call.
    }


    //------------------------------------------------------------------------------
    // Processing
    //------------------------------------------------------------------------------

    void Processor::suspend()
    {
        if (!isSuspended()) {
            suspendProcessing(true);
            if (instrument_) {
                instrument_->suspendModules();
            }
        }
    }


    void Processor::resume()
    {
        if (isSuspended()) {
            suspendProcessing(false);
            if (instrument_) {
                instrument_->resumeModules();
            }
        }
    }

} // namespace e3