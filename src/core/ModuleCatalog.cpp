
#include <e3_Exception.h>
#include "ModuleCatalog.h"


namespace e3 
{
    ModuleCatalog::ModuleCatalog()
    {
        initMaster();
        initMaster();
        initMidiGate();
        initMidiPitch();
        initMidiInput();
        initSineOscillator();
        initAdsrEnvelope();
        initDelay();
    }

    const ModuleModel& ModuleCatalog::getModuleModel(ModuleType type) const
    { 
        return map_.at(type); 
    }


    const std::string& ModuleCatalog::getModuleLabel(ModuleType type) const
    {
        return getModuleModel(type).label_;
    }


    const ProcessingType& ModuleCatalog::getProcessingType(ModuleType type) const
    {
        return getModuleModel(type).processingType_;
    }

    const VoicingType& ModuleCatalog::getVoicingType(ModuleType type) const
    {
        return getModuleModel(type).voicingType_;
    }

    const BoxStyle& ModuleCatalog::getBoxStyle(ModuleType type) const
    {
        return getModuleModel(type).boxStyle_;
    }


    //const PortModel& ModuleCatalog::getInPortModel(ModuleType type, uint16_t portId) const
    //{
    //    return getModuleModel(type).inPortModels_.at(portId);
    //}


    //const PortModel& ModuleCatalog::getOutPortModel(ModuleType type, uint16_t portId) const
    //{
    //    return getModuleModel(type).outPortModels_.at(portId);
    //}


    const Parameter& ModuleCatalog::getParameter(ModuleType type, uint16_t parameterId) const
    {
        return getModuleModel(type).parameters_.at(parameterId);
    }


    void ModuleCatalog::initMaster()
    {
        ModuleModel module;

        module.moduleType_     = kModuleMaster;
        module.label_          = "Master";
        module.processingType_ = kProcessAudio;
        module.voicingType_    = kMonophonic;
        module.boxStyle_       = (BoxStyle)(kBoxVisible | kBoxLabel);

        PortModel portIn;
        portIn.id_           = 0;
        portIn.label_        = "In";
        module.inPortModels_.push_back(portIn);

        Parameter paramVolume;
        paramVolume.controlType_  = kControlSlider;
        paramVolume.defaultValue_ = 0.35;
        paramVolume.id_           = 0;
        paramVolume.label_        = "Volume";
        paramVolume.numberFormat_ = kNumberDecibel;
        paramVolume.unit_         = "db";
        module.parameters_.add(paramVolume);

        map_.insert(std::make_pair(module.moduleType_, module));
    }

    void ModuleCatalog::initMidiGate()
    {
        ModuleModel module;

        module.moduleType_     = kModuleMidiGate;
        module.label_          = "Midi Gate";
        module.processingType_ = kProcessEvent;
        module.voicingType_    = kPolyphonic;
        module.boxStyle_       = (BoxStyle)(kBoxVisible | kBoxLabel);

        PortModel portGate;
        portGate.id_    = 0;
        portGate.label_ = "Gate";
        module.outPortModels_.push_back(portGate);

        map_.insert(std::make_pair(module.moduleType_, module));
    }

    void ModuleCatalog::initMidiPitch()
    {
        ModuleModel module;

        module.moduleType_     = kModuleMidiPitch;
        module.label_          = "Midi Pitch";
        module.processingType_ = (ProcessingType)(kProcessEvent | kProcessControl);

        PortModel port;
        port.id_    = 0;
        port.label_ = "Pitch";
        module.outPortModels_.push_back(port);
  
        Parameter paramBend;
        paramBend.id_           = 0;
        paramBend.label_        = "Bend Range";
        paramBend.controlType_  = kControlBiSlider;
        paramBend.defaultValue_ = 2;
        paramBend.valueShaper_  = { -48, 48, 96 };
        module.parameters_.add(paramBend);

        Parameter paramTime;
        paramTime.id_           = 1;
        paramTime.label_        = "Portamento Time";
        paramTime.controlType_  = kControlSlider;
        paramTime.defaultValue_ = 0;
        paramTime.valueShaper_  = { 0, 2000 };
        paramTime.unit_         = "msec";
        paramTime.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramTime);

        Parameter paramAuto;
        paramAuto.id_           = 2;
        paramAuto.label_        = "Portamento Auto";
        paramAuto.controlType_  = kControlCheckbox;
        paramAuto.defaultValue_ = 0;
        paramBend.valueShaper_ = { 0, 1 };
        module.parameters_.add(paramAuto);

        map_.insert(std::make_pair(module.moduleType_, module));
    }

    void ModuleCatalog::initMidiInput()
    {
        ModuleModel module;

        module.moduleType_     = kModuleMidiInput;
        module.label_          = "Midi Input";
        module.processingType_ = (ProcessingType)(kProcessEvent | kProcessControl);

        PortModel portPitch;
        portPitch.id_    = 0;
        portPitch.label_ = "Pitch";
        module.outPortModels_.push_back(portPitch);

        PortModel portGate;
        portGate.id_    = 1;
        portGate.label_ = "Gate";
        module.outPortModels_.push_back(portGate);

        Parameter paramBend;
        paramBend.id_           = 0;
        paramBend.label_        = "Portamento Auto";
        paramBend.controlType_  = kControlCheckbox;
        paramBend.defaultValue_ = 0;
        paramBend.valueShaper_  = { 0, 48, 96 };
        module.parameters_.add(paramBend);

        Parameter paramTime;
        paramTime.id_           = 1;
        paramTime.label_        = "Portamento Time";
        paramTime.controlType_  = kControlSlider;
        paramTime.defaultValue_ = 0;
        paramTime.valueShaper_  = { 0, 2000, 100, 12 };
        paramTime.unit_         = "msec";
        paramTime.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramTime);

        Parameter paramAuto;
        paramAuto.id_           = 2;
        paramAuto.label_        = "Portamento Auto";
        paramAuto.controlType_  = kControlCheckbox;
        paramAuto.defaultValue_ = 0;
        paramAuto.valueShaper_  = { 0, 1, 1 };
        module.parameters_.add(paramAuto);

        map_.insert(std::make_pair(module.moduleType_, module));
    }

    void ModuleCatalog::initSineOscillator()
    {
        ModuleModel module;

        module.moduleType_     = kModuleSineOscil;
        module.label_          = "Sine";
        module.processingType_ = kProcessAudio;

        PortModel portFreq;
        portFreq.id_          = 0;
        portFreq.label_       = "Freq";
        portFreq.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portFreq);

        PortModel portAmp;
        portAmp.id_          = 1;
        portAmp.label_       = "Ampl";
        portAmp.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portAmp);

        PortModel portOut;
        portOut.id_          = 1;
        portOut.label_       = "Ampl";
        portOut.controlType_ = kControlSlider;
        module.outPortModels_.push_back(portOut);

        Parameter paramFreq;
        paramFreq.id_          = 0;
        paramFreq.label_       = "Frequency";
        paramFreq.controlType_ = kControlHidden;
        module.parameters_.add(paramFreq);

        Parameter paramAmp;
        paramAmp.id_          = 1;
        paramAmp.label_       = "Frequency";
        paramAmp.controlType_ = kControlHidden;
        module.parameters_.add(paramAmp);

        Parameter paramTune;
        paramTune.id_           = 2;
        paramTune.label_        = "Tune";
        paramTune.controlType_  = kControlBiSlider;
        paramTune.defaultValue_ = 0;
        paramTune.valueShaper_  = { -48, 48, 96 };
        module.parameters_.add(paramTune);

        Parameter paramFinetune;
        paramFinetune.id_           = 3;
        paramFinetune.label_        = "Finetune";
        paramFinetune.controlType_  = kControlBiSlider;
        paramFinetune.defaultValue_ = 0;
        paramFinetune.valueShaper_  = { -1, 1, 200 };
        module.parameters_.add(paramFinetune);
 
        map_.insert(std::make_pair(module.moduleType_, module));
    }


    void ModuleCatalog::initAdsrEnvelope()
    {
        ModuleModel module;

        module.moduleType_     = kModuleAdsrEnv;
        module.label_          = "ADSR";
        module.processingType_ = (ProcessingType)(kProcessAudio | kProcessControl);

        PortModel portIn;
        portIn.id_          = 0;
        portIn.label_       = "In";
        portIn.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portIn);

        PortModel portGate;
        portGate.id_          = 0;
        portGate.label_       = "Gate";
        portGate.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portGate);

        PortModel portOut;
        portOut.id_          = 0;
        portOut.label_       = "Out";
        portOut.controlType_ = kControlSlider;
        module.outPortModels_.push_back(portOut);

        Parameter paramIn;
        paramIn.id_          = 0;
        paramIn.label_       = "Frequency";
        paramIn.controlType_ = kControlHidden;
        module.parameters_.add(paramIn);

        Parameter paramAmp;
        paramAmp.id_          = 1;
        paramAmp.label_       = "Frequency";
        paramAmp.controlType_ = kControlHidden;
        module.parameters_.add(paramAmp);
   
        Parameter paramAttack;
        paramAttack.id_           = 2;
        paramAttack.label_        = "Attack";
        paramAttack.controlType_  = kControlSlider;
        paramAttack.defaultValue_ = 0.1;
        paramAttack.valueShaper_  = { 0, 1.2, 96, 6 };
        paramAttack.unit_         = "sec";
        paramAttack.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramAttack);

        Parameter paramDecay;
        paramDecay.id_           = 3;
        paramDecay.label_        = "Decay";
        paramDecay.controlType_  = kControlSlider;
        paramDecay.defaultValue_ = 0;
        paramDecay.valueShaper_  = { 0, 1, 100, 6 };
        paramDecay.unit_         = "sec";
        paramDecay.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramDecay);

        Parameter paramSustain;
        paramSustain.id_           = 4;
        paramSustain.label_        = "Sustain";
        paramSustain.controlType_  = kControlSlider;
        paramSustain.defaultValue_ = 0.75;
        paramSustain.valueShaper_  = { 0, 1, 100, 6 };
        paramSustain.unit_         = "db";
        paramSustain.numberFormat_ = kNumberDecibel;
        module.parameters_.add(paramSustain);

        Parameter paramRelease;
        paramRelease.id_           = 5;
        paramRelease.label_        = "Attack";
        paramRelease.controlType_  = kControlSlider;
        paramRelease.defaultValue_ = 0;
        paramRelease.valueShaper_  = { 0, 1, 100, 6 };
        paramRelease.unit_         = "sec";
        paramRelease.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramRelease);

        map_.insert(std::make_pair(module.moduleType_, module));
    }


    void ModuleCatalog::initDelay()
    {
        ModuleModel module;

        module.moduleType_     = kModuleDelay;
        module.label_          = "Delay";
        module.processingType_ = kProcessAudio;
  
        PortModel portIn;
        portIn.id_          = 0;
        portIn.label_       = "Out";
        portIn.controlType_ = kControlHidden;
        module.inPortModels_.push_back(portIn);

        PortModel portTime;
        portTime.id_          = 1;
        portTime.label_       = "Time";
        portTime.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portTime);

        PortModel portRepeats;
        portRepeats.id_          = 2;
        portRepeats.label_       = "Repeats";
        portRepeats.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portRepeats);

        PortModel portGain;
        portGain.id_          = 3;
        portGain.label_       = "Gain";
        portGain.controlType_ = kControlSlider;
        module.inPortModels_.push_back(portGain);

        PortModel portOut;
        portOut.id_               = 0;
        portOut.label_            = "Gain";
        portOut.controlType_      = kControlHidden;
        module.outPortModels_.push_back(portOut);
    
        Parameter paramTime;
        paramTime.id_           = 1;                  // TODO: check why this is not zero
        paramTime.label_        = "Time";
        paramTime.controlType_  = kControlSlider;     // TODO: idea: make this bislider with reverse delay
        paramTime.defaultValue_ = 0.5;
        paramTime.unit_         = "sec";
        paramTime.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramTime);

        Parameter paramRepeats;
        paramRepeats.id_           = 2;
        paramRepeats.label_        = "Repeats";
        paramRepeats.controlType_  = kControlSlider;
        paramRepeats.defaultValue_ = 0.5;
        paramRepeats.unit_         = "sec";
        paramRepeats.numberFormat_ = kNumberFloat;
        module.parameters_.add(paramRepeats);

        Parameter paramGain;
        paramGain.id_           = 3;
        paramGain.label_        = "Gain";
        paramGain.controlType_  = kControlSlider;
        paramGain.defaultValue_ = 0.5;
        paramGain.unit_         = "db";
        paramGain.numberFormat_ = kNumberDecibel;
        module.parameters_.add(paramGain);
    
        map_.insert(std::make_pair(module.moduleType_, module));
    }

} // namespace e3
