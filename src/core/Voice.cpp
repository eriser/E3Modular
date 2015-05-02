
#include "core/Voice.h"


namespace e3 {

    Voice::Voice(int id, int state, double pitch, double gate, int tag, int unisonGroup)
    {
        init(id, state, pitch, gate, tag);
        unisonGroup_ = unisonGroup;
    }


    void Voice::reset()
    {
        id_          = -1;
        state_       = Silent;
        pitch_       = -1;
        gate_        = -1;
        tag_         = -1;
        unisonGroup_ = -1;
    }


    void Voice::init(int id, int state, double pitch, double gate, int tag)
    {
        id_    = id;
        state_ = state;
        pitch_ = pitch;
        gate_  = gate;
        tag_   = tag;
    }

} // namespace e3