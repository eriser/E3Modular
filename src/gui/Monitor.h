
#pragma once

#include <map>
#include "JuceHeader.h"
#include "core/MonitorCore.h"


namespace e3 {

    class Monitor : public Component
    {
    public:
        Monitor();

        void resized() override;
        void monitor(MonitorEvent e);

    private:
        enum LabelIds {
            kType,
            kValue1,
            kValue2,
            kVoices,
            kCpuMeter
        };

        typedef std::map<int, ScopedPointer<Label> > LabelMap;
        LabelMap labels_;
    };

} // namespace e3