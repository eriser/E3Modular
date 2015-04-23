
#pragma once

#include <string>
#include <map>

#include "core/GlobalHeader.h"
#include "core/ParameterShaper.h"


namespace e3 {
    
    //--------------------------------------------------------------------
    // class Parameter
    // Stores the data of a parameter
    //--------------------------------------------------------------------

    class Parameter
    {
    public:
        Parameter() {}
        Parameter(uint16_t id, const std::string& label, ControlType controlType, double defaultValue = 0);


        uint16_t id_               = 0;
        ControlType controlType_   = kControlHidden;
        NumberFormat numberFormat_ = kNumberFloat;
        double value_              = 0;
        double defaultValue_       = 0;
        double veloSens_           = 0;
        double keyTrack_           = 0;
        double resolution_         = 1;
        std::string label_;
        std::string unit_;

        ParameterShaper valueShaper_;
        MidiParameterShaper midiShaper_;
    };


    //-------------------------------------------
    // class ParameterMap
    //-------------------------------------------

    class ParameterMap : public std::map< uint16_t, Parameter >
    {
    public:
        void add(const Parameter& moel);

        // Returns true, if a Parameter with the given id exists in the map.
        bool containsId(const uint16_t id) const;

        // Updates the Parameter in the map with the data of the given Parameter. 
        // Parameter is identified by model.id_;
        // If the id does not exist in the map, a std::out_of_range is thrown.
        void update(const Parameter& model);
        void setDefaultValues();
    };

} // namespace e3



