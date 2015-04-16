
#include "gui/TabComponent.h"


namespace e3 {

    void TabComponent::resized()
    {
        TabbedComponent::resized();

        Rectangle<int> contentArea(getLocalBounds());
        Rectangle<int> tabArea;
        int tabDepth = getTabBarDepth();

        switch (getOrientation()) {
        case TabbedButtonBar::TabsAtTop: 
            tabArea = contentArea.removeFromTop(tabDepth);
            contentArea = contentArea.withTrimmedTop(gap_);
            break;
        case TabbedButtonBar::TabsAtBottom: 
            tabArea = contentArea.removeFromBottom(tabDepth);
            contentArea = contentArea.withTrimmedBottom(gap_);
            break;
        }

        tabs->setBounds(tabArea);

        for (int i = 0; i < getNumTabs(); i++) {
            Component* c = getTabContentComponent(i);
            c->setBounds(contentArea);
        }
    }


} // namespace e3