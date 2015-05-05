
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
            MonitorType,
            MonitorValue1,
            MonitorValue2,
            MonitorVoices,
            MonitorCpuMeter
        };

        typedef std::map<int, ScopedPointer<Label> > LabelMap;
        LabelMap labels_;
    };

} // namespace e3