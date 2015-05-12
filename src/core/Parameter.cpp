

#include <e3_Exception.h>
#include "core/Parameter.h"


namespace e3 {

    //----------------------------------------------------------------
    // class Parameter
    //----------------------------------------------------------------

    Parameter::Parameter( int id, int moduleId, 
                          const std::string& label, 
                          ControlType controlType, 
                          double defaultValue ) :
        id_( id ),
        moduleId_( moduleId ),
        value_( defaultValue ),
        defaultValue_( defaultValue ),
        controlType_( controlType ),
        label_( label )
    {}



    //------------------------------------------------------
    // class ParameterSet
    //------------------------------------------------------

    void ParameterSet::add( const Parameter& p )
    {
        bool valid = p.isValid();
        ASSERT( valid );
        if (valid) 
        {
            auto result = insert( p );
            ASSERT( result.second );
        }
    }


    const Parameter& ParameterSet::get( int parameterId, int moduleId )
    {
        
        for (iterator it = begin(); it != end(); ++it)
        {
            const Parameter& parameter = *it;
            if (parameter.id_ == parameterId && parameter.moduleId_ == moduleId) {
                return parameter;
            }
        }
        THROW( std::out_of_range, "No Parameter found with id=%d and moduleId=%d", parameterId, moduleId );
    }


    void ParameterSet::removeAllByModule( int moduleId )
    {
        for (iterator it = moduleFirst( moduleId ); it != moduleLast( moduleId ); )
        {
            erase( it++ );
        }
    }


    void ParameterSet::remove( int id, int moduleId )
    {
        for (iterator it = begin(); it != end(); )
        {
            const Parameter& p = *it;
            if (p.getId() == id && p.getModuleId() == moduleId) {
                erase( it++ );
            }
            else ++it;
        }
    }



} // namespace e3

