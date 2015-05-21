
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
		void update( const Preset& other ) const;
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
		const Preset& getSelectedPreset() const;
		void updateSelectedPreset( const Preset& newPreset );
		void clearSelectedPreset();

        void remove( int id );
        void renamePreset( int id, const std::string& name );
        const Preset& addPreset(int id, const std::string& name="");
        void addPreset( Preset& preset );
        bool contains( int id ) const;

		void selectPreset( int id ) const  { selectedPresetId_ = id; }
		int getSelectedPresetId() const	   { return selectedPresetId_; }

    protected:
        const_iterator find( int id ) const;
        int createUniqueId();

		mutable int selectedPresetId_ = -1;
    };


} // namespace e3