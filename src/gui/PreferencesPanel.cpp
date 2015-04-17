
#include "gui/PreferencesPanel.h"


namespace e3
{
    void PreferencesPanel::paint(Graphics& g)
    {
        g.setColour(Colours::red);
        g.fillRect(getLocalBounds());
    }


} // namespace e3