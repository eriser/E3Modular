
#include <string>
#include <map>
#include <e3_Exception.h>
#include <e3_Utilities.h>
#include "gui/Style.h"


namespace e3 {

    class StyleDocumentWindowButton : public Button
    {
    public:
        StyleDocumentWindowButton(const String& name, int buttonType) : Button(name), 
            buttonType_(buttonType)
        {}

        void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
        {
            ResizableWindow* parent = findParentComponentOfClass<ResizableWindow>();
            Colour highlight = parent ? parent->getBackgroundColour() : Colour(0xffbebdbb);

            Colour orange(0xffd08930);
            Colour grey(0xff3e4042);

            if (!isEnabled()) {
                grey = grey.withAlpha(0.6f);
                orange = orange.withAlpha(0.6f);
            }

            const float x = getWidth()  * 0.15f;
            const float y = getHeight() * 0.3f;
            const float w = jmin(getWidth(), getHeight()) * (isButtonDown ? 0.65f : 0.7f);

            if (isMouseOverButton) {
                g.setColour(highlight.brighter());
                g.fillRect(0, 0, getWidth(), getHeight());
            }

            switch (buttonType_)
            {
            case DocumentWindow::minimiseButton:
                g.setColour(grey);
                g.fillRect(x, y, w, w);
                g.setColour(orange);
                g.fillRect(x, y + w * 0.75f, w, w * 0.25f);
                break;
            case DocumentWindow::maximiseButton:
            {
                bool isFullscreen = getToggleState();
                Colour inner      = isFullscreen ? orange : grey;
                Colour outer      = isFullscreen ? grey : orange;

                g.setColour(outer);
                g.fillRect(x, y, w, w);
                g.setColour(inner);
                g.fillRect(x + w * 0.25f, y + w*0.25f, w*0.5f, w*0.5f);
                break;
            }
            case DocumentWindow::closeButton:
            {
                g.setColour(orange);
                g.fillRect(x, y, w, w);

                g.setColour(grey);
                Path shape;
                shape.addLineSegment(Line<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.30f);
                shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 1.0f), 0.30f);
                g.fillPath(shape, shape.getTransformToScaleToFit(
                    x + w * 0.15f, y + w * 0.15f, w * 0.7f, w * 0.7f, true));
                break;
            }
            }
        }

    private:
        int buttonType_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleDocumentWindowButton)
    };

    //--------------------------------------------------------------------
    // class Style
    //--------------------------------------------------------------------

    Style::Style(XmlElement* styleXml)
    {
        ASSERT(styleXml);
        std::map<std::string, Colour> colors;
        forEachXmlChildElementWithTagName(*styleXml, e, "Color")
        {
            std::string name = e->getStringAttribute("name").toStdString();
            std::string hex  = e->getStringAttribute("argb").toStdString();

            colors.insert(std::make_pair(name, Colour(common::HexToInt(hex))));
        }

        //Colour backgroundColour(0xff5f5f60);
        //Colour contentBackgroundColour(0xffbebdbb);
        //Colour darkTextColour(0xffffffff);
        //Colour lightTextColour(0xffbdbcba);
        //Colour highlightColour(0xffcf732f);
        //Colour buttonOffBackgroundColour(0xff3e4042);
        //Colour buttonOnBackgroundColour(0xffd08930);
        //Colour buttonOffTextColour(0xffbdbcba);
        //Colour buttonOnTextColour(0xff000000);


        // juce components
        setColour(ResizableWindow::backgroundColourId, colors["background"]);
        setColour(DocumentWindow::textColourId, colors["lightText"]);

        setColour(TextButton::buttonColourId, colors["buttonOffBackground"]);
        setColour(TextButton::buttonOnColourId, colors["buttonOnBackground"]);
        setColour(TextButton::textColourOffId, colors["buttonOffText"]);
        setColour(TextButton::textColourOnId, colors["buttonOnText"]);
        
        // custom colours
        setColour(kBackgroundColourId, colors["background"]);
        setColour(kContentBackgroundColourId, colors["contentBackground"]);
        setColour(kTabButtonOffBackgroundColourId, colors["buttonOffBackground"]);
        setColour(kTabButtonOnBackgroundColourId, colors["buttonOnBackground"]);
        setColour(kTabButtonOffTextColourId, colors["buttonOffText"]);
        setColour(kTabButtonOnTextColourId, colors["buttonOnText"]);
    }


    void Style::drawDocumentWindowTitleBar(
        DocumentWindow& window, Graphics& g,
        int w, int h, int titleSpaceX, int titleSpaceW,
        const Image* icon, bool drawTitleTextOnLeft)
    {
        const bool isActive = window.isActiveWindow();

        g.fillAll(window.getBackgroundColour());

        Font font(h * 0.65f, Font::bold);
        g.setFont(font);

        int textW = font.getStringWidth(window.getName());
        int iconW = 0;
        int iconH = 0;

        if (icon != nullptr)
        {
            iconH = (int)font.getHeight();
            iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
        }

        textW = jmin(titleSpaceW, textW + iconW);
        int textX = drawTitleTextOnLeft ? titleSpaceX : jmax(titleSpaceX, (w - textW) / 2);

        if (textX + textW > titleSpaceX + titleSpaceW)
            textX = titleSpaceX + titleSpaceW - textW;

        if (icon != nullptr)
        {
            g.setOpacity(isActive ? 1.0f : 0.6f);
            g.drawImageWithin(*icon, textX, (h - iconH) / 2, iconW, iconH,
                RectanglePlacement::centred, false);
            textX += iconW + 10;
            textW -= iconW;
        }

        if (window.isColourSpecified(DocumentWindow::textColourId) || isColourSpecified(DocumentWindow::textColourId))
            g.setColour(window.findColour(DocumentWindow::textColourId));
        else
            g.setColour(window.getBackgroundColour().contrasting(isActive ? 0.7f : 0.4f));

        g.drawText(window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
    }


    Button* Style::createDocumentWindowButton(int buttonType)
    {
        return new StyleDocumentWindowButton("", buttonType);
    }


    void Style::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown)
    {
        Colour baseColour(backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f));

        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);

        g.setColour(baseColour);
        g.fillRect(button.getLocalBounds());
    }


    int Style::getTabButtonBestWidth(TabBarButton &, int tabDepth)
    {
        return 100 + tabDepth / 3;
    }

    void Style::drawTabButton(TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
    { 
        const Rectangle<int> activeArea(button.getActiveArea());
        const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();
        
        //const Colour bkgnd(button.getTabBackgroundColour());
        int id = button.getToggleState() ? kTabButtonOnBackgroundColourId : kTabButtonOffBackgroundColourId;
        const Colour bkgndColour = findColour(id);

        //if (button.getToggleState()) {
        //    g.setColour(bkgnd);
        //}
        //else
        //{
            //Point<int> p1, p2;

            //switch (o)
            //{
            //case TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
            //case TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
            //case TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
            //case TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
            //default:                              jassertfalse; break;
            //}

            //g.setGradientFill(ColourGradient(bkgnd.brighter(0.2f), (float)p1.x, (float)p1.y,
            //    bkgnd.darker(0.1f), (float)p2.x, (float)p2.y, false));
        //}

        g.setColour(bkgndColour);
        g.fillRect(activeArea);

        g.setColour(button.findColour(TabbedButtonBar::tabOutlineColourId));

        Rectangle<int> r(activeArea);

        if (o != TabbedButtonBar::TabsAtBottom)   g.fillRect(r.removeFromTop(1));
        if (o != TabbedButtonBar::TabsAtTop)      g.fillRect(r.removeFromBottom(1));
        if (o != TabbedButtonBar::TabsAtRight)    g.fillRect(r.removeFromLeft(1));
        if (o != TabbedButtonBar::TabsAtLeft)     g.fillRect(r.removeFromRight(1));

        const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

        Colour col(bkgndColour.contrasting().withMultipliedAlpha(alpha));

        if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
        {
            TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId
                : TabbedButtonBar::tabTextColourId;

            if (bar->isColourSpecified(colID))
                col = bar->findColour(colID);
            else if (isColourSpecified(colID))
                col = findColour(colID);
        }

        const Rectangle<float> area(button.getTextArea().toFloat());

        float length = area.getWidth();
        float depth = area.getHeight();

        if (button.getTabbedButtonBar().isVertical())
            std::swap(length, depth);

        TextLayout textLayout;
        createTabTextLayout(button, length, depth, col, textLayout);

        AffineTransform t;

        switch (o)
        {
        case TabbedButtonBar::TabsAtLeft:   t = t.rotated(float_Pi * -0.5f).translated(area.getX(), area.getBottom()); break;
        case TabbedButtonBar::TabsAtRight:  t = t.rotated(float_Pi *  0.5f).translated(area.getRight(), area.getY()); break;
        case TabbedButtonBar::TabsAtTop:
        case TabbedButtonBar::TabsAtBottom: t = t.translated(area.getX(), area.getY()); break;
        default:                            jassertfalse; break;
        }

        g.addTransform(t);
        textLayout.draw(g, Rectangle<float>(length, depth));
    }




} // namespace e3