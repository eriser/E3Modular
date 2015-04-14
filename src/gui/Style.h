
#pragma once

#include <string>
#include "JuceHeader.h"


namespace e3 {

    class Style : public LookAndFeel_V3
    {
    public:
        Style(XmlElement* styleXml);
        
        void drawDocumentWindowTitleBar(
            DocumentWindow& window, Graphics& g,
            int w, int h, int titleSpaceX, int titleSpaceW,
            const Image* icon, bool drawTitleTextOnLeft);

        Button* createDocumentWindowButton(int buttonType) override;

        void drawButtonBackground(
            Graphics& g, Button& button, const Colour& backgroundColour,
            bool isMouseOverButton, bool isButtonDown) override;

        int getTabButtonBestWidth(TabBarButton &, int tabDepth) override;
        void drawTabButton(TabBarButton &, Graphics &, bool isMouseOver, bool isMouseDown) override;

        enum ColourIds
        {
            kBackgroundColourId             = 0x2000000,
            kContentBackgroundColourId      = 0x2000001,
            kTabButtonOffBackgroundColourId = 0x2000010,
            kTabButtonOnBackgroundColourId  = 0x2000011,
            kTabButtonOffTextColourId       = 0x2000012,
            kTabButtonOnTextColourId        = 0x2000013,
        };
    };

} // namespace e3