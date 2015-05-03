
#pragma once

#include <string>
#include <vector>
#include "JuceHeader.h"
#include "core/Module.h"


namespace e3 {

    class Polyphony;

    class Instrument
    {
    public:
        Instrument();
        ~Instrument();

        void deleteModules();
        void initModules( Polyphony* polyphony, double sampleRate );
        void resetModules();
        void connectModules();
        void updateModules();
        void resumeModules();
        void suspendModules();

        void setSampleRate( double sampleRate );
        void setNumVoices( uint16_t numVoices );

        Module* getModule( int moduleId ) const;
        const ModuleList& getModules() const { return modules_; }
        int getNumModules() const            { return modules_.size(); }

        Module* createAndAddModule( ModuleType type );
        void deleteModule( Module* module );

        Link* addLink( const Link& link );
        void removeLink( const Link& link );
        LinkList& getLinks()                            { return links_; }
        void getLinksForModule( int moduleId, PortType portType, LinkList& list );


        void setNumUnison( uint16_t numUnison )         { numUnison_    = numUnison; }
        void setUnisonSpread( uint16_t unisonSpread )   { unisonSpread_ = unisonSpread; }
        void setHold( bool hold )                       { hold_         = hold; }
        void setRetrigger( bool retrigger )             { retrigger_    = retrigger; }
        void setLegato( bool legato )                   { legato_       = legato; }

        bool checkSentinel( Module* module );

        uint16_t numVoices_    = 1;
        uint16_t numUnison_    = 1;
        uint16_t unisonSpread_ = 5;
        bool hold_             = false;
        bool retrigger_        = false;
        bool legato_           = false;
        bool ready_            = false;
        std::string name_      = "Default";
        std::string category_;
        std::string comment_;
        int hash_;

    protected:
        bool hasAudioOutTerminal();
        int createModuleId( ModuleType type );

        ModuleList modules_;
        LinkList links_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Instrument )
    };
}  // namespace e3
