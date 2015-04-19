
#include "e3_Exception.h"
#include "core/Ports.h"
#include "core/Module.h"


namespace e3 {

    //-------------------------------------------------------
    // class OutPort
    //-------------------------------------------------------

    uint16_t OutPort::addTarget(Link* link, PortAdapterType adapter)
    {
        targets_.push_back(link);
        numTargets_++;

        adapterBuffer_.resize(numTargets_);
        adapterBuffer_[numTargets_ - 1] = adapter;
        initModulation();

        return numTargets_;
    }


    int16_t OutPort::removeTarget(Link* link)
    {
        int16_t idx = getIndex(link);
        VERIFY(idx >= 0);

        numTargets_--;
        targets_.erase(targets_.begin() + idx);

        adapterBuffer_.resize(numTargets_);
        initModulation();

        return idx;
    }


    void OutPort::setNumVoices(uint16_t numVoices)
    {
        numVoices_ = numVoices;
        initModulation();
    }


    void OutPort::setModulation(uint16_t target, double value, int16_t voice)
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


    void OutPort::setModulation(Link* link)
    {
        int16_t idx = getIndex(link);
        ASSERT(idx >= 0);
        if (idx >= 0)
        {
            setModulation(idx, link->value_, -1);
        }
    }


    void OutPort::initModulation()
    {
        modulationBuffer_.set(0);
        modulationBuffer_.resize(numTargets_ * numVoices_);

        for (uint16_t i = 0; i < targets_.size(); i++)
        {
            Link* link = targets_[i];
            setModulation(i, link->value_);
        }
    }


    void OutPort::onGate(double gate, uint16_t voice)
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


    void OutPort::onController(int16_t controllerId, double value)
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



    int16_t OutPort::getIndex(Link* link)
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
    // class AudioOutPort
    //-------------------------------------------------------

    void AudioOutPort::connect(Link* link, Module* target, PortAdapterType adapter)
    {
        VERIFY(target);
        addTarget(link, adapter);

        audioOutBuffer_.resize(numTargets_);
        audioOutBuffer_[numTargets_ - 1] = target->connect(link->rightPort_);
    }


    void AudioOutPort::disconnect(Link* link, Module* target)
    {
        VERIFY(target);

        int16_t idx = removeTarget(link);
        if (idx >= 0)
        {
            audioOutBuffer_.remove(idx);
            target->disconnect(link->rightPort_);
        }
    }



    //-------------------------------------------------------
    // class AudioInPort
    //-------------------------------------------------------

    double* AudioInPort::setNumVoices(uint16_t numVoices)
    {
        numVoices_ = numVoices;
        audioInBuffer_.resize(numVoices_);

        return audioInBuffer_;
    }


    double* AudioInPort::connect()
    {
        numSources_++;

        VERIFY(audioInBuffer_);
        return audioInBuffer_;
    }


    void AudioInPort::disconnect()
    {
        VERIFY(numSources_ >= 1);
        numSources_--;
    }


    bool AudioInPort::isConnected()
    {
        VERIFY(numSources_ >= 0);
        return numSources_ > 0;
    }



    //-------------------------------------------------------
    // class EventOutPort
    //-------------------------------------------------------

    void EventOutPort::connect(Link* link, Module* target, PortAdapterType adapter)
    {
        VERIFY(target);

        EventTarget eventTarget;
        eventTarget.target_  = target;
        eventTarget.inputId_ = link->rightPort_;

        addTarget(link, adapter);

        resize(numTargets_);
        at(numTargets_ - 1) = eventTarget;
    }


    void EventOutPort::disconnect(Link* link, Module*)
    {
        int16_t idx = removeTarget(link);
        erase(begin() + idx);
    }


    void EventOutPort::putEvent(double value, uint16_t voice)
    {
        for (uint16_t target = 0; target < numTargets_; target++)
        {
            PortAdapterType adapter  = adapterBuffer_[target];
            double modulation        = modulationBuffer_[target*numVoices_ + voice];
            EventTarget* eventTarget = &operator[](target);
            Module* targetModule     = eventTarget->target_;
            ASSERT(targetModule);

            __assume(adapter < 3);
            switch (adapter)
            {
            case kAdapterNone:
                targetModule->setParameter(eventTarget->inputId_, value, modulation, voice);
                break;
            case kAdapterMonoToPoly:
                for (uint16_t i = 0; i<numVoices_; i++) {
                    targetModule->setParameter(eventTarget->inputId_, value, modulation, i);
                }
                break;
            case kAdapterPolyToMono:
                targetModule->setParameter(eventTarget->inputId_, value, modulation, 0);
                break;
            }
        }
    }


    void EventOutPort::putEvent(double value)
    {
        for (uint16_t target = 0; target<numTargets_; target++)
        {
            for (uint16_t voice = 0; voice<numVoices_; voice++)
            {
                double modulation        = modulationBuffer_[target*numVoices_ + voice];
                EventTarget* eventTarget = &operator[](target);
                Module* targetModule     = eventTarget->target_;
                ASSERT(targetModule);

                switch (adapterBuffer_[target])
                {
                case kAdapterNone:
                case kAdapterMonoToPoly:
                    targetModule->setParameter(eventTarget->inputId_, value, modulation, voice);
                    break;
                case kAdapterPolyToMono:
                    targetModule->setParameter(eventTarget->inputId_, value, modulation, 0);
                    voice = numVoices_;
                    break;
                }
            }
        }
    }
}  // namespace e3