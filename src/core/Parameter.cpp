

#include <e3_Exception.h>
#include "core/Parameter.h"


namespace e3 {


    //----------------------------------------------------------------
    // class Parameter
    //----------------------------------------------------------------

    Parameter::Parameter( int id, const std::string& label, ControlType controlType, double defaultValue ) :
        id_( id ),
        value_( defaultValue ),
        defaultValue_( defaultValue ),
        controlType_( controlType ),
        label_( label )
    {}

    Parameter::Parameter( int id, int ownerId, const std::string& label, ControlType controlType, double defaultValue ) :
        id_( id ),
        ownerId_( ownerId ),
        value_( defaultValue ),
        defaultValue_( defaultValue ),
        controlType_( controlType ),
        label_( label )
    {}

    //----------------------------------------------------------------
    // class ParameterMap
    //----------------------------------------------------------------

    void ParameterMap::add( const Parameter& p )
    {
        insert( std::make_pair( p.getId(), p ) );
    }


    bool ParameterMap::containsId( const int id ) const
    {
        return find( id ) != end();
    }


    void ParameterMap::setDefaultValues()
    {
        for (auto it = begin(); it != end(); it++)
        {
            Parameter& p = it->second;
            p.value_ = p.defaultValue_;
        }
    }


    //------------------------------------------------------
    // class ParameterList
    //------------------------------------------------------

    //Parameter& ParameterList::createNewParameter()
    //{
    //    emplace_back( Parameter() );
    //    return back();
    //}




    //Parameter& ParameterList::getParameterById( int parameterId )
    //{
    //    for (auto it = begin(); it != end(); it++)
    //    {
    //        Parameter& p = *it;
    //        if (p.id_ == parameterId) {
    //            return p;
    //        }
    //    }
    //    THROW( std::out_of_range, "Parameter with id %d not found", parameterId );
    //}


    //ParameterList ParameterList::getParametersForId( int moduleId, int linkId )
    //{
    //    ParameterList result;

    //    for (auto it = begin(); it != end(); ++it)
    //    {
    //        Parameter& p = *it;
    //        if (moduleId > -1 && p.moduleId_ == moduleId || linkId > -1 && p.moduleId_ == linkId) {
    //            result.push_back( p );
    //        }
    //    }
    //    return result;
    //}


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


    const Parameter& ParameterSet::get( int parameterId, int ownerId )
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            const Parameter& parameter = *it;
            if (parameter.id_ == parameterId && parameter.ownerId_ == ownerId) {
                return parameter;
            }
        }
        THROW( std::out_of_range, "No Parameter found with id=%d and ownerId=%d", parameterId, ownerId );
    }


    void ParameterSet::eraseAllByOwner( int ownerId )
    {
        for (iterator it = ownerFirst( ownerId ); it != ownerLast( ownerId ); )
        {
            erase( it++ );
        }
    }



} // namespace e3

