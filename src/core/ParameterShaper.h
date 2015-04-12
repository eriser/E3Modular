
#pragma once

#include <cstdint>
#include <e3_Math.h>

namespace e3 {

    //------------------------------------------------------------------------
    // class ParameterShaper
    // Helper class to perform shaping and quantizing on a parameter.
    //------------------------------------------------------------------------

    class ParameterShaper : public BoundedRange<double>
    {
    public:

        //---------------------------------------------------------------------------
        // Constructor
        // @param factor Specifies how a value is shaped.
        //               Positive values create a convex curve, 
        //               negative values create a concave curve.
        //               Useful values are between -6 and 24.
        // @param steps  Quantize value
        ParameterShaper(double min=0, double max=1, int16_t steps=100, int16_t factor=12) :
            BoundedRange(min, max),
            steps_(steps),
            factor_(factor)
        {}

        // Returns an exponentially scaled value of the ParameterValue.
        double exponential(double value) const;

        // Converts the given value to a linear scale.
        double linear(double value) const;

        int16_t getFactor() const         { return factor_; }
        void setFactor(int16_t factor)    { factor_ = factor; }

        int16_t getSteps() const          { return steps_; }
        void setSteps(int16_t steps)      { steps_ = steps; }

    protected:
        
        double normalize(double value) const;
        double denormalize(double value) const;
        double validate(double value) const;


        double base_    = 1.122;
        int16_t factor_ = 12;
        int16_t steps_  = 12;
    };


    //----------------------------------------------------------------------------------------
    // class MidiParameterValue
    // Handles parameters than are controllable by Midi continous controllers.
    //----------------------------------------------------------------------------------------
    class MidiParameterShaper : public ParameterShaper
    {
    public:
        MidiParameterShaper(int16_t controllerId=-1, double controllerMin=0, double controllerMax=127, bool softTakeover=false) :
            controllerId_(controllerId),
            controllerMin_(controllerMin),
            controllerMax_(controllerMax),
            softTakeover_(softTakeover)
        {}

        int16_t getControllerId() const { return controllerId_; }
        double getControllerMin() const { return controllerMin_; }
        double getControllerMax() const { return controllerMax_; }
        bool getSoftTakeover() const    { return softTakeover_; }

        void setControllerId(int16_t id)  { controllerId_ = id; }
        void setControllerMin(double min) { controllerMin_ = min; }
        void setControllerMax(double max) { controllerMax_ = max; }
        void setSoftTakeover(bool soft)   { softTakeover_ = soft; }

        //MidiParameterShaper() : ParameterShaper() {}
        //MidiParameterShaper(const ParameterShaper& other) : ParameterShaper(other) {}
    protected:
        int16_t controllerId_;
        double controllerMin_;
        double controllerMax_;
        bool softTakeover_;

    private:
        bool controllerAccepted_ = false;

    };

} // namespace e3