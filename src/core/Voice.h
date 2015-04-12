
#pragma once

#include <cstdint>
#include <vector>


namespace e3 {

    class Voice
    {
    public:
        enum State
        {
            kSilent = 0,
            kNoteOff = 1,
            kNoteOn = 2,
            kNoteHold = 4,
        };

        Voice() = default;
        Voice(int16_t id, uint16_t state, double pitch, double gate, int32_t tag, int32_t unisonGroup);
        void reset();
        void init(int16_t id, uint16_t state, double pitch, double gate, int32_t tag);

        int16_t id_          = -1;
        uint16_t state_      = kSilent;
        double pitch_        = -1;
        double gate_         = -1;
        int32_t tag_         = -1;
        int32_t unisonGroup_ = -1;
    };
    typedef std::vector< Voice > VoiceList;

} // namespace e3