
#pragma once

#include "core/GlobalHeader.h"
#include "core/Ports.h"
#include "core/ModuleBase.h"

#pragma warning(push)
#pragma warning(disable: 4100) 


namespace e3 {


    class Module;
    class OutPort;
    class Polyphony;

    // Pointer type to the audio processing function of a module.
    typedef void (Module::*ProcessFunctionPointer)(void) throw();


    class Module : public ModuleModel
    {
        friend class Instrument;

    public:
        Module(ModuleType type);
        virtual ~Module();

        static Module* createModule(ModuleType moduleType);

    public:
        void processAudio() throw() {}
        virtual void processEvent(double value, uint16_t voices) throw() {}
        virtual void processControl() throw () {}

        virtual void suspend() {}
        virtual void resume() {}

        virtual void setSampleRate(double sampleRate);
        virtual void setNumVoices(uint16_t numVoices);
        virtual void setParameter(uint16_t paramId, double value, double modulation = 0.f, int16_t voice = -1) {}

        const InPortList& getInPorts() const    { return inPorts_; }
        const OutPortList& getOutPorts() const  { return outPorts_; }

        OutPort* getOutPort(uint16_t portId);
        double* connect(uint16_t portId);
        void disconnect(uint16_t portId);

        virtual void onMidiController(int controllerNum, int value);

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
        virtual void initProcess() {}
        virtual void initPorts() {}
        virtual void initVoices() {}
        virtual void initParameters();

        virtual void resetSignals() {}
        virtual void resetPorts();
        virtual void resetData()  {}
        virtual void resetParameters()  {}

        virtual void updatePorts();
        virtual void updateInPorts();
        virtual void updateOutPorts();
        virtual void updateParameters();

        void connectPort(Module* target, Link* link);
        PortAdapterType chooseAdapter(VoicingType otherVoicingType) const;

        double sampleRate_   = INITIAL_SAMPLERATE;
        uint16_t numVoices_  = 0;
        bool mono_           = false;

        InPortList inPorts_;
        OutPortList outPorts_;

        Polyphony* polyphony_;
    };

    typedef std::vector<Module*> ModuleList;
} // namespace e3

#pragma warning(pop)
