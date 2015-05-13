
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

        //void addModuleParameter( const Parameter& p ) const;
        //void addLinkParameter( const Parameter& p ) const;
        //void removeLinkParameter( int linkId, int moduleId ) const;

        void addParameterSet( const ParameterSet& set ) const;

        ParameterSet& getModuleParameters() const             { return moduleParameters_; }
        ParameterSet& getLinkParameters() const               { return linkParameters_; }

        bool operator<(const Preset& other) const             { return id_ < other.id_; }

        int getId() const                                     { return id_; }

        const std::string& getName() const                    { return name_; }
        void setName( const std::string& name ) const         { name_ = name; }

    protected:
        int id_ = 0;
        mutable std::string name_ = "Unnamed";

        mutable ParameterSet moduleParameters_;
        mutable ParameterSet linkParameters_;
    };


    //-----------------------------------------------------------------
    // class PresetList
    //---------------------------------------------------------------- -

    class PresetSet : public std::set < Preset >
    {
    public:
        const Preset& get( int id );
        void remove( int id );
        const Preset& addPreset(int id, const std::string& name="");
        bool contains( int id ) const;

    protected:
        const_iterator find( int id ) const;
        int createUniqueId();
    };


} // namespace e3