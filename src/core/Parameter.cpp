

#include <e3_Exception.h>
#include "core/Parameter.h"


namespace e3 {

    //----------------------------------------------------------------
    // class Parameter
    //----------------------------------------------------------------

    Parameter::Parameter( int id, int moduleId, TargetType targetType, ControlType controlType ) :
        id_( id ),
        moduleId_( moduleId ),
        targetType_( targetType ),
        controlType_( controlType )
    {}



    //------------------------------------------------------
    // class ParameterSet
    //------------------------------------------------------

    const Parameter& ParameterSet::add( const Parameter& parameter )
    {
        if (parameter.isValid()) 
        {
            auto result = insert( parameter );
            ASSERT( result.second );

            return *result.first;
        }
        THROW( Assertion, "Parameter is not valid and can not be inserted." );
    }


    const Parameter& ParameterSet::addModuleParameter(
        int id, int moduleId,
        const std::string& label,
        ControlType controlType,
        double defaultValue)
    {
        Parameter p( id, moduleId, Parameter::TargetTypeModule, controlType );
        p.label_ = label;
        p.defaultValue_ = defaultValue;

        return add( p );
    }


    const Parameter& ParameterSet::addLinkParameter( int id, int moduleId )
    {
        Parameter p( id, moduleId, Parameter::TargetTypeLink, ControlSlider );
        return add( p );
    }


    const Parameter& ParameterSet::get( int id, int moduleId )
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            const Parameter& parameter = *it;
            if (parameter.id_ == id && parameter.moduleId_ == moduleId) {
                return parameter;
            }
        }
        THROW( std::out_of_range, "No Parameter found with id=%d and moduleId=%d", id, moduleId );
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

