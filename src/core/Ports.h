
#pragma once

#include <vector>
#include <e3_Buffer.h>
#include "core/ModuleBase.h"



namespace e3 {

    class Module;
    class OutPort;
    class AudioInPort;
    class AudioOutPort;
    class EventOutPort;


    //------------------------------------------
    // typedefs
    //------------------------------------------

    typedef std::vector< AudioInPort* > InPortList;
    typedef std::vector< OutPort* > OutPortList;
    typedef Buffer< double* > AudioOutBuffer;

    struct EventTarget
    {
        Module* target_;
        uint16_t inputId_;
    };
    typedef std::vector< EventTarget > EventOutBuffer;


    enum PortAdapterType {
        kAdapterNone    = 0,
        kAdapterMonoToPoly = 1,
        kAdapterPolyToMono = 2
    };


    //-----------------------------------------------------------------------
    // class OutPort
    // Abstract base class for module out ports
    //-----------------------------------------------------------------------
    
    class OutPort
    {
    public:
        virtual ~OutPort() {}

        virtual void connect(Link* link, Module* target, PortAdapterType adapter) = 0;
        virtual void disconnect(Link* link, Module* target) = 0;

        void setModulation(Link* link);
        void setNumVoices(uint16_t numVoices);

        void onGate(double gate, uint16_t voice);
        void onController(int16_t controllerId, double value);

    protected:
        uint16_t addTarget(Link* link, PortAdapterType adapter);
        int16_t removeTarget(Link* link);
        int16_t getIndex(Link* link);

        void setModulation(uint16_t targetId, double value, int16_t voice = -1);
        void initModulation();

        uint16_t numVoices_ = 0;
        uint16_t numTargets_ = 0;
        std::vector< Link* > targets_;

        Buffer< PortAdapterType > adapterBuffer_;
        Buffer< double > modulationBuffer_;
    };


    //------------------------------------------------------------------------
    // class AudioOutPort
    // Stores pointers to the input buffers of all connected AudioInPorts.
    //------------------------------------------------------------------------

    class AudioOutPort : public OutPort
    {
    public:
        void connect(Link* link, Module* target, PortAdapterType adapter);
        void disconnect(Link* link, Module* target);

        void __stdcall putAudio(double value, uint16_t voice = 0) throw();

    protected:
        AudioOutBuffer audioOutBuffer_;
    };


    __forceinline void __stdcall AudioOutPort::putAudio(double value, uint16_t voice) throw()
    {
        uint16_t modulationIndex = voice;

        for (uint16_t target = 0; target < numTargets_; target++)
        {
            double  val = value * modulationBuffer_[modulationIndex];     // apply modulation
            modulationIndex += numVoices_;

            double* ptr = audioOutBuffer_[target];	                      // get pointer to target

            PortAdapterType adapter = adapterBuffer_[target];
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
    // class AudioInPort
    // An audio in port is a buffer, where each element represents the value for a single voice.
    //--------------------------------------------------------------------------------------------

    class AudioInPort
    {
    public:
        double* setNumVoices(uint16_t numVoices);

        double* connect();
        void disconnect();
        bool isConnected();

    protected:
        Buffer< double > audioInBuffer_;

        uint16_t numVoices_ = 0;
        int16_t numSources_ = 0;
    };


    //----------------------------------------
    // class EventOutPort
    //----------------------------------------

    class EventOutPort : public OutPort, public EventOutBuffer
    {
    public:
        void connect(Link* link, Module* target, PortAdapterType adapter);
        void disconnect(Link* link, Module* target);
        void putEvent(double value, uint16_t voice);
        void putEvent(double value);
    };
}  // namespace e3