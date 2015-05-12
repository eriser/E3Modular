
#pragma once

#include <cstdint>
#include <string>

#include "core/GlobalHeader.h"
#include "core/Port.h"
#include "core/Link.h"
#include "core/Parameter.h"

#pragma warning(push)
#pragma warning(disable: 4100) 


namespace e3 {

    class Module;
    class Outport;
    class Polyphony;

    // Pointer type to the audio processing function of a module.
    typedef void ( Module::*ProcessFunctionPointer )( void ) throw( );

    enum ModuleType {
        ModuleTypeUndefined = -1,

        // system modules
        ModuleTypeAudioOutTerminal = 0,
        ModuleTypeMidiGate = 1,
        ModuleTypeMidiFrequency = 2,
        ModuleTypeMidiInput = 3,

        // audio
        ModuleTypeSineOscillator = 10,
        ModuleTypeAdsrEnvelope = 11,
        ModuleTypeDelay = 12,
    };


    enum ProcessingType
    {
        ProcessEvent = 1,
        ProcessControl = 2,
        ProcessAudio = 4
    };

    enum VoicingType
    {
        Monophonic = 0,
        Polyphonic = 1
    };

    enum ModuleStyle
    {
        ModuleStyleNothing = 0,
        ModuleStyleVisible = 1,
        ModuleStyleLabel = 2
    };


    //--------------------------------------------------------------------------
    // class Module
    // Abstract base class for Modules.
    //--------------------------------------------------------------------------

    class Module
    {
        friend class Instrument;
        friend class Sink;

    protected:
        Module(
            ModuleType moduleType,
            const std::string& label,
            VoicingType voicingType,
            ProcessingType processingType,
            ModuleStyle style = (ModuleStyle)( ModuleStyleVisible | ModuleStyleLabel ) );

    public:
        virtual ~Module();

        void processAudio() throw( ) {}
        virtual void processEvent( double value, uint16_t voices ) throw( ) {}
        virtual void processControl() throw ( ) {}

        virtual void onMidiController( int controllerNum, int value );

        virtual void suspend() {}
        virtual void resume() {}

        virtual void setSampleRate( double sampleRate );
        virtual void setNumVoices( int numVoices );

        virtual ParameterSet& getDefaultParameters() const;
        virtual const Parameter& getDefaultParameter( int parameterId ) const;
        virtual void setParameter( int paramId, double value, double modulation = 0.f, int voice = -1 ) {}
        virtual void setParameter( const Parameter& parameter );

        const InportList& getInports() const           { return inports_; }
        const OutportList& getOutports() const         { return outports_; }
        Inport* getInport( int portId ) const;         
        Outport* getOutport( int portId ) const;       
        int getNumInports() const                      { return inports_.size(); }
        int getNumOutports() const                     { return outports_.size(); }
                                                       
        int getId() const                              { return id_; }
        void setId( int id )                           { id_ = id; }
        const std::string& getLabel() const            { return label_; }
        void setLabel( const std::string& label )      { label_ = label; }
        VoicingType getVoicingType() const             { return voicingType_; }
        void setVoicingType( VoicingType type )        { voicingType_ = type; }
                                                       
        const ModuleType moduleType_;
        const ProcessingType processingType_;
        const ModuleStyle moduleStyle_;

    protected:
        std::string label_;
        int id_ = -1;
        VoicingType voicingType_;

        InportList inports_;
        OutportList outports_;

        ProcessFunctionPointer processFunction_ = nullptr;

        // Constructs all member and initializes them with the current sample rate and number of voices.
        virtual void init( double sampleRate, int numVoices, Polyphony* polyphony );

        // Update after all modules are connected. Links remain valid.
        virtual void update();

        // Deletes all members and links. After reset the module is in the same state as before init().
        virtual void reset();

        virtual void connectSignals() {}
        virtual void disconnectSignals() {}

        virtual void initData();

        virtual void updatePorts() {}
        virtual void disconnectPorts();
        void connect( Module* target, const PortData& data );
        void disconnect( Module* target, const Link& link );

        void addInport( int portId, const std::string& label, Inport* port );
        void addOutport( int portId, const std::string& label, Outport* port, PortType portType );

        VoiceAdapterType selectVoiceAdapter( VoicingType other ) const;

        double sampleRate_ = INITIAL_SAMPLERATE;
        int numVoices_     = 0;
        bool mono_         = false;

        Polyphony* polyphony_ = nullptr;

    private: // Make non copyable and prevent C4512
        Module( const Module& ) = delete;
        Module& operator=( const Module& ) = delete;
    };

    typedef std::vector<Module*> ModuleList;

} // namespace e3

#pragma warning(pop)
