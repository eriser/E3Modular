
#pragma once

#include <cstdint>
#include "e3_Trace.h"


namespace e3 {

    class ProfilerHelper
    {
    public:
        ProfilerHelper::ProfilerHelper() :
            count_(0),
            lastPercent_(-99999.9)
        {}

        void trace(double percent)
        {
            percent = round(percent * 100) / 100.0;
            if (percent != lastPercent_) {
                lastPercent_ = percent;
                TRACE("cpu: %g %%\n", percent);
            }
        }


    private:
        uint32_t count_;
        double lastPercent_;
    };

} // namespace e3;