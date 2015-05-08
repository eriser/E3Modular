

#pragma once

#include <string>
#include "core/Module.h"


namespace e3 {

    class SineOscillator : public Module 
    {
    public:
        SineOscillator();

        ParameterSet& getDefaultParameters() const override;
        void initData() override;
        void updatePorts() override;

        void setParameter(int paramId, double value, double modulation=0.f, int voice=-1) override;

        void processAudio() throw();
        void processAudioFm();
        void processAudioAm();
        void processAudioFmAm();

        enum ParamId {
            ParamFrequency   = 0,
            ParamAmplitude   = 1,
            ParamTuning      = 2,
            ParamFinetuning  = 3,
        };

        std::string debugLabel_ = "SineOscillator";

    protected:
        void setSampleRate(double sampleRate) override;
        void setTuning( double paramValue );
        void setFineTuning( double paramValue );
        void setIncrement( int_fast32_t voice );

        void makeWaveTable();

        Buffer<double> incrementBuffer_, phaseIndexBuffer_, amplitudeBuffer_, frequencyBuffer_;
        double *phaseIndex_, *amplitude_, *increment_, *freq_;
        
        double tuning_ = 1;
        double fineTuning_ = 1;

        Inport  freqInport_;
        Inport  ampInport_;
        Outport audioOutport_;
        double* freqInportPointer_ = nullptr;
        double* ampInportPointer_  = nullptr;
        
        static Buffer<double> tableBuffer_;
        static double* table_;
        static uint_fast32_t tableSize_; // = 2048;
    };


    inline void SineOscillator::setIncrement(int_fast32_t voice)
    {
        increment_[voice] = (tableSize_ * freq_[voice] * tuning_ * fineTuning_) / sampleRate_;
    }

} // namespace e3

