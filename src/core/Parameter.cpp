

#include "core/Parameter.h"


namespace e3 {


    //----------------------------------------------------------------
    // class Parameter
    //----------------------------------------------------------------

    Parameter::Parameter(int id, const std::string& label, ControlType controlType, double defaultValue) :
        id_(id),
        defaultValue_(defaultValue),
        controlType_(controlType),
        label_(label)
    {}


    //----------------------------------------------------------------
    // class ParameterMap
    //----------------------------------------------------------------

    void ParameterMap::add(const Parameter& p)
    {
        insert(std::make_pair(p.id_, p));
    }


    bool ParameterMap::containsId(const int id) const
    {
        return find(id) != end();
    }

    
    void ParameterMap::update(const Parameter& p)
    {
        at(p.id_) = p;
    }

    void ParameterMap::setDefaultValues()
    {
        for (auto it = begin(); it != end(); it++) 
        {
            Parameter& p = it->second;
            p.value_ = p.defaultValue_;
        }
    }

} // namespace e3

