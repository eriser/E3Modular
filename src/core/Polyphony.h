
#pragma once

#include <cstdint>

#include "JuceHeader.h"
#include "core/GlobalHeader.h"
#include <e3_Buffer.h>
#include "core/Voice.h"


namespace e3 {

    //----------------------------------------------------------------------------------
    // class Polyphony
    // Polyphonic voice managment
    //----------------------------------------------------------------------------------
    class Polyphony
    {
    public:
        Polyphony();
        ~Polyphony() {}

        void handleMidiMessage(const MidiMessage& m);

        void startVoice(uint16_t voice, double pitch, double gate);
        void endVoice(uint16_t voice);

        void setNumVoices(uint16_t numVoices);
        uint16_t getNumVoices()                     { return numVoices_; }
        bool isVoiceActive(uint16_t voice) const	{ return voices_[voice].state_ != Voice::kSilent; }
        
        //void setNumUnison(uint16_t numUnison);
        //void setUnisonSpread(uint16_t cent);
        //void setHold(bool hold);
        //void setSustain(bool sustain);
        //void setRetrigger(bool retrigger) { retrigger_ = retrigger; }
        //void setLegato(bool legato);
        //double getPreviousPitch(uint16_t voice);

        VoiceList voices_;
        Buffer< uint16_t > soundingVoices_;

        int16_t numSounding_ = 0;
        int16_t numActive_   = 0;
        int16_t numUnison_   = 1;
        double unisonSpread_ = 5;

        Gallant::Signal2<double, uint16_t>          midiPitchSignal;          // double pitch, uint16_t voice
        Gallant::Signal2<double, uint16_t>          midiGateSignal;           // double gate, uint16_t voice
        Gallant::Signal3<double, double, uint16_t>  midiNoteSignal;           // double pitch, double gate, uint16_t voice
        Gallant::Signal1<int>                       midiPitchbendSignal;      // int value
        Gallant::Signal2<double, int>               midiAftertouchSignal;     // double pitch, int value
        Gallant::Signal2<int, int>                  midiControllerSignal;     // uint16_t controllerNum, double value

        Gallant::Signal4<int, double, double, int>  monitorSignal;

    protected:

        void noteOn(double pitch, double gate);
        void noteOff(double pitch);
        void allNotesOff(bool reset);

        int16_t getUnusedVoice();
        void combineVoices(double basePitch, double gate);
        void updateSoundingVoices();
        void monitorNoteEvent(int16_t numSounding, double pitch, double gate, uint16_t flags) const;
        //void dumpStack(const string& msg);

        uint32_t tags_      = 0;
        bool hold_          = false;
        bool sustain_       = false;
        bool retrigger_     = false;
        bool legato_        = false;
        double lastPitch_   = -1;
        uint16_t numVoices_ = 0;
        double tuning_      = 0;

        std::vector< Voice > stack_;
    };

} // namespace e3

