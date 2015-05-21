
#pragma once

#include <set>
#include <string>
#include "core/Parameter.h"


namespace e3 {

    //-----------------------------------------------------------------
    // class PresetList
    //---------------------------------------------------------------- -

    class Preset
    {
        friend class PresetSet;

    public:
        Preset( int id=-1, const std::string& name="" ) :
            id_( id ),
            name_( name )
        {}

        bool operator<(const Preset& other) const     { return id_ < other.id_; }

        void addParameterSet( const ParameterSet& set ) const;
        void updateParameters( const Preset& other ) const;
        void clear() const;

        ParameterSet& getModuleParameters() const     { return moduleParameters_; }
        ParameterSet& getLinkParameters() const       { return linkParameters_; }


        int getId() const                             { return id_; }
        void setId( int id )                          { id_ = id; }

        const std::string& getName() const            { return name_; }
        void setName( const std::string& name ) const { name_ = name; }

        bool empty() const;

    protected:
        int id_ = 0;
        mutable std::string name_ = "Unnamed";

        mutable ParameterSet moduleParameters_;
        mutable ParameterSet linkParameters_;
    };



    //-----------------------------------------------------------------
    // class PresetSet
    //---------------------------------------------------------------- -

    class PresetSet : public std::set < Preset >
    {
    public:
        const Preset& getCurrentPreset() const;
        int getCurrentPresetId() const	   { return currentPresetId_; }
        void setCurrentPresetId( int id ) const;

        void updateCurrentPreset( const Preset& newPreset );
        void clearCurrentPreset();

        const Preset& addPreset( int id, const std::string& name="" );
        void addPreset( Preset& preset );
        void removePreset( int id );
        void renamePreset( int id, const std::string& name );

        bool contains( int id ) const;
        int findClosestId( int id ) const;

    protected:
        const_iterator find( int id ) const;
        int createUniqueId();
        std::string createUniqueName( const std::string& name ) const;

        mutable int currentPresetId_ = -1;
    };


} // namespace e3