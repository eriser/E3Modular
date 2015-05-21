
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

        void save( const std::string& path );


        void deleteModules();
        void initModules( double sampleRate, int numVoices, Polyphony* polyphony = nullptr );
        void resetModules();
        void connectModules();
        void updateModules();
        void suspendModules();
        void resumeModules();

        void loadPreset( int id = -1, bool allowSaving = false );
        void saveCurrentPreset();
        const Preset& addPreset();
        const Preset& deleteCurrentPreset();
        void renameCurrentPreset( const std::string& name );
        const PresetSet& getPresets() const { return presetSet_; }
        const Preset& getCurrentPreset() const { return currentPreset_; }

        Module* getModule( int moduleId ) const;
        const ModuleList& getModules() const    { return modules_; }
        int getNumModules() const               { return modules_.size(); }

        Module* createAndAddModule( ModuleType type );
        void deleteModule( Module* module );

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

        void setFilePath( const std::string& path ) { file_ = path; }
        File getFilePath() const                    { return file_; }
        void storeFilePath() const;

        void setXml( XmlElement* xml )    { xml_ = xml; }
        XmlElement* getXml()              { return xml_; }


        int presetId_     = 0;
        int numVoices_    = 1;
        int numUnison_    = 1;
        int unisonSpread_ = 5;
        bool hold_        = false;
        bool retrigger_   = false;
        bool legato_      = false;
        bool ready_       = false;

        std::string name_ = "Default";

    protected:
        bool hasAudioOutTerminal();
        int createUniqueId( ModuleType type );
        void createDefaultPreset();
        const Preset& setCurrentPreset( int id );
        std::string createParameterLabel( const Link& link );
        void removeModuleParameters( int moduleId );
        void removeLinkParameters( int linkId, int moduleId );


        ModuleList modules_;
        LinkSet links_;
        PresetSet presetSet_;
        Preset currentPreset_;

        ScopedPointer<XmlElement> xml_ = nullptr;
        File file_;
    };
}  // namespace e3
