
#include "gui/ModulePanel.h"
#include "gui/ParameterPanel.h"
#include "gui/EditorPanel.h"


namespace e3
{
    void EditorPanel::setContent(ModulePanel* modulePanel, ParameterPanel* parameterPanel)
    {
        modulePanel_    = modulePanel;
        parameterPanel_ = parameterPanel;

        viewport_.setViewedComponent(modulePanel_, false);
        viewport_.setFocusContainer(true);

        addAndMakeVisible(viewport_);
        addAndMakeVisible(parameterPanel_);
    }


    void EditorPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        int separator = jmax(300, (int)(content.getWidth() * 0.75));

        viewport_.setBounds(content.withWidth(separator));      // this is the visible area
        modulePanel_->checkViewport();
    }

} // namespace e3