
#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>

#include "core/GlobalHeader.h"
#include "core/ParameterShaper.h"


namespace e3 {

    enum ParameterTarget 
    {
        ParameterUndefined,
        ParameterModule,
        ParameterLink
    };
    
    //--------------------------------------------------------------------
    // class Parameter
    // Parameters control the behaviour of Modules and Links
    //--------------------------------------------------------------------

    class Parameter
    {
        friend class ParameterSet;

    public:
        Parameter() {}
        Parameter( int id, int moduleId, 
                   const std::string& label = "", 
                   ControlType controlType = ControlHidden, 
                   double defaultValue = 1 );

        bool operator==(const Parameter& other) const
        {
            return other.id_ == id_ && other.moduleId_ == moduleId_;
        }
        bool operator==(const Parameter* other) const   { return operator==(*other); }
        bool operator!=(const Parameter& other) const   { return !(*this == other); }
        bool operator!=(const Parameter* other) const   { return !(this == other); }

        bool operator<(const Parameter& other) const
        {
            if (other.moduleId_ == moduleId_) return id_ < other.id_;
            return moduleId_ < other.moduleId_;
        }

        int getId() const                { return id_; }
        int getModuleId() const          { return moduleId_; }

    protected:
        int id_       = 0;
        int moduleId_ = -1;

    public:
        ControlType controlType_           = ControlHidden;
        mutable ParameterTarget target_    = ParameterUndefined;
        mutable NumberFormat numberFormat_ = NumberFloat;
        mutable double defaultValue_       = 1;
        mutable double value_              = defaultValue_;
        mutable double veloSens_           = 0;
        mutable double keyTrack_           = 0;
        mutable double resolution_         = 1;
        mutable std::string label_;
        mutable std::string unit_;

        mutable ParameterShaper valueShaper_;
        mutable MidiParameterShaper midiShaper_;

        bool isValid() const            { return id_ > -1 && moduleId_ > -1; }
        
        double getMin() const           { return valueShaper_.getMin(); }
        double getMax() const           { return valueShaper_.getMax(); }

        int getNumSteps() const         { return valueShaper_.getNumSteps(); }
        double getInterval() const      { return valueShaper_.getInterval(); }
    };


    //--------------------------------------------------------------------
    // class ParameterSet
    // A container for Parameters
    //--------------------------------------------------------------------

    class ParameterSet : public std::set < Parameter >
    {
    public:
        void add(const Parameter& p);
        const Parameter& get( int parameterId, int moduleId );
        void removeAllByModule( int moduleId );
        void remove( int id, int moduleId );

        iterator moduleFirst( int moduleId )    { return lower_bound( Parameter( 0, moduleId ) ); }
        iterator moduleLast( int moduleId )     { return lower_bound( Parameter( 0, moduleId+1 ) ); }
    };

} // namespace e3



