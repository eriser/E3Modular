
// Global includes and definitions

#pragma once


#include <cstdint>

#pragma warning(push)
#pragma warning(disable: 4100) 
#include "GallantSignal.h"
#pragma warning(pop)

#define INITIAL_SAMPLERATE 44100
#define INITIAL_CONTROLRATE 400
#define NUMPROGRAMS 128
#define NUMINPUTS 0
#define NUMOUTPUTS 2


namespace e3 {

    enum NumberFormat
    {
        NumberInt     = 0,
        NumberFloat   = 1,
        NumberDecibel = 2
    };

    enum ControlType
    {
        ControlHidden   = 0,
        ControlSlider   = 1,
        ControlBiSlider = 2,
        ControlCheckbox = 3,
        ControlNumEdit  = 4
    };



    const double PI = 3.14159265358979;
    const double TWO_PI = 2 * PI;
    const double ONE_OVER_128 = 0.0078125;

} // namespace e3

