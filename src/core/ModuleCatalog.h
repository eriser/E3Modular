
#pragma once

#include "GlobalHeader.h"
#include "core/ModuleBase.h"


namespace e3 {

    //------------------------------------------------------------------------------------
    // Provides static information about modules, their parameters and ports.
    // This is used to build the modules and to retrieve module properties at runtime.
    //
    class ModuleCatalog 
    {
    public:
        static const ModuleCatalog& instance()  {
            static ModuleCatalog _instance;
            return _instance;   
        }          

    private:                       
        ModuleCatalog();

    public:
        const ModuleModel& getModuleModel(ModuleType type) const;
        const std::string& getModuleLabel(ModuleType type) const;
        const ProcessingType& getProcessingType(ModuleType type) const;
        const VoicingType& getVoicingType(ModuleType type) const;
        const BoxStyle& getBoxStyle(ModuleType type) const;

        //const PortModel& getInPortModel(ModuleType type, uint16_t portId) const;
        //const PortModel& getOutPortModel(ModuleType type, uint16_t portId) const;

        const Parameter& getParameter(ModuleType type, uint16_t parameterId) const;

    private:
        void initMaster();
        void initMidiGate();
        void initMidiPitch();
        void initMidiInput();
        void initSineOscillator();
        void initAdsrEnvelope();
        void initDelay();

        typedef std::map< ModuleType, ModuleModel > Map;
        Map map_;
    };

} // namespace e3



