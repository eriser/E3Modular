
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
        PortTypeUndefined    = 0,
        PortTypeInport       = 1,
        PortTypeOutport      = 2,
        PortTypeAudio        = 4,
        PortTypeEvent        = 8,
        PortTypeAudioInport  = PortTypeInport  | PortTypeAudio,   // 5
        PortTypeAudioOutport = PortTypeOutport | PortTypeAudio,   // 6
        PortTypeEventInport  = PortTypeInport  | PortTypeEvent,   // 9
        PortTypeEventOutport = PortTypeOutport | PortTypeEvent    // 10
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

    //------------------------------------------
    // class ModulationBuffer
    //------------------------------------------

    class ModulationBuffer : public Buffer < double >
    {
    public:
        void init( int_fast32_t numVoices, LinkList& targets );
        void setValue( uint32_t targetIndex, double value, int voice );

        void onGate( uint32_t targetIndex, Link* link, double gate, int voice );
        void onController( uint32_t targetIndex, Link* link, int16_t controllerId, double value );

    protected:
        uint_fast32_t numVoices_;
    };


    typedef PortList<Inport*> InportList;
    typedef PortList<Outport*> OutportList;


    //---------------------------------------------------------
    // class Port
    // Abstract base class for all Inports and Outports
    //---------------------------------------------------------

    class Port
    {
    public:
        Port(PortType type) : type_(type)  {}
        virtual ~Port() {}

        virtual void disconnectAll() = 0;
        virtual uint32_t getNumConnections() const { return 0;  }

        virtual void setNumVoices(int numVoices);

        void setId(int id)                      { id_ = id; }
        int getId() const                       { return id_;  }

        PortType getType() const                { return type_; }
        void setProcessType(PortType type)      { type_ = (PortType)(type_ | type); }

        const std::string& getLabel()           { return label_; }
        void setLabel(const std::string& label) { label_ = label; }

    protected:
        int id_                  = -1;
        uint_fast32_t numVoices_ = 0;
        PortType type_           = PortTypeUndefined;
        ControlType controlType_ = ControlHidden;
        std::string label_;
    };


    //------------------------------------------
    // class Outport
    //------------------------------------------

    class Outport : public Port
    {
    public:
        Outport() : Port( PortTypeOutport ) {}

        void connect( Module* target, Link* link, VoiceAdapterType voiceAdapter );
        void disconnect( Module* target, Link* link );

        void disconnectAll() override;
        uint32_t getNumConnections() const override;
        
        void setNumVoices( int numVoices ) override;

        void __stdcall putAudio( double value, int_fast32_t voice = 0 ) throw();
        void putEvent( double value, int_fast32_t voice );

        void onGate( double gate, int voice );
        void onController( int16_t controllerId, double value );


    protected:
        void addAudioTarget( Link* link, VoiceAdapterType adapter );
        void addEventTarget( Link* link, VoiceAdapterType adapter );
        void removeAudioTarget( Link* link );
        void removeEventTarget( Link* link );

    protected:
        LinkList audioLinks_;
        Buffer< double* > audioOutBuffer_;
        Buffer< VoiceAdapterType > audioAdapterBuffer_;
        ModulationBuffer audioModulationBuffer_;
        uint_fast32_t numAudioTargets_ = 0;

        LinkList eventLinks_;
        InportList eventInports_;
        Buffer< VoiceAdapterType > eventAdapterBuffer_;
        ModulationBuffer eventModulationBuffer_;
        uint_fast32_t numEventTargets_ = 0;
    };


    class Inport : public Port
    {
        friend class Outport;

    public:
        Inport() : Port( PortTypeInport ) {}

        double* connectAudio();
        void connectEvent( Module* eventModule, int eventParamId );
        void disconnectAudio();
        void disconnectEvent();
        void disconnectAll() override;
        uint32_t getNumConnections() const override;

        void setNumVoices( int numVoices ) override;
        double* getAudioBuffer() const  { return audioInBuffer_; }

    protected:
        Buffer<double> audioInBuffer_;
        int numAudioSources_ = 0;

        Module* eventModule_;
        int eventParamId_    = -1;
        int numEventSources_ = 0;
    };


    __forceinline void __stdcall Outport::putAudio( double value, int_fast32_t voice ) throw()
    {
        int modulationIndex = voice;

        for (uint_fast32_t target = 0; target < numAudioTargets_; target++)
        {
            double  val = value * audioModulationBuffer_[modulationIndex];  // apply modulation
            modulationIndex += numVoices_;

            double* inportPointer = audioOutBuffer_[target];	            // get pointer to target
                                                                            
            VoiceAdapterType adapter = audioAdapterBuffer_[target];         
            __assume(adapter < 3);                                          
            switch (adapter)                                                
            {                                                               
            case AdapterNone:                                               
                *(inportPointer + voice) += val; 	                         // add value to existing value, per voice
                break;                                                      
            case AdapterMonoToPoly:                                         
                for (uint32_t i = 0; i < numVoices_; i++) {                  // add value to all voices of target
                    *(inportPointer + i) += val;
                }
                break;
            case AdapterPolyToMono:
                *inportPointer += val; 	                              // add value only to voice 0
                break;
            }
        }
    }


}  // namespace e3