

//------------------------------------------------------------
// CpuMeter.h
//
// Measures time in microseconds
//------------------------------------------------------------


#pragma once

#include <cstdint>


namespace e3 {

    class CpuTime {
    public:
        CpuTime();

        int_least64_t getTicks() const;
        double ticksToSeconds(int_least64_t ticks) const;

    private:
        int_least64_t frequency_ = 0;
    };


    class CpuMeter
    {
    public:
        CpuMeter(double sampleRate = 44100);
        
        void init(double sampleRate);
        void start();
        double getElapsedSeconds() const;
        bool stop(uint64_t frames);

        void setSampleRate(uint32_t sampleRate)  { init(sampleRate); }
        void setRunsPerCycle(int n)              { runsPerCycle_ = n; }
        double getPercent() const                { return percent_; }


    private:
        double samplePeriod_     = 0;
        double percent_          = 0;
        double lastPercent_      = -1;
        double secondsTotal_     = 0;

        int64_t framesTotal_     = 0;
        int_least64_t startTime_ = 0;
        int numRuns_             = 0;
        int runsPerCycle_        = 50;

        CpuTime clock_;
    };
} // namespace e3