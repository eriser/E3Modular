
#include "e3_Exception.h"
#include "core/Port.h"
#include "core/Link.h"
#include "core/Module.h"


namespace e3 {

    //-------------------------------------------------------
    // class ModulationBuffer
    //-------------------------------------------------------

    void ModulationBuffer::init( int_fast32_t numVoices, LinkList& targets )
    {
        numVoices_ = numVoices;

        set( 0 );
        resize( numVoices_ * targets.size() );

        for (size_t i = 0; i < targets.size(); i++)
        {
            Link* link = &targets[i];
            setValue( i, link->value_, -1 );
        }
    }


    void ModulationBuffer::setValue( uint32_t targetIndex, double value, int voice )
    {
        if (voice == -1)                // set values for all voices
        {
            for (uint_fast32_t i = 0; i < numVoices_; i++) {
                data_[targetIndex * numVoices_ + i] = value;
            }
        }
        else if (numVoices_ > 0)        // set value for single voice
        {
            voice = std::min<int>( (int)(numVoices_ - 1), voice );
            data_[targetIndex * numVoices_ + voice] = value;
        }

    }


    void ModulationBuffer::onGate( uint32_t targetIndex, Link* link, double gate, int voice )
    {
        if (link->veloSens_)
        {
            double value = link->valueShaper_.exponential( link->value_ );
            value       += gate * link->veloSens_;

            setValue( targetIndex, value, voice );
        }
    }


    void ModulationBuffer::onController( uint32_t targetIndex, Link* link, int16_t controllerId, double value )
    {
        UNUSED( targetIndex );
        UNUSED( link );
        UNUSED( controllerId );
        UNUSED( value );
        //if (link->scaleController(controllerId, value))
        //{
        //    link->value_ = value;
        //    setValue( targetIndex, value, -1 );
        //}
    }


    //-------------------------------------------------------
    // class Port
    //-------------------------------------------------------

    void Port::setNumVoices(int numVoices)
    {
        numVoices_ = (uint_fast32_t)numVoices;
    }



    //-------------------------------------------------------
    // class Outport
    //-------------------------------------------------------

    void Outport::connect( Module* target, Link* link, VoiceAdapterType voiceAdapter ) 
    {
        ASSERT( target ); 
        ASSERT( link );
        if (target == nullptr) return;
        if (link == nullptr) return;

        Inport* inport = target->getInport( link->rightPort_ );
        ASSERT( inport != nullptr );
        if (inport == nullptr) return;

        if (getType() & PortTypeAudio)
        {
            addAudioTarget( link, voiceAdapter );
            audioOutBuffer_.resize( numAudioTargets_ );
            audioOutBuffer_[numAudioTargets_ - 1] = inport->connectAudio();
        }
        else if (getType() & PortTypeEvent) 
        {
            addEventTarget( link, voiceAdapter );

            inport->connectEvent( target, link->rightPort_ );
            eventInports_.resize( numEventTargets_ );
            eventInports_.at( numEventTargets_ - 1 ) = inport;
        }
    }

   
    void Outport::disconnect( Module* target, Link* link )
    {
        ASSERT( target );
        ASSERT( link );
        if (target == nullptr) return;
        if (link == nullptr) return;

        int16_t portId = audioLinks_.getIndex( *link );
        if (portId >= 0)
        {
            removeAudioTarget( link );
        
            Inport* inport = target->getInport( portId );
            ASSERT( inport );
            ASSERT( getType() & PortTypeAudio );

            if (inport != nullptr) {
                inport->disconnectAudio();
            }
            audioOutBuffer_.remove( portId );   

            return;
        }

        portId = eventLinks_.getIndex( *link );
        ASSERT( portId >= 0 );
        if (portId >= 0)
        {
            Inport* inport = target->getInport( portId );
            ASSERT( inport );
            ASSERT( getType() & PortTypeEvent );

            if (inport != nullptr) {
                inport->disconnectEvent();
            }
            removeEventTarget( link );
            eventInports_.erase( eventInports_.begin() + portId );   
        }
    }
    
    
    void Outport::disconnectAll()
    {
        audioLinks_.clear();
        eventLinks_.clear();
        audioAdapterBuffer_.clear();
        eventAdapterBuffer_.clear();
        audioModulationBuffer_.clear();
        eventModulationBuffer_.clear();
        audioOutBuffer_.clear();
        eventInports_.clear();

        numAudioTargets_ = 0;
        numEventTargets_ = 0;
    }


    uint32_t Outport::getNumConnections() const
    {
        return numAudioTargets_ + numEventTargets_;
    }


    void Outport::putEvent( double value, int_fast32_t voice )
    {
        for (uint_fast32_t target = 0; target < numEventTargets_; target++)
        {
            double modulation = eventModulationBuffer_[target * numVoices_ + voice];

            Inport* inport  = eventInports_.at( target );
            ASSERT( inport );
            Module* targetModule = inport->eventModule_;
            ASSERT( targetModule );

            VoiceAdapterType adapter = eventAdapterBuffer_[target];
            __assume(adapter < 3);
            switch (adapter)
            {
            case AdapterNone:
                targetModule->setParameter( inport->eventParamId_, value, modulation, voice );
                break;
            case AdapterMonoToPoly:
                for (uint_fast32_t i = 0; i < numVoices_; i++) {
                    targetModule->setParameter( inport->eventParamId_, value, modulation, i );
                }
                break;
            case AdapterPolyToMono:
                targetModule->setParameter( inport->eventParamId_, value, modulation, 0 );
                break;
            }
        }
    }
    

    void Outport::addAudioTarget( Link* link, VoiceAdapterType adapter )
    {
        numAudioTargets_ = audioLinks_.add( *link );

        audioAdapterBuffer_.resize( numAudioTargets_ );
        audioAdapterBuffer_[numAudioTargets_ - 1] = adapter;
        audioModulationBuffer_.init( numVoices_, audioLinks_ );
    }


    void Outport::addEventTarget( Link* link, VoiceAdapterType adapter )
    {
        numEventTargets_ = eventLinks_.add( *link );

        eventAdapterBuffer_.resize( numEventTargets_ );
        eventAdapterBuffer_[numEventTargets_ - 1] = adapter;
        eventModulationBuffer_.init( numVoices_, eventLinks_ );
    }


    void Outport::removeAudioTarget( Link* link )
    {
        numAudioTargets_ = audioLinks_.remove( *link );

        audioAdapterBuffer_.resize( numAudioTargets_ );
        audioModulationBuffer_.init( numVoices_, audioLinks_ );
    }


    void Outport::removeEventTarget( Link* link )
    {
        numEventTargets_ = eventLinks_.remove( *link );

        eventAdapterBuffer_.resize( numEventTargets_ );
        eventModulationBuffer_.init( numVoices_, eventLinks_ );
    }


    void Outport::setNumVoices( int numVoices )
    {
        Port::setNumVoices( numVoices );

        audioModulationBuffer_.init( numVoices_, audioLinks_ );
        eventModulationBuffer_.init( numVoices_, eventLinks_ );
    }


    void Outport::onGate(double gate, int voice)
    {
        for (uint_fast32_t i = 0; i < numAudioTargets_; i++) {
            audioModulationBuffer_.onGate( i, &audioLinks_[i], gate, voice );
        }
        for (uint_fast32_t i = 0; i < numEventTargets_; i++) {
            eventModulationBuffer_.onGate( i, &eventLinks_[i], gate, voice );
        }
    }


    void Outport::onController(int16_t controllerId, double value)
    {
        for (uint_fast32_t i = 0; i < numAudioTargets_; i++) {
            audioModulationBuffer_.onController( i, &audioLinks_[i], controllerId, value );
        }
        for (uint_fast32_t i = 0; i < numEventTargets_; i++) {
            eventModulationBuffer_.onController( i, &eventLinks_[i], controllerId, value );
        }
    }



    //-------------------------------------------------------
    // class Inport
    //-------------------------------------------------------

    double* Inport::connectAudio()
    {
        ASSERT( audioInBuffer_ && audioInBuffer_.size() == numVoices_ );

        numAudioSources_++;
        return audioInBuffer_;
    }


    void Inport::connectEvent( Module* eventModule, int eventParamId )
    {
        numEventSources_++;
        eventModule_  = eventModule;
        eventParamId_ = eventParamId;
    }


    void Inport::disconnectAudio()
    {
        ASSERT( numAudioSources_ >= 1 );
        if (numAudioSources_ >= 1)
            numAudioSources_--;
    }


    void Inport::disconnectEvent()
    {
        ASSERT( numAudioSources_ >= 1 );
        if (numAudioSources_ >= 1)
            numAudioSources_--;

        eventModule_  = nullptr;
        eventParamId_ = -1;
    }


    void Inport::disconnectAll()  // TODO: handle multiple connections
    {
        numAudioSources_ = 0;
        numEventSources_ = 0;

        eventModule_     = nullptr;
        eventParamId_    = -1;
    }


    uint32_t Inport::getNumConnections() const
    {
        return numAudioSources_ + numEventSources_;
    }


    void Inport::setNumVoices( int numVoices )
    {
        Port::setNumVoices( numVoices );
        audioInBuffer_.resize( numVoices_ );
    }


}  // namespace e3