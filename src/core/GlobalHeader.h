
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

    //enum ModuleType {
    //    kModuleUndefined = -1,
    //    // system modules
    //    kModuleMaster    = 0,
    //    kModuleMidiGate  = 1,
    //    kModuleMidiPitch = 2,
    //    kModuleMidiInput = 3,
    //    // audio
    //    kModuleSineOscil = 10,
    //    kModuleAdsrEnv   = 11,
    //    kModuleDelay     = 12,
    //};


    //enum ProcessingType
    //{
    //    kProcessEvent = 1,
    //    kProcessControl = 2,
    //    kProcessAudio = 4

    //};

    //enum VoicingType
    //{
    //    kMonophonic = 0,
    //    kPolyphonic = 1
    //};

    //enum ModuleStyle
    //{
    //    kModuleStyleNothing = 0,
    //    kModuleStyleVisible = 1,
    //    kModuleStyleLabel = 2
    //};


    //enum PortType
    //{
    //    kInport  = 0,
    //    kOutport = 1
    //};

    enum NumberFormat
    {
        kNumberInt     = 0,
        kNumberFloat   = 1,
        kNumberDecibel = 2
    };

    enum ControlType
    {
        kControlHidden   = 0,
        kControlSlider   = 1,
        kControlBiSlider = 2,
        kControlCheckbox = 3,
        kControlNumEdit  = 4
    };



    const double PI = 3.14159265358979;
    const double TWO_PI = 2 * PI;
    const double ONE_OVER_128 = 0.0078125;

} // namespace e3

