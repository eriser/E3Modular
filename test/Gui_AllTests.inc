

#include "gui/Style.h"
#include "gui/Wires.h"


namespace e3 {
    namespace testing {

        class StyleTest : public ::testing::Test
        {
        public:
            StyleTest() 
            {
                settings_.load();
                XmlElement* e = settings_.getStyleXml();
                style_ = new Style(e);
            }

            TestableSettings settings_;
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
            Rectangle<int> r1(10, 10, 100, 100);
            Wire w(r1.getTopRight(), r1.getBottomLeft());

            Rectangle<int> r2 = w.getBoundingRect();
            EXPECT_EQ(r1, r2);

            Rectangle<int> r3(0, 0, 90, 90);
            EXPECT_NE(r2, r3);
        }

        TEST_F(WireTest, hitTest)
        {
            Rectangle<int> r(10, 10, 100, 100);

            Wire w1(r.getTopRight(), r.getBottomLeft());
            EXPECT_TRUE(w1.hitTest(r));

            Wire w2(Point<int>(0, 20), Point<int>(20, 0));
            EXPECT_TRUE(w2.hitTest(r));

            Wire w3(Point<int>(0, 10), Point<int>(10, 0));
            EXPECT_FALSE(w3.hitTest(r));

            Wire w4(Point<int>(0, 10), Point<int>(10, 10));
            EXPECT_TRUE(w4.hitTest(r));

            Wire w5(Point<int>(0, 9), Point<int>(100, 9));
            EXPECT_FALSE(w5.hitTest(r));

            Wire w6(Point<int>(0, 10), Point<int>(9, 10));
            EXPECT_FALSE(w6.hitTest(r));
        }


    } // namespace testing
} // namespace e3