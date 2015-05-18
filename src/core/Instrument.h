
#pragma once

#include <string>
#include <vector>
#include "JuceHeader.h"
#include "core/Preset.h"
#include "core/Module.h"


namespace e3 {

    class Polyphony;

    class Instrument
    {
    public:
        Instrument();
        Instrument( XmlElement* xml, const std::string& path );
        ~Instrument();

        void deleteModules();
        void initModules( double sampleRate, int numVoices, Polyphony* polyphony = nullptr );
        void resetModules();
        void connectModules();
        void updateModules();
        void suspendModules();
        void resumeModules();

        void loadPreset();
        const PresetSet& getPresets() const                      { return presetSet_; }
        const Preset& getPreset();
        const Preset& addPreset( int id, const std::string& name="" );

        Module* getModule( int moduleId ) const;
        const ModuleList& getModules() const    { return modules_; }
        int getNumModules() const               { return modules_.size(); }

        Module* createAndAddModule( ModuleType type );
        void deleteModule( Module* module );
        //void disconnectModules( const Link& link );

        void addLink( Link& link, bool addParameter = true );
        void removeLink( const Link& link );
        LinkSet& getLinks()                        { return links_; }
        void getLinksForModule( int moduleId, PortType portType, LinkList& list );

        void setNumVoices( int numVoices )         { numVoices_    = numVoices; }
        void setNumUnison( int numUnison )         { numUnison_    = numUnison; }
        void setUnisonSpread( int unisonSpread )   { unisonSpread_ = unisonSpread; }
        void setHold( bool hold )                  { hold_         = hold; }
        void setRetrigger( bool retrigger )        { retrigger_    = retrigger; }
        void setLegato( bool legato )              { legato_       = legato; }

        void setPath( const std::string& path ) const;
        std::string getPath() const;

        void setXml( XmlElement* xml )    { xml_ = xml; }
        XmlElement* getXml()              { return xml_; }


        int id_           = -1;
        int presetId_     = 0;
        int numVoices_    = 1;
        int numUnison_    = 1;
        int unisonSpread_ = 5;
        bool hold_        = false;
        bool retrigger_   = false;
        bool legato_      = false;
        bool ready_       = false;

        std::string name_ = "Default";
        std::string category_;
        std::string comment_;

    protected:
        bool hasAudioOutTerminal();
        int createUniqueId( ModuleType type );
        void createDefaultPreset();
        std::string createParameterLabel( const Link& link );

        ModuleList modules_;
        LinkSet links_;
        PresetSet presetSet_;

        ScopedPointer<XmlElement> xml_ = nullptr;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Instrument )
    };
}  // namespace e3
