
#include <e3_Trace.h>
#include <e3_Exception.h>

#include "gui/Style.h"
#include "gui/StackedPanel.h"


namespace e3 {

    void StackedPanel::insertPanel(Component* panel, int id)
    {
        panels_.insert(std::make_pair(id, panel));
    }


    void StackedPanel::showPanel(int id)
    {
        Component* nextPanel = panels_.at(id);

        if (nextPanel != currentPanel_)
        {
            if (currentPanel_ != nullptr)
            {
                currentPanel_->setVisible(false);
                removeChildComponent(currentPanel_);
            }

            currentPanel_ = nextPanel;
            addChildComponent(currentPanel_);
            currentPanel_->setVisible(true);
            currentPanel_->toFront(true);

            repaint();
        }
        resized();
    }


    void StackedPanel::paint(Graphics &g)
    {
        Colour c = findColour(Style::kBackgroundColourId);
        g.fillAll(Colour(0xffbebdbb));
    }


    void StackedPanel::resized()
    {
        Rectangle<int> content(getLocalBounds());

        if (currentPanel_ != nullptr) {
            currentPanel_->setBounds(content);
        }
    }


} // namespace e3