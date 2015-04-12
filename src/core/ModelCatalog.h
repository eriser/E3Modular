
#pragma once

#include "GlobalHeader.h"
#include "core/Models.h"


namespace e3 {

    //------------------------------------------------------------------------------------
    // Provides static information about modules, their parameters and ports.
    // This is used to build the modules and to retrieve module properties at runtime.
    //
    class ModelCatalog 
    {
    public:
        static const ModelCatalog& instance()  {                          
            static ModelCatalog _instance;   
            return _instance;   
        }          
        //static ModelCatalog& instance();

    private:                       
        ModelCatalog();  
        //ModelCatalog(const ModelCatalog&);

    public:
        const ModuleModel& getModuleModel(ModuleType type) const;
        const std::string& getModuleLabel(ModuleType type) const;
        const ProcessingType& getProcessingType(ModuleType type) const;
        const VoicingType& getVoicingType(ModuleType type) const;
        const BoxStyle& getBoxStyle(ModuleType type) const;

        const PortModel& getInPortModel(ModuleType type, uint16_t portId) const;
        const PortModel& getOutPortModel(ModuleType type, uint16_t portId) const;

        const ParameterModel& getParameterModel(ModuleType type, uint16_t parameterId) const;
        //void buildParameterModelMap(ModuleType moduleType, ParameterModelMap& map, bool makeDefault = false) const;
        //ModuleModelPtr createModuleModel(ModuleType type) const;

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

    //ModelCatalog modelCatalogInstance_;

    //inline const ModelCatalog& ModelCatalog::instance()
    //{
    //    return modelCatalogInstance_;
    //}


} // namespace e3



