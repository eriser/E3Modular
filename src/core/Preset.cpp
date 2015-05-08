
#include <e3_Exception.h>
#include "core/Preset.h"


namespace e3 {

    //-----------------------------------------------------
    // class Preset
    //-----------------------------------------------------

    void Preset::addModuleParameter( const Parameter& p ) const   
    { 
        moduleParameters_.insert( p ); 
    }


    void Preset::addLinkParameter( const Parameter& p ) const     
    { 
        linkParameters_.insert( p ); 
    }


    void Preset::addModuleParameterSet( const ParameterSet& set ) const
    {
        moduleParameters_.insert( set.begin(), set.end() ); 
    }


    void Preset::addLinkParameterSet( const ParameterSet& set ) const
    { 
        linkParameters_.insert( set.begin(), set.end() ); 
    }


    //-----------------------------------------------------
    // class PresetSet
    //-----------------------------------------------------

    const Preset& PresetSet::get( int id )
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            const Preset& preset = *it;
            if (preset.id_ == id) {
                return preset;
            }
        }
        THROW( std::out_of_range, "No Preset found with id=%d", id );
    }


    bool PresetSet::contains( int id ) const
    {
        for (const_iterator it = begin(); it != end(); ++it)
        {
            const Preset& preset = *it;
            if (preset.id_ == id) {
                return true;
            }
        }
        return false;
    }


    int PresetSet::createUniqueId()
    {
        int count = 0;
        for (iterator it = begin(); it != end(); ++it) 
        {
            const Preset& preset = *it;
            if (preset.id_ != count) {
                return count;
            }
        }
        return size();
    }

    
    const Preset& PresetSet::createNewPreset(int id)
    {
        id = (id < 0) ? createUniqueId() : id;
        auto result = emplace( Preset(id) );
        if (result.second) {
            return *result.first;
        }
        THROW( std::runtime_error, "Can not insert Preset with non-unique id" );
    }


} // namespace e3