
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <e3_Buffer.h>
#include "core/GlobalHeader.h"



namespace e3 {

    class Link;
    class Module;
    class Inport;
    class Outport;


    //------------------------------------------
    // typedefs
    //------------------------------------------

    enum PortType {
        kUndefined = 0,
        kInport = 1,
        kOutport = 2
    };


    enum VoiceAdapterType {
        kAdapterNone = 0,
        kAdapterMonoToPoly = 1,
        kAdapterPolyToMono = 2
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
        virtual ~Port() {}

        virtual void connect() {}
        virtual void disconnect()  {}
        virtual void disconnectAll() = 0;
        virtual void connect(Link*, Module*, VoiceAdapterType) {}
        virtual void disconnect(Link*, Module*) {}
        virtual bool isConnected() { return false;  }

        virtual void setNumVoices(uint16_t numVoices);

        uint16_t numVoices_      = 0;
        ControlType controlType_ = kControlHidden;
        std::string label_;
    };


    //-----------------------------------------------------------------------
    // class Outport
    // Base class for module out ports
    //-----------------------------------------------------------------------
    
    class Outport : public Port
    {
    public:
        void disconnectAll() override;
        bool isConnected() override;

        void setModulation(Link* link);
        void setNumVoices(uint16_t numVoices) override;

        void onGate(double gate, uint16_t voice);
        void onController(int16_t controllerId, double value);

    protected:
        uint16_t addTarget(Link* link, VoiceAdapterType adapter);
        int16_t removeTarget(Link* link);
        int16_t getIndex(Link* link);

        void setModulation(uint16_t targetId, double value, int16_t voice = -1);
        void initModulation();

        uint16_t numTargets_ = 0;
        std::vector< Link* > targets_;

        Buffer< VoiceAdapterType > adapterBuffer_;
        Buffer< double > modulationBuffer_;
    };


    //------------------------------------------------------------------------
    // class Inport
    // Base class for Inports
    //------------------------------------------------------------------------

    class Inport : public Port
    {};



    //------------------------------------------------------------------------
    // class AudioOutport
    // Stores pointers to the input buffers of all connected AudioInports.
    //------------------------------------------------------------------------

    class AudioOutport : public Outport
    {
    public:
        void connect(Link* link, Module* target, VoiceAdapterType adapter) override;
        void disconnect(Link* link, Module* target) override;
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
            case kAdapterNone:
                *(ptr + voice) += val; 	                     // add value to existing value, per voice
                break;
            case kAdapterMonoToPoly:
                for (uint16_t i = 0; i < numVoices_; i++) {  // add value to all voices of target
                    *(ptr + i) += val;
                }
                break;
            case kAdapterPolyToMono:
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
        Module* module_  = nullptr;
        int16_t paramId_ = -1;

        void disconnectAll() override;
        bool isConnected() override;
    };


    
    //----------------------------------------
    // class EventOutport
    //----------------------------------------

    class EventOutport : public Outport
    {
    public:
        void connect(Link* link, Module* target, VoiceAdapterType adapter) override;
        void disconnect(Link* link, Module* target) override;
        void disconnectAll() override;
        bool isConnected() override;

        void putEvent(double value, uint16_t voice);

    protected:
        std::vector< EventInport > inports_;
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