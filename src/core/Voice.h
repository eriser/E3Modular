
#pragma once

#include <vector>


namespace e3 {

    class Voice
    {
    public:
        enum State
        {
            Silent = 0,
            NoteOff = 1,
            NoteOn = 2,
            NoteHold = 4,
        };

        Voice() = default;
        Voice(int id, int state, double pitch, double gate, int tag, int unisonGroup);
        void reset();
        void init(int id, int state, double pitch, double gate, int tag);

        int id_           = -1;
        int state_        = Silent;
        double pitch_     = -1;
        double gate_      = -1;
        int tag_          = -1;
        int unisonGroup_  = -1;
    };
    typedef std::vector< Voice > VoiceList;

} // namespace e3