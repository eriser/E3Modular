
#include "e3_Exception.h"
#include "core/Port.h"
#include "core/Link.h"
#include "core/Module.h"


namespace e3 {


    //-------------------------------------------------------
    // class Port
    //-------------------------------------------------------

    void Port::setNumVoices(uint16_t numVoices)
    {
        numVoices_ = numVoices;
    }


    //-------------------------------------------------------
    // class Outport
    //-------------------------------------------------------

    void Outport::addTarget(Link* link, VoiceAdapterType adapter)
    {
        numTargets_ = targets_.add(link);

        adapterBuffer_.resize(numTargets_);
        adapterBuffer_[numTargets_ - 1] = adapter;
        initModulation();
    }


    void Outport::removeTarget(Link* link)
    {
        numTargets_ = targets_.remove(link);

        adapterBuffer_.resize(numTargets_);
        initModulation();
    }


    void Outport::disconnectAll()
    {
        targets_.clear();
        numTargets_ = 0;

        adapterBuffer_.clear();
        modulationBuffer_.clear();
    }


    bool Outport::isConnected()
    {
        return numTargets_ > 0;
    }


    void Outport::setNumVoices(uint16_t numVoices)
    {
        Port::setNumVoices(numVoices);
        initModulation();
    }


    void Outport::setModulation(uint16_t target, double value, int16_t voice)
    {
        if (voice == -1)    // modulate all voices
        {
            for (uint16_t i = 0; i<numVoices_; i++) {
                modulationBuffer_[target * numVoices_ + i] = value;
            }
        }
        else {              // modulate all voices
            voice = std::min<uint16_t>(numVoices_ - 1, voice);  // TODO: make sure numVoices_ > 0
            modulationBuffer_[target * numVoices_ + voice] = value;
        }
    }


    void Outport::setModulation(Link* link)
    {
        int16_t idx = targets_.getIndex(link);
        ASSERT(idx >= 0);
        if (idx >= 0)
        {
            setModulation(idx, link->value_, -1);
        }
    }


    void Outport::initModulation()
    {
        modulationBuffer_.set(0);
        modulationBuffer_.resize(numTargets_ * numVoices_);

        for (uint16_t i = 0; i < numTargets_; i++)
        {
            Link* link = targets_[i];
            setModulation(i, link->value_);
        }
    }


    void Outport::onGate(double gate, uint16_t voice)
    {
        for (uint16_t i = 0; i<targets_.size(); i++)
        {
            Link* link = targets_[i];
            if (link->veloSens_)
            {
                double value = link->valueShaper_.exponential(link->value_);
                gate *= link->veloSens_;
                value += gate;
                setModulation(i, value, voice);
            }
        }
    }


    void Outport::onController(int16_t controllerId, double value)
    {
        value = value;
        controllerId = controllerId;
        
        for (uint16_t i = 0; i < numTargets_; i++)
        {
            //Link* link = targets_[i];
            //if (link->scaleController(controllerId, value))
            //{
            //    link->value_ = value;
            //    setModulation(i, value);
            //}
        }
    }


    //-------------------------------------------------------
    // class AudioOutport
    //-------------------------------------------------------

    void AudioOutport::connect(Module* target, Link* link, VoiceAdapterType adapter)
    {
        AudioInport* inport = target->getAudioInport(link->rightPort_);
        ASSERT(inport != nullptr);

        if (inport != nullptr) 
        {
            addTarget(link, adapter);
            audioOutBuffer_.resize(numTargets_);
            audioOutBuffer_[numTargets_ - 1] = inport->getBuffer();
        }
    }


    void AudioOutport::disconnect(Module* target, Link* link)
    {
        ASSERT(target);
        ASSERT(link);

        int16_t index = targets_.getIndex(link);
        removeTarget(link);
        ASSERT(index >= 0);

        if (index >= 0)
        {
            audioOutBuffer_.remove(index);
            target->disconnectTargetFromSource(link->rightPort_);
        }
    }


    void AudioOutport::disconnectAll()
    {
        Outport::disconnectAll();
        audioOutBuffer_.clear();
    }


    //-------------------------------------------------------
    // class EventInport
    //-------------------------------------------------------

    void EventInport::disconnectAll()  // TODO: handle multiple connections
    {
        module_  = nullptr;
        paramId_ = -1;
    }


    bool EventInport::isConnected()
    {
        return module_ != nullptr;
    }

    
    //-------------------------------------------------------
    // class EventOutport
    //-------------------------------------------------------

    void EventOutport::connect(Module* target, Link* link, VoiceAdapterType adapter)
    {
        EventInport* inport = target->getEventInport(link->leftPort_);
        ASSERT(inport);
        ASSERT(link);

        if (inport != nullptr)
        {
            addTarget(link, adapter);

            inport->module_  = target;
            inport->paramId_ = link->rightPort_;

            inports_.resize(numTargets_);
            inports_.at(numTargets_ - 1) = inport;
        }
    }


    void EventOutport::disconnect(Module*, Link* link)
    {
        int16_t index = targets_.getIndex(link);
        ASSERT(index >= 0);
        removeTarget(link);

        if (index >= 0) {
            inports_.erase(inports_.begin() + index);
        }
    }


    void EventOutport::disconnectAll()
    {
        Outport::disconnectAll();
        inports_.clear();
    }


    void EventOutport::putEvent(double value, uint16_t voice)
    {
        for (uint16_t targetIndex = 0; targetIndex < numTargets_; targetIndex++)
        {
            double modulation    = modulationBuffer_[targetIndex * numVoices_ + voice];

            EventInport* inport  = inports_.at(targetIndex);
            Module* targetModule = inport->module_;
            ASSERT(targetModule);

            VoiceAdapterType adapter = adapterBuffer_[targetIndex];
            __assume(adapter < 3);
            switch (adapter)
            {
            case kAdapterNone:
                targetModule->setParameter(inport->paramId_, value, modulation, voice);
                break;
            case kAdapterMonoToPoly:
                for (uint16_t i = 0; i<numVoices_; i++) {
                    targetModule->setParameter(inport->paramId_, value, modulation, i);
                }
                break;
            case kAdapterPolyToMono:
                targetModule->setParameter(inport->paramId_, value, modulation, 0);
                break;
            }
        }
    }


    //-------------------------------------------------------
    // class AudioInport
    //-------------------------------------------------------

    void AudioInport::setNumVoices(uint16_t numVoices)
    {
        Port::setNumVoices(numVoices);
        audioInBuffer_.resize(numVoices_);
    }


    double* AudioInport::getBuffer() const
    {
        return audioInBuffer_;
    }


    void AudioInport::connect()
    {
        numSources_++;

        VERIFY(audioInBuffer_ && audioInBuffer_.size() == numVoices_);
    }


    void AudioInport::disconnect()
    {
        VERIFY(numSources_ >= 1);
        numSources_--;
    }


    void AudioInport::disconnectAll()
    {
        numSources_ = 0;
    }


    bool AudioInport::isConnected()
    {
        VERIFY(numSources_ >= 0);
        return numSources_ > 0;
    }

}  // namespace e3