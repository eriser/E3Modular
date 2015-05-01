
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <e3_Buffer.h>
#include "core/GlobalHeader.h"
#include "core/Link.h"



namespace e3 {

    class Module;
    class Link;
    class Inport;
    class Outport;


    //------------------------------------------
    // typedefs
    //------------------------------------------

    enum PortType {
        PortTypeUndefined = 0,
        PortTypeInport    = 1,
        PortTypeOutport   = 2
    };


    enum PortAction {
        PortActionIdle             = 0,
        PortActionDocking          = 1,
        PortActionUndocking        = 2,
        PortActionContinueDocking  = 3,
        PortActionEndDocking       = 4
    };


    enum VoiceAdapterType {
        AdapterNone       = 0,
        AdapterMonoToPoly = 1,
        AdapterPolyToMono = 2
    };

    
    template <typename T>
    class PortList : public std::vector < T >
    {
    public: 
        void disconnect()
        {
            for (iterator it = begin(); it != end(); ++it)
            {
                T port = (*it);
                if (port != nullptr) {
                    port->disconnectAll();
                }
            }
        }

    };

    typedef PortList<Inport*> InportList;
    typedef PortList<Outport*> OutportList;


    //------------------------------------------
    // class Port
    // Base class for all Ports
    //------------------------------------------

    class Port
    {
    public:
        Port(PortType type) : type_(type)  {}
        virtual ~Port() {}

        virtual void disconnectAll() = 0;
        virtual bool isConnected() { return false;  }

        virtual void setNumVoices(uint16_t numVoices);

        void setId(int id)                      { id_ = id; }
        int getId()                             { return id_;  }

        PortType getType()                      { return type_; }
        const std::string& getLabel()           { return label_; }
        void setLabel(const std::string& label) { label_ = label; }

    protected:
        int id_                  = -1;
        uint16_t numVoices_      = 0;
        PortType type_           = PortTypeUndefined;
        ControlType controlType_ = ControlHidden;
        std::string label_;
    };


    //-----------------------------------------------------------------------
    // class Outport
    // Base class for module out ports
    //-----------------------------------------------------------------------
    
    class Outport : public Port
    {
    public:
        Outport() : Port(PortTypeOutport) {}

        virtual void connect(Module*, Link*, VoiceAdapterType) {}
        virtual void disconnect(Module*, Link*) {}

        void disconnectAll() override;
        bool isConnected() override;

        void setModulation(Link* link);
        void setNumVoices(uint16_t numVoices) override;

        void onGate(double gate, uint16_t voice);
        void onController(int16_t controllerId, double value);

        const LinkPointerList& getTargets() const  { return targets_; }

    protected:
        void addTarget(Link* link, VoiceAdapterType adapter);
        void removeTarget(Link* link);

        void setModulation(uint16_t targetId, double value, int16_t voice = -1);
        void initModulation();

        LinkPointerList targets_;
        size_t numTargets_ = 0;

        Buffer< VoiceAdapterType > adapterBuffer_;
        Buffer< double > modulationBuffer_;
    };


    //------------------------------------------------------------------------
    // class Inport
    // Base class for Inports
    //------------------------------------------------------------------------

    class Inport : public Port
    {
    public: 
        Inport() : Port(PortTypeInport)  {}

        virtual void connect()    {}
        virtual void disconnect() {}
    };



    //------------------------------------------------------------------------
    // class AudioOutport
    // Stores pointers to the input buffers of all connected AudioInports.
    //------------------------------------------------------------------------

    class AudioOutport : public Outport
    {
    public:
        void connect(Module* target, Link* link, VoiceAdapterType adapter) override;
        void disconnect(Module* target, Link* link) override;
        void disconnectAll() override;

        void __stdcall putValue(double value, uint16_t voice = 0) throw();

    protected:
        Buffer<double*> audioOutBuffer_;
    };


    __forceinline void __stdcall AudioOutport::putValue(double value, uint16_t voice) throw()
    {
        uint16_t modulationIndex = voice;

        for (uint16_t target = 0; target < numTargets_; target++)
        {
            double  val = value * modulationBuffer_[modulationIndex];     // apply modulation
            modulationIndex += numVoices_;

            double* ptr = audioOutBuffer_[target];	                      // get pointer to target

            VoiceAdapterType adapter = adapterBuffer_[target];
            __assume(adapter < 3);
            switch (adapter)
            {
            case AdapterNone:
                *(ptr + voice) += val; 	                     // add value to existing value, per voice
                break;
            case AdapterMonoToPoly:
                for (uint16_t i = 0; i < numVoices_; i++) {  // add value to all voices of target
                    *(ptr + i) += val;
                }
                break;
            case AdapterPolyToMono:
                *ptr += val; 	                              // add value only to voice 0
                break;
            }
        }
    }


    //--------------------------------------------------------------------------------------------
    // class EventInport
    // Has a pointer to a target module. EventOutport use this pointer to access the parameters
    // of the target module.
    //--------------------------------------------------------------------------------------------

    class EventInport : public Inport
    {
    public:
        Module* module_ = nullptr;
        int paramId_    = -1;

        void disconnectAll() override;
        bool isConnected() override;
    };


    
    //----------------------------------------
    // class EventOutport
    //----------------------------------------

    class EventOutport : public Outport
    {
    public:
        void connect(Module* module, Link* link, VoiceAdapterType adapter) override;
        void disconnect(Module* target, Link* link) override;
        void disconnectAll() override;

        void putEvent(double value, uint16_t voice);

    protected:
        std::vector< EventInport* > inports_;
    };


    //--------------------------------------------------------------------------------------------
    // class AudioInport
    // AudioInport has a buffer, where each element represents the value for a single voice.
    //--------------------------------------------------------------------------------------------

    class AudioInport : public Inport
    {
    public:
        void setNumVoices(uint16_t numVoices) override;

        void connect() override;
        void disconnect() override;
        void disconnectAll() override;
        bool isConnected() override;

        double* getBuffer() const;

    protected:
        Buffer< double > audioInBuffer_;
        int16_t numSources_ = 0;
    };
    
}  // namespace e3