
#pragma once

#include "JuceHeader.h"
#include "core/Module.h"


namespace e3 {

    class Module;
    class Instrument;

    class Sink : public ModuleList
    {
    public:
        Sink();
        void compile(Instrument* instrument);
        void process(AudioSampleBuffer& audioBuffer, int startFrame, int numFrames);

        void setSampleRate(double sampleRate);

    protected:
        void reset();
        bool contains(Module* module);

        double zero_                 = 0;
        double* audioOutPointer_    = &zero_;
        int16_t frameCounter_        = 0;
        uint16_t controlRateDivisor_ = 1;
    };


    inline void Sink::process(AudioSampleBuffer& audioBuffer, int startFrame, int numFrames)
    {
        int lastFrame = startFrame + numFrames;
        for (int frame = startFrame; frame < lastFrame; frame++)
        {
            if (--frameCounter_ <= 0) {
                frameCounter_ = controlRateDivisor_;
            }
            for (Module** m = _Myfirst; m != _Mylast; m++)
            {
                if (frameCounter_ == controlRateDivisor_)
                {
                    if ((*m)->processingType_ & kProcessControl)
                        (*m)->processControl();
                }

                if ((*m)->processFunction_ != NULL)
                    ((*m)->*(*m)->processFunction_)();
            }

            for (int channel = audioBuffer.getNumChannels(); --channel >= 0;)  {
                audioBuffer.addSample(channel, frame, (float)*audioOutPointer_);  // TODO: use double
            }
        }
    }

} //namespace e3