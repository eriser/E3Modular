

#pragma once

#include "core/Module.h"


namespace e3 {

    class SineOscil : public Module 
    {
    public:
        SineOscil();

        void initVoices() override;
        void updatePorts() override;
        void updateInports() override;
        void setParameter( uint16_t paramId, double value, double modulation=0.f, int16_t voice=-1 );

        void processAudio() throw();
        void processAudioFm();
        void processAudioAm();
        void processAudioFmAm();

        enum ParamId {
            kParamFrequency   = 0,
            kParamAmplitude   = 1,
            kParamTuning      = 2,
            kParamFinetuning  = 3,
        };

    protected:
        void setSampleRate(double sampleRate) override;
        void setTuning( double paramValue );
        void setFineTuning( double paramValue );
        void setIncrement( uint16_t voice );

        void makeWaveTable();

        Buffer<double> incrementBuffer_, phaseIndexBuffer_, amplitudeBuffer_, frequencyBuffer_;
        double *phaseIndex_, *amplitude_, *increment_, *freq_;
        
        double tuning_ = 1;
        double fineTuning_ = 1;

        AudioInport  fmInport_;
        AudioInport  amInport_;
        AudioOutport audioOutport_;
        double* fmInportPointer_ = nullptr;
        double* amInportPointer_ = nullptr;
        
        static Buffer<double> tableBuffer_;
        static double* table_;
        static uint16_t tableSize_; // = 2048;
    };


    inline void SineOscil::setIncrement(uint16_t voice)
    {
        increment_[voice] = (tableSize_ * freq_[voice] * tuning_ * fineTuning_) / sampleRate_;
    }

} // namespace e3

