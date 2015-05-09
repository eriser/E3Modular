
#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>

#include "core/GlobalHeader.h"
#include "core/ParameterShaper.h"


namespace e3 {
    
    //--------------------------------------------------------------------
    // class Parameter
    // Parameters control the behaviour of Modules and Links
    //--------------------------------------------------------------------

    class Parameter
    {
        friend class ParameterSet;

    public:
        Parameter() {}
        Parameter( int id, const std::string& label, ControlType controlType, double defaultValue = 1 ); // TODO: remove
        Parameter( int id, int ownerId, const std::string& label="", ControlType controlType=ControlHidden, double defaultValue = 1 );

        bool operator==(const Parameter& other) const
        {
            return
                other.id_ == id_ &&
                other.ownerId_ == ownerId_;
        }
        bool operator==(const Parameter* other) const   { return operator==(*other); }
        bool operator!=(const Parameter& other) const   { return !(*this == other); }
        bool operator!=(const Parameter* other) const   { return !(this == other); }

        bool operator<(const Parameter& other) const
        {
            if (other.ownerId_ == ownerId_) return id_ < other.id_;
            return ownerId_ < other.ownerId_;
        }

        int getId() const               { return id_; }
        int getOwnerId() const          { return ownerId_; }

        ControlType controlType_           = ControlHidden;
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

        bool isValid() const            { return id_ > -1 && ownerId_ > -1; }
        
        double getMin() const           { return valueShaper_.getMin(); }
        double getMax() const           { return valueShaper_.getMax(); }

        int getNumSteps() const         { return valueShaper_.getNumSteps(); }
        double getInterval() const      { return valueShaper_.getInterval(); }

    protected:
        int id_      = 0;
        int ownerId_ = -1;
    };


    //--------------------------------------------------------------------
    // class ParameterSet
    // A container for Parameters
    //--------------------------------------------------------------------

    class ParameterSet : public std::set < Parameter >
    {
    public:
        void add(const Parameter& p);
        const Parameter& get( int parameterId, int ownerId );
        void eraseAllByOwner( int ownerId );

        iterator ownerFirst( int ownerId )    { return lower_bound( Parameter( 0, ownerId ) ); }
        iterator ownerLast( int ownerId )     { return lower_bound( Parameter( 0, ownerId+1 ) ); }
    };

} // namespace e3



