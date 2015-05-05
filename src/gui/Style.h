
#pragma once

#include <string>
#include <map>
#include "JuceHeader.h"


namespace e3 {

    class Style : public LookAndFeel_V3
    {
    public:
        static Style& getInstance();

        void setXml( XmlElement* styleXml );

        void drawDocumentWindowTitleBar(
            DocumentWindow& window, Graphics& g,
            int w, int h, int titleSpaceX, int titleSpaceW,
            const Image* icon, bool drawTitleTextOnLeft ) override;

        Button* createDocumentWindowButton( int buttonType ) override;

        //void fillTextEditorBackground( Graphics& g, int width, int height, TextEditor& textEditor ) override;

        void drawButtonBackground(
            Graphics& g, Button& button, const Colour& backgroundColour,
            bool isMouseOverButton, bool isButtonDown ) override;

        void drawToggleButton( Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown ) override;
        void drawTickBox( Graphics& g, Component& component,
                          float x, float y, float w, float h,
                          bool ticked,
                          bool isEnabled,
                          bool isMouseOverButton,
                          bool isButtonDown ) override;

        int getTabButtonBestWidth( TabBarButton &, int tabDepth ) override;
        int getTabButtonOverlap( int tabDepth ) override;
        void drawTabButton( TabBarButton &, Graphics &, bool isMouseOver, bool isMouseDown ) override;
        void drawTabbedButtonBarBackground( TabbedButtonBar& buttonBar, Graphics& g ) override;
        void drawTabAreaBehindFrontButton( TabbedButtonBar& bar, Graphics& g, int w, int h ) override;

        void drawCornerResizer( Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging ) override;

        void drawTableHeaderBackground( Graphics& g, TableHeaderComponent& header ) override;
        void drawTableHeaderColumn( Graphics& g, const String& columnName, int columnId,
            int width, int height, bool isMouseOver, bool isMouseDown,
            int columnFlags ) override;

        enum ColourIds
        {
            BackgroundColourId              = 0x2000000,
            ContentBackground1ColourId      = 0x2000001,
            ContentBackground2ColourId      = 0x2000002,
            HighlightColourId               = 0x2000010,
            TextEditorFocusedColourId       = 0x2000020,
            TabButtonOffBackgroundColourId  = 0x2000030,
            TabButtonOnBackgroundColourId   = 0x2000031,
            TabButtonOffTextColourId        = 0x2000032,
            TabButtonOnTextColourId         = 0x2000033,
            ToggleButtonBackgroundColourId  = 0x2000040,
            ToggleButtonTickColourId        = 0x2000041,

            ModuleColourId                  = 0x2000050,
            ModuleMonoColourId              = 0x2000051,
            ModulePort1ColourId             = 0x2000052,
            ModulePort2ColourId             = 0x2000053,
            ModuleText1ColourId             = 0x2000054,
            ModuleText2ColourId             = 0x2000055,
            ModuleBorderNormalColourId      = 0x2000056,
            ModuleBorderSelectedColourId    = 0x2000057,
            ModuleBorderFocusedColourId     = 0x2000058,
            Wire1ColourId                   = 0x2000059,
            Wire2ColourId                   = 0x200005a,
        };

    protected:
        XmlElement* getDefaultXml() const;

        Colour modifyButtonBackgroundColour(
            const Colour& colour, bool isMouseOver, bool isMouseDown, bool isEnabled, bool hasFocus );

        std::string defaultStyleXml_ =
            "<Style name = 'Default'>"
            "<Color argb = 'ff5f5f60' name = 'background' />"
            "<Color argb = 'ffbebdbb' name = 'contentBackground1' />"
            "<Color argb = 'ff808080' name = 'contentBackground2' />"
            "<Color argb = 'ff000000' name = 'darkText' />"
            "<Color argb = 'ffbdbcba' name = 'lightText' />"
            "<Color argb = 'ffd08930' name = 'highlight' />"
            "<Color argb = 'ff3e4042' name = 'buttonOffBackground' />"
            "<Color argb = 'ffd08930' name = 'buttonOnBackground' />"
            "<Color argb = 'ffbdbcba' name = 'buttonOffText' />"
            "<Color argb = 'ff000000' name = 'buttonOnText' />"
            "<Color argb = 'cc8e8e8e' name = 'module' />"
            "<Color argb = 'ff4d5f53' name = 'moduleMono' />"
            "<Color argb = 'ff666666' name = 'modulePort1' />"
            "<Color argb = 'ffd08930' name = 'modulePort2' />"
            "<Color argb = 'ff555555' name = 'moduleText1' />"
            "<Color argb = 'ffA21D08' name = 'moduleText2' />"
            "<Color argb = 'ff6f6f6f' name = 'moduleBorderNormal' />"
            "<Color argb = 'ffd08930' name = 'moduleBorderSelected' />"
            "<Color argb = 'ffa21d08' name = 'moduleBorderFocused' />"
            "<Color argb = 'ff777777' name = 'wire1' />"
            "<Color argb = 'ffd08930' name = 'wire2' />"
            "</ Style>";
    };


    class StyleDocumentWindowButton : public Button
    {
    public:
        StyleDocumentWindowButton( const String& name, int buttonType ) : Button( name ),
            buttonType_( buttonType )
        {}

        void paintButton( Graphics& g, bool isMouseOver, bool isButtonDown ) override;

    private:
        int buttonType_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( StyleDocumentWindowButton )
    };



} // namespace e3