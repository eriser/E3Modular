
#include "gui/ColourIds.h"
#include "gui/EditorPanel.h"


namespace e3
{
    EditorPanel::EditorPanel() : Component()
    {}

    void EditorPanel::paint(Graphics& g)
    {
        g.fillAll(findColour(ColourIds::contentBackgroundColourId));
    }
} // namespace e3