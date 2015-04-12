
#pragma once
#include "JuceHeader.h"

namespace e3
{
    class EditorPanel : public Component
    {
    public:
        EditorPanel();

        void paint(Graphics& g) override;
    };


} // namespace e3