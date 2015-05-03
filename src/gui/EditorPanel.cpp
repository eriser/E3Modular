
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
        int width              = content.getWidth();
        int separator          = jmax(300, (int)(width * 0.75));

        viewport_.setBounds(content.withWidth(separator));      // this is the visible area
        modulePanel_->checkViewport();

        //parameterPanel_->setBounds( content.withX( separator ).withX( 15 ).withWidth( width * 0.25 ) );
        parameterPanel_->setBounds( separator + 15, 0, width - 15, 25 );
    }

} // namespace e3