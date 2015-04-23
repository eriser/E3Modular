

#include "core/Parameter.h"


namespace e3 {


    //----------------------------------------------------------------
    // class Parameter
    //----------------------------------------------------------------

    Parameter::Parameter(uint16_t id, const std::string& label, ControlType controlType, double defaultValue) :
        id_(id),
        defaultValue_(defaultValue),
        controlType_(controlType),
        label_(label)
    {}


    //----------------------------------------------------------------
    // class ParameterMap
    //----------------------------------------------------------------

    void ParameterMap::add(const Parameter& model)
    {
        insert(std::make_pair(model.id_, model));
    }


    bool ParameterMap::containsId(const uint16_t id) const
    {
        return find(id) != end();
    }

    
    void ParameterMap::update(const Parameter& model)
    {
        at(model.id_) = model;
    }

    void ParameterMap::setDefaultValues()
    {
        for (auto it = begin(); it != end(); it++) 
        {
            Parameter& model = it->second;
            model.value_ = model.defaultValue_;
        }
    }

} // namespace e3

