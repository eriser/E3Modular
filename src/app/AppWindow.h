
#pragma once

#include "JuceHeader.h"


namespace e3 {

    class AppWindow : public DocumentWindow 
    {
    public:
        AppWindow(const String& title);
        ~AppWindow() {}

        void closeButtonPressed() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppWindow)
    };

} // namespace e3
