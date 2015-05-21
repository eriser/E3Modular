
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


    void Preset::updateParameters( const Preset& other ) const
    {
        moduleParameters_ = other.moduleParameters_;
        linkParameters_   = other.linkParameters_;
    }


    void Preset::clear() const
    {
        moduleParameters_.clear();
        linkParameters_.clear();
    }


    bool Preset::empty() const 
    { 
        return moduleParameters_.empty() && linkParameters_.empty(); 
    }



    //-----------------------------------------------------
    // class PresetSet
    //-----------------------------------------------------

    const Preset& PresetSet::getCurrentPreset() const
    {
        const_iterator pos = find( currentPresetId_ );
        if (pos != end()) {
            return *pos;
        }
        else THROW( std::out_of_range, "No Preset found with id=%d", currentPresetId_ );
    }


    void PresetSet::setCurrentPresetId( int id ) const  
    { 
        if (contains( id )) {
            currentPresetId_ = id;
        }
    }


    void PresetSet::updateCurrentPreset( const Preset& newPreset )
    {
        const_iterator pos = find( currentPresetId_ );
        if( pos != end() ) 
        {
            const Preset& preset = *pos;
            preset.updateParameters( newPreset );
        }
    }


    void PresetSet::clearCurrentPreset()
    {
        const_iterator pos = find( currentPresetId_ );
        if( pos != end() )
        {
            const Preset& preset = *pos;
            preset.clear();
        }
    }


    const Preset& PresetSet::addPreset( int id, const std::string& name )
    {
        id = (id < 0) ? createUniqueId() : id;
        std::string uniqueName = createUniqueName( name );
        auto result = emplace( Preset( id, uniqueName ) );

        if (result.second) {
            return *result.first;
        }
        THROW( std::runtime_error, "Can not insert Preset with non-unique id" );
    }


    void PresetSet::addPreset( Preset& preset )
    {
        int id = createUniqueId();
        preset.setId( id );
        std::string name = createUniqueName( preset.getName() );
        preset.setName( name );

        auto result = insert( preset );

        if (result.second == false) {
            THROW( std::runtime_error, "Can not insert Preset" );
        }
    }


    void PresetSet::removePreset( int id )
    {
        const_iterator pos = find( id );
        ASSERT( pos != end() );
        if (pos != end()) {
            erase( pos );
        }
    }


    void PresetSet::renamePreset( int id, const std::string& name )
    {
        const_iterator pos = find( id );
        ASSERT( pos != end() );
        if (pos != end()) {
            const Preset& preset = *pos;
            preset.setName( name );
        }
    }


    int PresetSet::findClosestId( int id ) const
    {
        const_iterator pos = lower_bound( id );
        
        if (pos != begin()) {                   // id is higher than all elements or somewhere in the middle
            pos--;                              // use the one below 
        }
        if (pos == end()) {                     // set is empty
            return -1;
        }
        else {
            const Preset& preset = *pos;
            return preset.getId();
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


    std::string PresetSet::createUniqueName( const std::string& name ) const
    {
        bool done  = false;
        int suffix = 0;
        std::string newName = name;

        do {
            bool found = false;
            for (iterator it = begin(); it != end(); ++it)
            {
                const Preset& next = *it;
                if (newName == next.getName()) {
                    newName = name + "-" + std::to_string( ++suffix );
                    found = true;
                    break;
                }
            }
            done = !found;
        } 
        while (done == false);

        return newName;
    }

    

} // namespace e3