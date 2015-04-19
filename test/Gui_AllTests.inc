
#include "gui/Style.h"
#include "gui/Wire.h"


namespace e3 {
    namespace testing {

        class TestableStyle : public Style
        {
        public:
        };

        class StyleTest : public ::testing::Test
        {
        public:
            StyleTest() 
            {
                settings_.load();
                XmlElement* e = settings_.getStyle();
                style_ = new Style(e);
            }

            Settings settings_;
            ScopedPointer<Style> style_;
        };

        TEST_F(StyleTest, FindColours)
        {
            EXPECT_NE(Colours::black, style_->findColour(Style::kBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kContentBackground1ColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kContentBackground2ColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOffBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOnBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOffTextColourId));
            EXPECT_EQ(Colours::black, style_->findColour(Style::kTabButtonOnTextColourId));
        }


        // WireTest
        using juce::Rectangle;

        class WireTest : public ::testing::Test
        {};


        TEST_F(WireTest, getBoundingRect)
        {
            Rectangle<float> r1(10, 10, 100, 100);
            Wire w(r1.getTopRight(), r1.getBottomLeft());

            Rectangle<float> r2;
            w.getBoundingRect(r2);
            EXPECT_EQ(r1, r2);

            Rectangle<float> r3(0, 0, 90, 90);
            EXPECT_NE(r2, r3);
        }

        TEST_F(WireTest, hitTest)
        {
            Rectangle<float> r(10, 10, 100, 100);

            Wire w1(r.getTopRight(), r.getBottomLeft());
            EXPECT_TRUE(w1.hitTest(r));

            Wire w2(Point<float>(0, 20), Point<float>(20, 0));
            EXPECT_TRUE(w2.hitTest(r));

            Wire w3(Point<float>(0, 10), Point<float>(10, 0));
            EXPECT_FALSE(w3.hitTest(r));

            Wire w4(Point<float>(0, 10), Point<float>(10, 10));
            EXPECT_TRUE(w4.hitTest(r));

            Wire w5(Point<float>(0, 9), Point<float>(100, 9));
            EXPECT_FALSE(w5.hitTest(r));

            Wire w6(Point<float>(0, 10), Point<float>(9, 10));
            EXPECT_FALSE(w6.hitTest(r));
        }


    } // namespace testing
} // namespace e3