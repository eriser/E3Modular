
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


	void Preset::update( const Preset& other ) const
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

	const Preset& PresetSet::getSelectedPreset() const
	{
		const_iterator pos = find( selectedPresetId_ );
		if( pos != end() ) {
			return *pos;
		}
		else THROW( std::out_of_range, "No Preset found with id=%d", selectedPresetId_ );
	}


	void PresetSet::updateSelectedPreset( const Preset& newPreset )
	{
		const_iterator pos = find( selectedPresetId_ );
		if( pos != end() ) 
		{
			const Preset& preset = *pos;
			preset.update( newPreset );
		}
	}


	void PresetSet::clearSelectedPreset()
	{
		const_iterator pos = find( selectedPresetId_ );
		if( pos != end() )
		{
			const Preset& preset = *pos;
			preset.clear();
		}
	}



    void PresetSet::remove( int id )
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


    void PresetSet::addPreset( Preset& preset )
    {
        int id = createUniqueId();
        preset.setId( id );
        auto result = insert( preset );

        if (result.second == false) {
            THROW( std::runtime_error, "Can not insert Preset" );
        }
        selectPreset( id );
    }



} // namespace e3