
#pragma once

#include <cstdint>

#include "JuceHeader.h"
#include "core/GlobalHeader.h"
#include <e3_Buffer.h>
#include "core/Voice.h"
#include "core/MonitorUpdater.h"


namespace e3 {

    //----------------------------------------------------------------------------------
    // class Polyphony
    // Polyphonic voice managment
    //----------------------------------------------------------------------------------
    class Polyphony : public MonitorUpdater
    {
    public:
        Polyphony();
        ~Polyphony() {}

        void handleMidiMessage(const MidiMessage& m);

        void startVoice(int voice, double pitch, double gate);
        void endVoice(int voice);

        void setNumVoices(int numVoices);
        int getNumVoices() const                    { return numVoices_; }
        bool isVoiceActive(int voice) const	        { return voices_[voice].state_ != Voice::Silent; }
        
        //void setNumUnison(uint16_t numUnison);
        //void setUnisonSpread(uint16_t cent);
        //void setHold(bool hold);
        //void setSustain(bool sustain);
        //void setRetrigger(bool retrigger) { retrigger_ = retrigger; }
        //void setLegato(bool legato);
        //double getPreviousPitch(uint16_t voice);

        VoiceList voices_;
        Buffer< int > soundingVoices_;

        int numSounding_     = 0;
        int numActive_       = 0;
        int numUnison_       = 1;
        double unisonSpread_ = 5;

        Gallant::Signal2<double, int>          midiPitchSignal;          // double pitch, uint16_t voice
        Gallant::Signal2<double, int>          midiGateSignal;           // double gate, uint16_t voice
        Gallant::Signal3<double, double, int>  midiNoteSignal;           // double pitch, double gate, uint16_t voice
        Gallant::Signal1<int>                  midiPitchbendSignal;      // int value
        Gallant::Signal2<double, int>          midiAftertouchSignal;     // double pitch, int value
        Gallant::Signal2<int, int>             midiControllerSignal;     // uint16_t controllerNum, double value

    protected:

        void noteOn(double pitch, double gate);
        void noteOff(double pitch);
        void allNotesOff(bool reset);

        int getUnusedVoice();
        void combineVoices(double basePitch, double gate);
        void updateSoundingVoices();

        //void dumpStack(const string& msg);

        uint32_t tags_    = 0;
        bool hold_        = false;
        bool sustain_     = false;
        bool retrigger_   = false;
        bool legato_      = false;
        double lastPitch_ = -1;
        int numVoices_    = 0;
        double tuning_    = 0;

        std::vector< Voice > stack_;

    };

} // namespace e3

