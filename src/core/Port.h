
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <e3_Buffer.h>
#include "core/GlobalHeader.h"
#include "core/Link.h"
#include "core/Parameter.h"


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
        PortTypeAudioInport  = PortTypeInport | PortTypeAudio,   // 5
        PortTypeAudioOutport = PortTypeOutport | PortTypeAudio,   // 6
        PortTypeEventInport  = PortTypeInport | PortTypeEvent,   // 9
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


    //------------------------------------------
    // class PortData
    //------------------------------------------

    class PortData : public Link, public Parameter
    {
    public:
        PortData() {}
        PortData( const Link& link, const Parameter& parameter ) : Link( link ), Parameter( parameter ) {}
    };

    typedef std::vector<PortData> PortDataList;


    //------------------------------------------
    // class ModulationBuffer
    //------------------------------------------

    class ModulationBuffer : public Buffer < double >
    {
    public:
        void init( int_fast32_t numVoices, PortDataList& data );
        void setValue( uint32_t targetIndex, double value, int voice = -1 );

        void onGate( uint32_t targetIndex, PortData& data, double gate, int voice );
        void onController( uint32_t targetIndex, PortData& data, int16_t controllerId, double value );

    protected:
        uint_fast32_t numVoices_;
    };


    //---------------------------------------------------------
    // class PortList
    // A container for ports based on std::vector
    //---------------------------------------------------------

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


    //---------------------------------------------------------
    // class Port
    // Abstract base class for all Inports and Outports
    //---------------------------------------------------------

    class Port
    {
    public:
        Port( PortType type ) : type_( type )  {}
        virtual ~Port() {}

        virtual void disconnectAll();

        virtual int getNumConnections() const      { return getNumAudioConnections() + getNumEventConnections(); }
        virtual int getNumAudioConnections() const { return numAudioConnections_; }
        virtual int getNumEventConnections() const { return numEventConnections_; }

        virtual void setNumVoices( int numVoices ) { numVoices_ = (uint_fast32_t)numVoices; }
        virtual int getNumVoices() const           { return (int)numVoices_; }

        void setId( int id )                       { id_ = id; }
        int getId() const                          { return id_; }

        PortType getType() const                   { return type_; }
        void setProcessType( PortType type )       { type_ = (PortType)(type_ | type); }

        const std::string& getLabel()              { return label_; }
        void setLabel( const std::string& label )  { label_ = label; }

    protected:
        int id_                           = -1;
        int_fast32_t numVoices_           = 0;
        int_fast32_t numAudioConnections_ = 0;
        int_fast32_t numEventConnections_ = 0;
        PortType type_                    = PortTypeUndefined;
        ControlType controlType_          = ControlHidden;
        std::string label_;
    };


    //------------------------------------------
    // class Outport
    //------------------------------------------

    class Outport : public Port
    {
    public:
        Outport() : Port( PortTypeOutport ) {}

        void connect( Module* target, const PortData& data, VoiceAdapterType voiceAdapter );
        void disconnectAll() override;

        void setNumVoices( int numVoices ) override;
        bool setParameter( const Parameter& parameter );

        void __stdcall putAudio( double value, int_fast32_t voice = 0 ) throw();
        void putEvent( double value, int_fast32_t voice );

        void onGate( double gate, int voice );
        void onController( int16_t controllerId, double value );

    protected:
        void addAudioTarget( const PortData& data, VoiceAdapterType adapter );
        void addEventTarget( const PortData& data, VoiceAdapterType adapter );

    protected:
        PortDataList audioData_;
        Buffer< double* > audioOutBuffer_;
        Buffer< VoiceAdapterType > audioAdapterBuffer_;
        ModulationBuffer audioModulationBuffer_;

        PortDataList eventData_;
        InportList eventInports_;
        Buffer< VoiceAdapterType > eventAdapterBuffer_;
        ModulationBuffer eventModulationBuffer_;
    };


    class Inport : public Port
    {
        friend class Outport;

    public:
        Inport() : Port( PortTypeInport ) {}

        double* connectAudio();
        void connectEvent( int eventParamId );
        void disconnectAudio();
        void disconnectEvent();
        void disconnectAll() override;

        void setNumVoices( int numVoices ) override;
        void setOwner( Module* module );
        Module* getOwner() const;
        double* getAudioBuffer() const  { return audioInBuffer_; }

    protected:
        Buffer<double> audioInBuffer_;

        Module* owner_ = nullptr;
        int eventParamId_    = -1;
    };


    __forceinline void __stdcall Outport::putAudio( double value, int_fast32_t voice ) throw()
    {
        int modulationIndex = voice;

        for (int_fast32_t target = 0; target < numAudioConnections_; target++)
        {
            double mod = audioModulationBuffer_[modulationIndex];
            modulationIndex += numVoices_;
            if (mod != 1) {
                value *= mod;  // apply modulation
            }

            double* inportPointer = audioOutBuffer_[target];	            // get pointer to target

            VoiceAdapterType adapter = audioAdapterBuffer_[target];
            __assume(adapter < 3);
            switch (adapter)
            {
            case AdapterNone:
                *(inportPointer + voice) += value; 	                         // add value to existing value, per voice
                break;
            case AdapterMonoToPoly:
                for (int32_t i = 0; i < numVoices_; i++) {                  // add value to all voices of target
                    *(inportPointer + i) += value;
                }
                break;
            case AdapterPolyToMono:
                *inportPointer += value; 	                                // add value only to voice 0
                break;
            }
        }
    }


}  // namespace e3