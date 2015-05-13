
#include <e3_Exception.h>
#include "core/Preset.h"


namespace e3 {

    //-----------------------------------------------------
    // class Preset
    //-----------------------------------------------------

    //void Preset::addModuleParameter( const Parameter& p ) const   
    //{ 
    //    p.targetType_ = TargetTypeModule;
    //    moduleParameters_.insert( p ); 
    //}


    //void Preset::addLinkParameter( const Parameter& p ) const     
    //{ 
    //    p.targetType_ = TargetTypeLink;
    //    linkParameters_.insert( p ); 
    //}


    //void Preset::removeLinkParameter( int linkId, int moduleId ) const
    //{
    //    ASSERT( linkParameters_.size() > 0 );
    //    linkParameters_.remove( linkId, moduleId );
    //}


    void Preset::addParameterSet( const ParameterSet& set ) const
    {
        for (ParameterSet::const_iterator it = set.begin(); it != set.end(); ++it)
        {
            const Parameter& parameter = *it;
            ASSERT( parameter.isValid() );
            if (parameter.isValid()) 
            {
                if (parameter.isModuleType())
                    moduleParameters_.insert( parameter );
                else if (parameter.isLinkType())
                    linkParameters_.insert( parameter );
            }
        }
        //moduleParameters_.insert( set.begin(), set.end() ); 
    }



    //-----------------------------------------------------
    // class PresetSet
    //-----------------------------------------------------

    const Preset& PresetSet::get( int id )
    {
        const_iterator pos = find( id );
        if (pos != end()) {
            return *pos;
        }
        else THROW( std::out_of_range, "No Preset found with id=%d", id );
    }


    void PresetSet::remove( int id )
    {
        const_iterator pos = find( id );
        ASSERT( pos != end() );
        if (pos != end()) {
            erase( pos );
        }
    }


    bool PresetSet::contains( int id ) const
    {
        const_iterator pos = find( id );
        return pos != end();
    }


    PresetSet::const_iterator PresetSet::find( int id ) const
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            const Preset& preset = *it;
            if (preset.id_ == id) {
                return it;
            }
        }
        return end();
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
            count++;
        }
        return size();
    }

    
    const Preset& PresetSet::addPreset(int id, const std::string& name)
    {
        id = (id < 0) ? createUniqueId() : id;
        auto result = emplace( Preset( id, name ) );
        if (result.second) {
            return *result.first;
        }
        THROW( std::runtime_error, "Can not insert Preset with non-unique id" );
    }


} // namespace e3