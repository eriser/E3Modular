
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


    void Module::init( Polyphony* polyphony, double sampleRate )
    {
        polyphony_  = polyphony;

        setSampleRate( sampleRate );
        setNumVoices( polyphony->getNumVoices() );

        ASSERT( sampleRate_ > 0 );
        ASSERT( numVoices_ > 0 );

        connectSignals();

        initData();
        initParameters();
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

        updateParameters();
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


    void Module::initParameters()
    {
        for (ParameterMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            Parameter& param = it->second;
            if (param.midiShaper_.getControllerId() >= 0) {
                polyphony_->midiControllerSignal.Connect( this, &Module::onMidiController );
            }
        }
        updateParameters();
    }


    void Module::updateParameters()
    {
        for (ParameterMap::iterator it = parameters_.begin(); it != parameters_.end(); it++)
        {
            Parameter& param = it->second;
            setParameter( it->first, param.value_, 0, -1 );  // set all parameters to zero
        }
    }


    const Parameter& Module::getParameter( int parameterId ) const
    {
        return parameters_.at( parameterId );
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


    void Module::connectPort( Module* target, const Link& link )
    {
        ASSERT( target );

        if (target)
        {
            Outport* outport = getOutport( link.leftPort_ );
            ASSERT( outport );

            if (outport)
            {
                VoiceAdapterType adapter = selectVoiceAdapter( target->voicingType_ );
                outport->connect( target, link, adapter );
            }
        }
    }


    VoiceAdapterType Module::selectVoiceAdapter( VoicingType otherVoicingType ) const
    {
        return (voicingType_ == otherVoicingType) ? AdapterNone :
            (voicingType_ == Monophonic) ? AdapterMonoToPoly : AdapterPolyToMono;
    }


} // namespace e3