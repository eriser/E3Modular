
#pragma once
#include "JuceHeader.h"

namespace e3
{
    class BrowserPanel : public Component
    {
    public:
        BrowserPanel();

        void resized() override;

    protected:
        ScopedPointer<TabbedComponent> tabPanel_;
        ScopedPointer<Component> bankBrowser_;
        ScopedPointer<Component> instrumentBrowser_;
    };


} // namespace e3