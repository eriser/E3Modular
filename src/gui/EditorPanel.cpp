
#include "core/GlobalHeader.h"
#include "core/Module.h"
#include "core/Instrument.h"
//#include "gui/PortComponent.h"
//#include "gui/ModuleComponent.h"
#include "gui/ModulePanel.h"
#include "gui/ParameterPanel.h"
#include "gui/EditorPanel.h"


namespace e3
{
    EditorPanel::EditorPanel()
    {
        modulePanel_    = new ModulePanel(this);
        parameterPanel_ = new ParameterPanel();

        viewport_.setViewedComponent(modulePanel_, false);
        viewport_.setFocusContainer(true);

        addAndMakeVisible(viewport_);
        addAndMakeVisible(parameterPanel_);
    }


    void EditorPanel::showInstrument(Instrument* instrument, XmlElement* instrumentXml)
    {
        modulePanel_->createModules(instrument, instrumentXml);
    }


    void EditorPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        int separator = jmax(300, (int)(content.getWidth() * 0.75));

        viewport_.setBounds(content.withWidth(separator));      // this is the visible area
        modulePanel_->checkSize();
    }

} // namespace e3