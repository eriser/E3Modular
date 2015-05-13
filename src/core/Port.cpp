
#include "e3_Exception.h"
#include "core/Port.h"
#include "core/Link.h"
#include "core/Module.h"


namespace e3 {


    //-------------------------------------------------------
    // class ModulationBuffer
    //-------------------------------------------------------

    void ModulationBuffer::init( int_fast32_t numVoices, PortDataList& targets )
    {
        numVoices_ = numVoices;

        set( 0 );
        resize( numVoices_ * targets.size() );

        for (size_t i = 0; i < targets.size(); i++)
        {
            PortData& data = targets[i];
            setValue( i, data.value_, -1 );
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


    void ModulationBuffer::onGate( uint32_t targetIndex, PortData& data, double gate, int voice )
    {
        if (data.veloSens_)
        {
            double value = data.valueShaper_.exponential( data.value_ );
            value       += gate * data.veloSens_;

            setValue( targetIndex, value, voice );
        }
    }


    void ModulationBuffer::onController( uint32_t targetIndex, PortData& data, int16_t controllerId, double value )
    {
        UNUSED( targetIndex );
        UNUSED( data );
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

    void Port::disconnectAll()
    {
        numAudioConnections_ = 0;
        numEventConnections_ = 0;
    }



    //-------------------------------------------------------
    // class Outport
    //-------------------------------------------------------

    void Outport::connect( Module* target, const PortData& data, VoiceAdapterType voiceAdapter ) 
    {
        ASSERT( target ); 
        if (target == nullptr) return;

        Inport* inport = target->getInport( data.rightPort_ );
        ASSERT( inport != nullptr );
        if (inport == nullptr) return;

        if (getType() & PortTypeAudio)
        {
            addAudioTarget( data, voiceAdapter );
            audioOutBuffer_.resize( numAudioConnections_ );
            audioOutBuffer_[numAudioConnections_ - 1] = inport->connectAudio();
        }
        else if (getType() & PortTypeEvent) 
        {
            addEventTarget( data, voiceAdapter );

            inport->connectEvent( data.rightPort_ );
            eventInports_.resize( numEventConnections_ );
            eventInports_.at( numEventConnections_ - 1 ) = inport;
        }
    }

   
    void Outport::disconnectAll()
    {
        audioData_.clear();
        eventData_.clear();
        audioAdapterBuffer_.clear();
        eventAdapterBuffer_.clear();
        audioModulationBuffer_.clear();
        eventModulationBuffer_.clear();
        audioOutBuffer_.clear();
        eventInports_.clear();

        Port::disconnectAll();
    }


    void Outport::putEvent( double value, int_fast32_t voice )
    {
        for (int_fast32_t target = 0; target < numEventConnections_; target++)
        {
            double modulation = eventModulationBuffer_[target * numVoices_ + voice];

            Inport* inport  = eventInports_.at( target );
            ASSERT( inport );
            Module* targetModule = inport->getOwner();
            ASSERT( targetModule );

            VoiceAdapterType adapter = eventAdapterBuffer_[target];
            __assume(adapter < 3);
            switch (adapter)
            {
            case AdapterNone:
                targetModule->setParameter( inport->eventParamId_, value, modulation, voice );
                break;
            case AdapterMonoToPoly:
                for (int_fast32_t i = 0; i < numVoices_; i++) {
                    targetModule->setParameter( inport->eventParamId_, value, modulation, i );
                }
                break;
            case AdapterPolyToMono:
                targetModule->setParameter( inport->eventParamId_, value, modulation, 0 );
                break;
            }
        }
    }
    

    void Outport::addAudioTarget( const PortData& data, VoiceAdapterType adapter )
    {
        audioData_.push_back( data );
        numAudioConnections_ = audioData_.size();

        audioAdapterBuffer_.resize( numAudioConnections_ );
        audioAdapterBuffer_[numAudioConnections_ - 1] = adapter;
        audioModulationBuffer_.init( numVoices_, audioData_ );
    }


    void Outport::addEventTarget( const PortData& data, VoiceAdapterType adapter )
    {
        eventData_.push_back( data );
        numEventConnections_ = eventData_.size();

        eventAdapterBuffer_.resize( numEventConnections_ );
        eventAdapterBuffer_[numEventConnections_ - 1] = adapter;
        eventModulationBuffer_.init( numVoices_, eventData_ );
    }


    void Outport::setNumVoices( int numVoices )
    {
        Port::setNumVoices( numVoices );

        audioModulationBuffer_.init( numVoices_, audioData_ );
        eventModulationBuffer_.init( numVoices_, eventData_ );
    }


    bool Outport::setParameter( const Parameter& parameter )
    {
        ASSERT( parameter.isValid() && parameter.isLinkType() );

        for (size_t i = 0; i < audioData_.size(); i++)
        {
            const PortData& data = audioData_[i];
            if (data.linkId_ == parameter.getId())
            {
                audioModulationBuffer_.setValue( i, parameter.value_ );
                return true;
            }
        }
        return false;
    }


    void Outport::onGate(double gate, int voice)
    {
        for (int_fast32_t i = 0; i < numAudioConnections_; i++) {
            audioModulationBuffer_.onGate( i, audioData_[i], gate, voice );
        }
        for (int_fast32_t i = 0; i < numEventConnections_; i++) {
            eventModulationBuffer_.onGate( i, eventData_[i], gate, voice );
        }
    }


    void Outport::onController(int16_t controllerId, double value)
    {
        for (int_fast32_t i = 0; i < numAudioConnections_; i++) {
            audioModulationBuffer_.onController( i, audioData_[i], controllerId, value );
        }
        for (int_fast32_t i = 0; i < numEventConnections_; i++) {
            eventModulationBuffer_.onController( i, eventData_[i], controllerId, value );
        }
    }



    //-------------------------------------------------------
    // class Inport
    //-------------------------------------------------------

    double* Inport::connectAudio()
    {
        ASSERT( audioInBuffer_ && audioInBuffer_.size() == (size_t)numVoices_ );

        numAudioConnections_++;
        return audioInBuffer_;
    }


    void Inport::connectEvent( int eventParamId )
    {
        numEventConnections_++;
        eventParamId_ = eventParamId;
    }


    void Inport::disconnectAudio()
    {
        ASSERT( numAudioConnections_ >= 1 );
        if (numAudioConnections_ >= 1)
            numAudioConnections_--;
    }


    void Inport::disconnectEvent()
    {
        ASSERT( numAudioConnections_ >= 1 );
        if (numAudioConnections_ >= 1)
            numAudioConnections_--;

        eventParamId_ = -1;
    }


    void Inport::disconnectAll()  // TODO: handle multiple connections
    {
        numAudioConnections_ = 0;
        numEventConnections_ = 0;

        eventParamId_        = -1;
    }


    void Inport::setNumVoices( int numVoices )
    {
        Port::setNumVoices( numVoices );
        audioInBuffer_.resize( numVoices_ );
    }


    void Inport::setOwner( Module* module )  { owner_ = module; }
    Module* Inport::getOwner() const { return owner_; }

}  // namespace e3