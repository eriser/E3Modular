
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
        int getTabButtonOverlap(int tabDepth) override;
        void drawTabButton(TabBarButton &, Graphics &, bool isMouseOver, bool isMouseDown) override;
        void drawTabbedButtonBarBackground(TabbedButtonBar& buttonBar, Graphics& g) override;
        void drawTabAreaBehindFrontButton(TabbedButtonBar& bar, Graphics& g, int w, int h) override;

        void drawCornerResizer(Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging) override;

        void drawTableHeaderBackground(Graphics& g, TableHeaderComponent& header) override;
        void drawTableHeaderColumn(Graphics& g, const String& columnName, int columnId,
            int width, int height, bool isMouseOver, bool isMouseDown,
            int columnFlags) override;


        enum ColourIds
        {
            kBackgroundColourId             = 0x2000000,
            kContentBackground1ColourId     = 0x2000001,
            kContentBackground2ColourId     = 0x2000002,
            kHighlightColourId              = 0x2000010,
            kTabButtonOffBackgroundColourId = 0x2000020,
            kTabButtonOnBackgroundColourId  = 0x2000021,
            kTabButtonOffTextColourId       = 0x2000022,
            kTabButtonOnTextColourId        = 0x2000023,
        };

    protected:
        Colour modifyButtonBackgroundColour(
            const Colour& colour, bool isMouseOver, bool isMouseDown, bool isEnabled, bool hasFocus);
    };

} // namespace e3