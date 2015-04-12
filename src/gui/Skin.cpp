
#include "gui/ColourIds.h"
#include "gui/Skin.h"

namespace e3 {

    class SkinDocumentWindowButton : public Button
    {
    public:
        SkinDocumentWindowButton(const String& name, int buttonType) : Button(name), 
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

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkinDocumentWindowButton)
    };

    //--------------------------------------------------------------------
    // class Skin
    //--------------------------------------------------------------------

    Skin::Skin()
    {
        Colour corporateOrangeColour(0xffcf7e2f);
        Colour corporateGreyColour(0xff3e4042);

        Colour backgroundColour(0xff5f5f60);
        Colour contentBackgroundColour(0xffbebdbb);
        Colour darkTextColour(0xffffffff);
        Colour lightTextColour(0xffbdbcba);
        Colour highlightColour(0xffcf732f);
        Colour buttonOffBackgroundColour(0xff3e4042);
        Colour buttonOnBackgroundColour(0xffd08930);
        Colour buttonOffTextColour(0xffbdbcba);
        Colour buttonOnTextColour(0xff000000);


        // juce components
        setColour(ResizableWindow::backgroundColourId, backgroundColour);
        setColour(DocumentWindow::textColourId, lightTextColour);

        setColour(TextButton::buttonColourId, buttonOffBackgroundColour);
        setColour(TextButton::buttonOnColourId, buttonOnBackgroundColour);
        setColour(TextButton::textColourOffId, buttonOffTextColour);
        setColour(TextButton::textColourOnId, buttonOnTextColour);
        
        // custom colours
        setColour(ColourIds::backgroundColourId, backgroundColour);
        setColour(ColourIds::contentBackgroundColourId, contentBackgroundColour);
    }


    int Skin::getTabButtonBestWidth(TabBarButton &, int tabDepth)
    {
        return 100 + tabDepth / 3;
    }


    void Skin::drawDocumentWindowTitleBar(
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


    Button* Skin::createDocumentWindowButton(int buttonType)
    {
        return new SkinDocumentWindowButton("", buttonType);
    }


    void Skin::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown)
    {
        Colour baseColour(backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f));

        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);

        g.setColour(baseColour);
        g.fillRect(button.getLocalBounds());
    }

} // namespace e3