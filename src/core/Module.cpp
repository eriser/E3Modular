
#include <e3_Exception.h>

#include "core/GlobalHeader.h"
#include "core/Polyphony.h"
#include "core/Module.h"



namespace e3 {

    Module::Module(
        ModuleType moduleType,
        const std::string& label,
        VoicingType voicingType,
        ProcessingType processingType,
        ModuleStyle style)
        :
        label_(label),
        moduleType_(moduleType),
        voicingType_(voicingType),
        processingType_(processingType),
        style_(style),
        mono_(voicingType_ == kMonophonic)
    {}


    Module::~Module()
    {
        outports_.clear();
        inports_.clear();
        reset();
    }


    void Module::init(Polyphony* polyphony, double sampleRate)
    {
        polyphony_  = polyphony;

        setSampleRate(sampleRate);
        setNumVoices(polyphony->getNumVoices());

        ASSERT(sampleRate_ > 0);
        ASSERT(numVoices_ > 0);

        initVoices();
        initParameters();
        initSignals();

        updateParameters();
        updatePorts();
    }


    void Module::reset()
    {
        resetSignals();
        disconnectPorts();
        resetData();
        resetParameters();

        sampleRate_      = 0;
        numVoices_       = 0;
        polyphony_       = nullptr;
        processFunction_ = nullptr;
    }


    void Module::disconnectPorts()
    {
        outports_.disconnect();
        inports_.disconnect();
    }


    void Module::update() 
    {
        ASSERT(sampleRate_ > 0);
        ASSERT(numVoices_ > 0 );

        initVoices();
        updateParameters();
        updatePorts();
    }


    void Module::updatePorts()
    {
        updateInports();
        updateOutports();
    }


    void Module::updateInports()
    {
        for (uint16_t i = 0; i < inports_.size(); i++) {
            inports_[i]->setNumVoices(numVoices_);
        }
    }


    void Module::updateOutports()
    {
        for (uint16_t i = 0; i< outports_.size(); i++) {
            outports_[i]->setNumVoices(numVoices_);
        }
    }


    void Module::initParameters()
    {
        for (ParameterMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            Parameter& param = it->second;
            if (param.midiShaper_.getControllerId() >= 0) {
                polyphony_->midiControllerSignal.Connect(this, &Module::onMidiController);
            }
        }
        updateParameters();
    }


    void Module::updateParameters()
    {
        for (ParameterMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            Parameter& param = it->second;
            setParameter(it->first, param.value_, 0, -1);  // set all parameters to zero
        }
    }


    const Parameter& Module::getParameter(int parameterId) const
    {
        return parameters_.at(parameterId);
    }


    //void Module::addLink(Link& link)
    //{
    //    if (removedLinks_.contains(link))
    //        removedLinks_.remove(link);

    //    link.rightModule_ = id_;
    //    links_.push_back(link);
    //}


    //void Module::removeLink(const Link& link)
    //{
    //    links_.remove(link);

    //    if (removedLinks_.contains(link)) {
    //        removedLinks_.remove(link);
    //    }
    //    removedLinks_.push_back(link);
    //}


    //Link& Module::getLink(uint16_t index)
    //{
    //    return links_.at(index);
    //}

    void Module::onMidiController(int, int)  {}

    
    void Module::setSampleRate(double sampleRate)
    {
        sampleRate_ = sampleRate;
    }


    void Module::setNumVoices(uint16_t numVoices)
    {
        numVoices_ = mono_ ? 1 : numVoices;
    }


    void Module::addInport(int id, const std::string& label, Inport* port)
    {
        port->setId(id);
        port->setLabel(label);
        inports_.insert(inports_.begin() + id, port);
    }


    void Module::addOutport(int id, const std::string& label, Outport* port)
    {
        port->setId(id);
        port->setLabel(label);
        outports_.insert(outports_.begin() + id, port);
    }


    Outport* Module::getOutport(int portId) const
    {
        return portId < (int)outports_.size() ? outports_.at(portId) : nullptr;
    }

    
    Inport* Module::getInport(int portId) const
    {
        return portId < (int)inports_.size() ? inports_.at(portId) : nullptr;
    }


    AudioOutport* Module::getAudioOutport(int portId) const
    {
        Outport* port = getOutport(portId);
        return dynamic_cast<AudioOutport*>(port);
    }


    AudioInport* Module::getAudioInport(int portId) const
    {
        Inport* port = getInport(portId);
        return dynamic_cast<AudioInport*>(port);
    }


    EventOutport* Module::getEventOutport(int portId) const
    {
        Outport* port = getOutport(portId);
        return dynamic_cast<EventOutport*>(port);
    }


    EventInport* Module::getEventInport(int portId) const
    {
        Inport* port = getInport(portId);
        return dynamic_cast<EventInport*>(port);
    }


    double* Module::connectTargetWithSource(int portId)
    {
        VERIFY(portId >= 0 && portId < (int)inports_.size());
        VERIFY(numVoices_ > 0);

        AudioInport* inport = dynamic_cast<AudioInport*>(inports_.at(portId));
        inport->connect();

        //checkPorts();
        return inport->getBuffer();
    }


    void Module::disconnectTargetFromSource(int portId)
    {
        VERIFY(portId >= 0 && portId < (int)inports_.size());

        AudioInport* inport = dynamic_cast<AudioInport*>(inports_.at(portId));
        inport->disconnect();

        updatePorts();
    }


    void Module::connectPort(Module* target, Link* link)
    {
        ASSERT(target);

        if (target)
        {
            Outport* outport = getOutport(link->leftPort_);
            ASSERT(outport);

            if (outport)
            {
                VoiceAdapterType adapter = selectVoiceAdapter(target->voicingType_);
                outport->connect(target, link, adapter);
            }
        }
    }


    VoiceAdapterType Module::selectVoiceAdapter(VoicingType otherVoicingType) const
    {
        return (voicingType_ == otherVoicingType) ? kAdapterNone :
            (voicingType_ == kMonophonic) ? kAdapterMonoToPoly : kAdapterPolyToMono;
    }


} // namespace e3