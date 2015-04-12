
#pragma once

#include "JuceHeader.h"

namespace e3 {

    struct Skin : public LookAndFeel_V3
    {
        Skin();
        
        int getTabButtonBestWidth(TabBarButton &, int tabDepth) override;
        void drawDocumentWindowTitleBar(
            DocumentWindow& window, Graphics& g,
            int w, int h, int titleSpaceX, int titleSpaceW,
            const Image* icon, bool drawTitleTextOnLeft);

        Button* createDocumentWindowButton(int buttonType) override;

        void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
            bool isMouseOverButton, bool isButtonDown) override;
    };

} // namespace e3