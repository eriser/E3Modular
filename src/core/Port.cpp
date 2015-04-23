
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

    uint16_t Outport::addTarget(Link* link, VoiceAdapterType adapter)
    {
        targets_.push_back(link);
        numTargets_++;

        adapterBuffer_.resize(numTargets_);
        adapterBuffer_[numTargets_ - 1] = adapter;
        initModulation();

        return numTargets_;
    }


    int16_t Outport::removeTarget(Link* link)
    {
        int16_t idx = getIndex(link);
        VERIFY(idx >= 0);

        numTargets_--;
        targets_.erase(targets_.begin() + idx);

        adapterBuffer_.resize(numTargets_);
        initModulation();

        return idx;
    }


    void Outport::disconnectAll()
    {
        numTargets_ = 0;
        targets_.clear();
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
        int16_t idx = getIndex(link);
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

        for (uint16_t i = 0; i < targets_.size(); i++)
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
        
        for (uint16_t i = 0; i<targets_.size(); i++)
        {
            //Link* link = targets_[i];
            //if (link->scaleController(controllerId, value))
            //{
            //    link->value_ = value;
            //    setModulation(i, value);
            //}
        }
    }



    int16_t Outport::getIndex(Link* link)
    {
        for (uint16_t i = 0; i<targets_.size(); i++)
        {
            const Link& next = *targets_.at(i);
            if (next == link) {
                return i;
            }
        }
        return -1;
    }



    //-------------------------------------------------------
    // class AudioOutport
    //-------------------------------------------------------

    void AudioOutport::connect(Link* link, Module* target, VoiceAdapterType adapter)
    {
        VERIFY(target);
        addTarget(link, adapter);

        audioOutBuffer_.resize(numTargets_);
        audioOutBuffer_[numTargets_ - 1] = target->connectTargetWithSource(link->rightPort_);
    }


    void AudioOutport::disconnect(Link* link, Module* target)
    {
        VERIFY(target);

        int16_t idx = removeTarget(link);
        if (idx >= 0)
        {
            audioOutBuffer_.remove(idx);
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

    void EventInport::disconnectAll()
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

    void EventOutport::connect(Link* link, Module* target, VoiceAdapterType adapter)
    {
        VERIFY(target);
        addTarget(link, adapter);

        EventInport inport;
        inport.module_  = target;
        inport.paramId_ = link->rightPort_;

        inports_.resize(numTargets_);
        inports_.at(numTargets_ - 1) = inport;
    }


    void EventOutport::disconnect(Link* link, Module*)
    {
        int16_t idx = removeTarget(link);
        inports_.erase(inports_.begin() + idx);
    }


    void EventOutport::disconnectAll()
    {
        Outport::disconnect();
        inports_.clear();
    }


    bool EventOutport::isConnected()
    {
        return numTargets_ > 0;
    }


    void EventOutport::putEvent(double value, uint16_t voice)
    {
        for (uint16_t target = 0; target < numTargets_; target++)
        {
            double modulation        = modulationBuffer_[target * numVoices_ + voice];

            EventInport& inport      = inports_.at(target);
            Module* targetModule     = inport.module_;
            ASSERT(targetModule);

            VoiceAdapterType adapter = adapterBuffer_[target];
            __assume(adapter < 3);
            switch (adapter)
            {
            case kAdapterNone:
                targetModule->setParameter(inport.paramId_, value, modulation, voice);
                break;
            case kAdapterMonoToPoly:
                for (uint16_t i = 0; i<numVoices_; i++) {
                    targetModule->setParameter(inport.paramId_, value, modulation, i);
                }
                break;
            case kAdapterPolyToMono:
                targetModule->setParameter(inport.paramId_, value, modulation, 0);
                break;
            }
        }
    }


    //void EventOutport::putEvent(double value)
    //{
    //    for (uint16_t target = 0; target<numTargets_; target++)
    //    {
    //        for (uint16_t voice = 0; voice<numVoices_; voice++)
    //        {
    //            double modulation = modulationBuffer_[target*numVoices_ + voice];
    //            EventInport* inport = &operator[](target);
    //            Module* targetModule = inport->module_;
    //            ASSERT(targetModule);

    //            switch (adapterBuffer_[target])
    //            {
    //            case kAdapterNone:
    //            case kAdapterMonoToPoly:
    //                targetModule->setParameter(inport->paramId_, value, modulation, voice);
    //                break;
    //            case kAdapterPolyToMono:
    //                targetModule->setParameter(inport->paramId_, value, modulation, 0);
    //                voice = numVoices_;
    //                break;
    //            }
    //        }
    //    }
    //}
    
    
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