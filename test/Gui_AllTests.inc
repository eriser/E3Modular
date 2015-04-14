
#include "gui/Style.h"

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
            EXPECT_NE(Colours::black, style_->findColour(Style::kContentBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOffBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOnBackgroundColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOffTextColourId));
            EXPECT_NE(Colours::black, style_->findColour(Style::kTabButtonOnTextColourId));
        }



    } // namespace testing
} // namespace e3