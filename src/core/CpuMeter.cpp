
#include <cmath>
#include <Windows.h>
#include "core/CpuMeter.h"


namespace e3 {

    //------------------------------------------------------------------------
    // class CpuTime
    //------------------------------------------------------------------------

    CpuTime::CpuTime()
    {
        if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_)) {
            frequency_ = 0;
        }
    }


    int_least64_t CpuTime::getTicks() const
    {
        int_least64_t ticks;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&ticks);

        return ticks;
    }


    double CpuTime::ticksToSeconds(int_least64_t ticks) const
    {
        return ticks / (double)frequency_;
    }


    //------------------------------------------------------------------------
    // class CpuMeter
    //------------------------------------------------------------------------
    
    CpuMeter::CpuMeter(double sampleRate)
    {
        init(sampleRate);
    }


    void CpuMeter::init(double sampleRate)
    {
        samplePeriod_ = 1 / sampleRate;
        framesTotal_ = 0;
        secondsTotal_ = 0;
        numRuns_ = 0;
    }


    void CpuMeter::start()
    {
        startTime_ = clock_.getTicks();
    }


    double CpuMeter::getElapsedSeconds() const
    {
        int_least64_t ticks = clock_.getTicks() - startTime_;
        return clock_.ticksToSeconds(ticks);
    }


    bool CpuMeter::stop(uint64_t frames)
    {
        secondsTotal_ += clock_.ticksToSeconds(clock_.getTicks() - startTime_);
        framesTotal_  += frames;

        if (++numRuns_ < runsPerCycle_) {
            return false;
        }
        percent_ = (secondsTotal_ / (framesTotal_ * samplePeriod_)) * 100.0;
        percent_ = round(percent_ * 100) / 100.0;

        secondsTotal_ = 0;
        framesTotal_  = 0;
        numRuns_      = 0;

        if (percent_ != lastPercent_) {
            lastPercent_ = percent_;
            return true;
        }
        return false;
    }


} // namespace e3