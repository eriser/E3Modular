


#include <math.h>
#include <e3_Exception.h>
#include <e3_Math.h>

#include "core/ParameterShaper.h"


namespace e3 {


    double ParameterShaper::exponential(double value) const
    {
        double v = bound(value);
        v = normalize(value);

        v = v * pow(base_, (1 - v) * -factor_);

        v = validate(v);
        return denormalize(v);
    }


    double ParameterShaper::linear(double value) const
    {
        double v = bound(value);
        v = normalize(value);

        if (factor_ != 0) {                         // factor=0: linear
            double f = factor_ * log(base_);
            v = LambertW(pow(base_, factor_) * v * f) / f;
        }

        v = validate(v);
        return denormalize(v);
    }


    double ParameterShaper::normalize(double value) const
    { 
        return (value - min_) / (-min_ + max_);
    } 
    
    double ParameterShaper::denormalize(double value) const
    { 
        return value * (-min_ + max_) + min_;
    }    
 
    
    double ParameterShaper::validate(double value) const
    { 
        UNDENORMALIZE(value);       // avoid numbers very close to zero
        return value = (isinf(value) || isnan(value)) ? min_ : value;
    }


    //--------------------------------------------------------------------------------------
    // class MidiParameterValue
    //--------------------------------------------------------------------------------------
    /*
    bool MidiParameterValue::scaleController(int16_t controllerId, double& value)
    {
        if (controller_ == -1 || controller != controller_)
            return false;

        value = normalize(value, controllerMin_, controllerMax_);
        //if (controllerMax_ < controllerMin_)                   // reverse range if min > max
        //    value = controllerMin_ - value;

        //FLOAT range = abs(controllerMax_ - controllerMin_);
        //value /= range;                                         // normalize to 1

        if (max_ < min_)
            value = min_ - value;

        range = abs(max_ - min_);
        FLOAT raster = range / numSteps_;
        value *= range;
        value = makeLogarithmic(value);                // apply logarithmic scaling factor

        if (numSteps_ > 0) {                                   // rasterize
            value = (INT16)(value / raster) * raster;
        }

        if (controllerSoft_ && controllerAccepted_ == false)   // soft takeover
        {
            FLOAT diff = abs(value_ - value);
            if (diff > raster * 2) {
                return false;
            }
            controllerAccepted_ = true;
        }
        return true;
    }
    */
    



} // namespace e3;