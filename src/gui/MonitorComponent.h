
#pragma once

#include <map>
#include "JuceHeader.h"
#include "core/MonitorUpdater.h"


namespace e3 {

    class MonitorComponent : public Component
    {
    public:
        MonitorComponent();

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