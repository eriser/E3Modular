
#pragma once

#include <map>
#include "JuceHeader.h"
#include "core/VoiceMonitorCore.h"


namespace e3 {

    class VoiceMonitor : public Component
    {
    public:
        VoiceMonitor();

        void resized() override;
        void monitor(const VoiceMonitorEvent& event);

    private:
        enum LabelIds {
            kType,
            kValue1,
            kValue2,
            kVoices
        };

        typedef std::map<int, ScopedPointer<Label> > LabelMap;
        LabelMap labels_;
    };

} // namespace e3