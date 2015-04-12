
#include <stdexcept>
#include <e3_Exception.h>

#include <e3_Trace.h> // TODO remove this line

#include "core/GlobalHeader.h"
#include "core/ModelCatalog.h"
#include "core/Polyphony.h"
#include "modules/Master.h"
#include "modules/MidiModules.h"
#include "modules/Envelopes.h"
#include "modules/SineOscil.h"
#include "modules/Delay.h"



namespace e3 {

    Module::Module(ModuleType type) : 
        ModuleModel(ModelCatalog::instance().getModuleModel(type))
    {
        mono_ = (voicingType_ == kMonophonic);
    }


    Module::~Module()
    {
        reset();
    }


    Module* Module::createModule(ModuleType type)
    {
        switch (type)
        {
        case kModuleMaster:	   return new Master();
        case kModuleMidiGate:  return new MidiGate();
        case kModuleMidiPitch: return new MidiPitch();
        case kModuleMidiInput: return new MidiInput();
        case kModuleSineOscil: return new SineOscil();
        case kModuleAdsrEnv:   return new ADSREnv();
        case kModuleDelay:	   return new Delay();

        default: THROW(std::domain_error, "module type %d does not exist", type);
        }
    }


    void Module::init(Polyphony* polyphony)
    {
        polyphony_  = polyphony;

        initPorts();
        initProcess();
        initData();
        initParameters();
        initSignals();
    }


    void Module::reset()
    {
        resetSignals();
        resetPorts();
        resetData();
        resetParameters();

        sampleRate_      = 0;
        numVoices_       = 0;
        polyphony_       = nullptr;
        processFunction_ = nullptr;
    }


    void Module::resetPorts()
    {
        inPorts_.clear();
        outPorts_.clear();
    }


    void Module::update() 
    {
        ASSERT(sampleRate_ > 0);
        ASSERT(numVoices_ > 0 && numVoices_ < 1000); // debug only

        updateData();
        updateParameters();
        updatePorts();
    }


    void Module::updatePorts()
    {
        updateInPorts();
        updateOutPorts();
    }


    void Module::updateInPorts()
    {
        for (uint16_t i = 0; i < inPorts_.size(); i++) {
            inPorts_[i]->setNumVoices(numVoices_);
        }
    }


    void Module::updateOutPorts()
    {
        for (uint16_t i = 0; i< outPorts_.size(); i++) {
            outPorts_[i]->setNumVoices(numVoices_);
        }
    }


    void Module::initParameters()
    {
        for (ParameterModelMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            ParameterModel& param = it->second;
            if (param.midiShaper_.getControllerId() >= 0) {
                polyphony_->midiControllerSignal.Connect(this, &Module::onMidiController);
            }
        }
        updateParameters();
    }


    void Module::updateParameters()
    {
        for (ParameterModelMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            ParameterModel& param = it->second;
            setParameter(it->first, param.value_, 0, -1);  // set all parameters to zero
        }
    }


    void Module::onMidiController(int controllerNum, int value)
    {
        UNUSED(value);
        UNUSED(controllerNum);
        //TRACE("Module::onMidiController cc=%d\n", controllerNum);
    }

    
    void Module::setSampleRate(double sampleRate)
    {
        sampleRate_ = sampleRate;
    }


    void Module::setNumVoices(uint16_t numVoices)
   {
        numVoices_ = mono_ ? 1 : numVoices;
   }


    OutPort* Module::getOutPort(uint16_t portId)
    {
        return portId < outPorts_.size() ? outPorts_.at(portId) : nullptr;
    }


    double* Module::connect(uint16_t portId)
    {
        VERIFY(portId >= 0 && portId < inPorts_.size());
        VERIFY(numVoices_ > 0);

        AudioInPort* port = inPorts_.at(portId);
        double* ptr = port->connect();

        //checkPorts();
        return ptr;
    }


    void Module::disconnect(uint16_t portId)
    {
        VERIFY(portId >= 0 && portId < inPorts_.size());

        AudioInPort* port = inPorts_.at(portId);
        port->disconnect();
        updatePorts();
    }


    void Module::connectPort(Module* target, LinkModel* link)
    {
        ASSERT(target);

        if (target)
        {
            OutPort* port = getOutPort(link->rightPort_);
            ASSERT(port);

            if (port)
            {
                PortAdapterType adapter = chooseAdapter(target->voicingType_);
                port->connect(link, target, adapter);
            }
        }
    }


    PortAdapterType Module::chooseAdapter(VoicingType otherVoicingType) const
    {
        return voicingType_ == otherVoicingType ? kAdapterNone :
            voicingType_ == kMonophonic ? kAdapterMonoToPoly : kAdapterPolyToMono;
    }


} // namespace e3