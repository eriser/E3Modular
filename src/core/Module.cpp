
#include <e3_Exception.h>

#include "core/GlobalHeader.h"
#include "core/Polyphony.h"
#include "core/Module.h"



namespace e3 {

    Module::Module(
        ModuleType moduleType,
        const std::string& label,
        VoicingType voicingType,
        ProcessingType processingType,
        ModuleStyle style )
        :
        label_( label ),
        moduleType_( moduleType ),
        voicingType_( voicingType ),
        processingType_( processingType ),
        moduleStyle_( style ),
        mono_( voicingType_ == Monophonic )
    {}


    Module::~Module()
    {
        outports_.clear();
        inports_.clear();
        reset();
    }


    void Module::init( double sampleRate, int numVoices, Polyphony* polyphony )
    {
        ASSERT( sampleRate > 0 );
        ASSERT( numVoices > 0 );
        ASSERT( polyphony != nullptr );

        polyphony_  = polyphony;
        setNumVoices( numVoices );
        initData();
        setSampleRate( sampleRate );
        connectSignals();
    }


    void Module::reset()
    {
        disconnectSignals();
        disconnectPorts();

        sampleRate_ = 0;
        numVoices_  = 0;
        polyphony_  = nullptr;
    }


    void Module::update()
    {
        ASSERT( sampleRate_ > 0 );
        ASSERT( numVoices_ > 0 );

        updatePorts();
    }


    void Module::initData()
    {
        for (size_t i = 0; i < inports_.size(); i++) {
            inports_[i]->setNumVoices( numVoices_ );
        }
        for (size_t i = 0; i < outports_.size(); i++) {
            outports_[i]->setNumVoices( numVoices_ );
        }
    }
    
    
    void Module::disconnectPorts()
    {
        outports_.disconnect();
        inports_.disconnect();
    }


    void Module::onMidiController( int, int )  {}


    void Module::setSampleRate( double sampleRate )
    {
        sampleRate_ = sampleRate;
    }


    void Module::setNumVoices( int numVoices )
    {
        numVoices_ = mono_ ? 1 : numVoices;
    }


    void Module::addInport( int portId, const std::string& label, Inport* port )
    {
        ASSERT( port );
        ASSERT( port->getType() & PortTypeInport );

        port->setId( portId );
        port->setLabel( label );
        port->setOwner( this );
        inports_.insert( inports_.begin() + portId, port );
    }


    void Module::addOutport( int id, const std::string& label, Outport* port, PortType portType )
    {
        ASSERT( port );
        ASSERT( port->getType() & PortTypeOutport );

        port->setId( id );
        port->setLabel( label );
        port->setProcessType( portType );

        outports_.insert( outports_.begin() + id, port );
    }


    Outport* Module::getOutport( int portId ) const
    {
        return portId < (int)outports_.size() ? outports_.at( portId ) : nullptr;
    }


    Inport* Module::getInport( int portId ) const
    {
        return portId < (int)inports_.size() ? inports_.at( portId ) : nullptr;
    }


    void Module::connect( Module* target, const PortData& data )
    {
        Outport* outport = getOutport( data.leftPort_ );
        ASSERT( outport );

        if (outport)
        {
            VoiceAdapterType adapter = selectVoiceAdapter( target->voicingType_ );
            outport->connect( target, data, adapter );
        }
    }


    //void Module::disconnect( Module* target, const Link& link )
    //{
    //    Outport* outport = getOutport( link.leftPort_ );
    //    ASSERT( outport );

    //    if (outport) {
    //        outport->disconnect( target, link );
    //    }
    //}


    VoiceAdapterType Module::selectVoiceAdapter( VoicingType otherVoicingType ) const
    {
        return (voicingType_ == otherVoicingType) ? AdapterNone :
            (voicingType_ == Monophonic) ? AdapterMonoToPoly : AdapterPolyToMono;
    }


    ParameterSet& Module::getDefaultParameters() const
    {
        static ParameterSet defaultParameters;
        return defaultParameters;
    }


    const Parameter& Module::getDefaultParameter( int parameterId ) const
    {
        ParameterSet& set = getDefaultParameters();
        return set.get( parameterId, id_ );
    }


    void Module::setParameter( const Parameter& parameter )
    {
        if (parameter.target_ == ParameterModule) {
            setParameter( parameter.getId(), parameter.value_ );
        }
        else if (parameter.target_ == ParameterLink)
        {
            for (OutportList::const_iterator it = outports_.begin(); it != outports_.end(); ++it)
            {
                Outport* outport = *it;
                if (outport->setParameter( parameter )) {
                    break;
                }
            }
        }
    }



} // namespace e3