
#include "gui/TabComponent.h"


namespace e3 {

    void TabComponent::resized()
    {
        TabbedComponent::resized();

        TabbedButtonBar::Orientation o = getOrientation();
        if (o == TabbedButtonBar::TabsAtTop || o == TabbedButtonBar::TabsAtBottom)
        {
            Rectangle<int> contentArea(getLocalBounds());
            Rectangle<int> tabArea;
            int tabDepth = getTabBarDepth();

            switch (o) {
            case TabbedButtonBar::TabsAtTop:
                tabArea = contentArea.removeFromTop(tabDepth);
                contentArea = contentArea.withTrimmedTop(gap_);
                tabs->setBounds(tabArea);
                break;
            case TabbedButtonBar::TabsAtBottom:
                tabArea = contentArea.removeFromBottom(tabDepth);
                contentArea = contentArea.withTrimmedBottom(gap_);
                tabs->setBounds(tabArea);
                break;
            }

            for (int i = 0; i < getNumTabs(); i++) {
                Component* c = getTabContentComponent(i);
                c->setBounds(contentArea);
            }
        }
    }


} // namespace e3