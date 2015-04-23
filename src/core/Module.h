
#pragma once

#include <cstdint>
#include <string>

#include "core/GlobalHeader.h"
#include "core/Port.h"
#include "core/Link.h"

#pragma warning(push)
#pragma warning(disable: 4100) 


namespace e3 {

    class Module;
    class Outport;
    class Polyphony;

    // Pointer type to the audio processing function of a module.
    typedef void (Module::*ProcessFunctionPointer)(void) throw();

    enum ModuleType {
        kModuleUndefined = -1,
        // system modules
        kModuleMaster = 0,
        kModuleMidiGate = 1,
        kModuleMidiPitch = 2,
        kModuleMidiInput = 3,
        // audio
        kModuleSineOscil = 10,
        kModuleAdsrEnv = 11,
        kModuleDelay = 12,
    };


    enum ProcessingType
    {
        kProcessEvent = 1,
        kProcessControl = 2,
        kProcessAudio = 4

    };

    enum VoicingType
    {
        kMonophonic = 0,
        kPolyphonic = 1
    };

    enum ModuleStyle
    {
        kModuleStyleNothing = 0,
        kModuleStyleVisible = 1,
        kModuleStyleLabel = 2
    };

    
    //--------------------------------------------------------------------------
    // class Module
    // Abstract base class for Modules.
    //--------------------------------------------------------------------------

    class Module
    {
        friend class Instrument;

    protected:
        Module(
            ModuleType moduleType,
            const std::string& label,
            VoicingType voicingType,
            ProcessingType processingType,
            ModuleStyle style = (ModuleStyle)(kModuleStyleVisible | kModuleStyleLabel));

    public:
        virtual ~Module();

        void processAudio() throw() {}
        virtual void processEvent(double value, uint16_t voices) throw() {}
        virtual void processControl() throw () {}

        virtual void suspend() {}
        virtual void resume() {}

        virtual void setSampleRate(double sampleRate);
        virtual void setNumVoices(uint16_t numVoices);
        virtual void setParameter(uint16_t paramId, double value, double modulation = 0.f, int16_t voice = -1) {}
        const Parameter& getParameter(uint16_t parameterId) const;

        void addLink(Link& link);
        void removeLink(const Link& link);
        Link& getLink(uint16_t index);

        Inport* Module::getInport(uint16_t portId);
        Outport* getOutport(uint16_t portId);
        double* connectTargetWithSource(uint16_t portId);
        void disconnectTargetFromSource(uint16_t portId);

        virtual void onMidiController(int controllerNum, int value);

    public:
        std::string label_;
        int16_t id_ = -1;
        ModuleType moduleType_;
        VoicingType voicingType_;
        ProcessingType processingType_;
        ModuleStyle style_;

        ParameterMap parameters_;
        LinkList links_;
        LinkList removedLinks_;
        InportList inports_;
        OutportList outports_;

        ProcessFunctionPointer processFunction_ = nullptr;

    protected:
        // Constructs all member and initializes them with the current sample rate and number of voices.
        // No links are created.
        virtual void init(Polyphony* polyphony, double sampleRate);

        // Updates all members with the current sample rate and number of voices.
        // Links remain valid.
        virtual void update();

        // Deletes all members and links. After reset the module is in the same state as before init().
        virtual void reset();

        virtual void initSignals() {}
        virtual void initVoices() {}
        virtual void initParameters();

        virtual void resetSignals() {}
        virtual void disconnectPorts();
        virtual void resetData()  {}
        virtual void resetParameters()  {}

        virtual void updatePorts();
        virtual void updateInports();
        virtual void updateOutports();
        virtual void updateParameters();

        void addInport(uint16_t id, Inport* port);
        void addOutport(uint16_t id, Outport* port);
        void connectPort(Module* target, Link* link);
        VoiceAdapterType selectVoiceAdapter(VoicingType otherVoicingType) const;

        double sampleRate_   = INITIAL_SAMPLERATE;
        uint16_t numVoices_  = 0;
        bool mono_           = false;
        bool collapsed_      = false;

        Polyphony* polyphony_ = nullptr;
    };

    typedef std::vector<Module*> ModuleList;

} // namespace e3

#pragma warning(pop)
