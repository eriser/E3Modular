
#include "core/GlobalHeader.h"
#include "core/Module.h"
#include "core/Instrument.h"
//#include "gui/PortComponent.h"
//#include "gui/ModuleComponent.h"
#include "gui/WirePanel.h"
#include "gui/ParameterPanel.h"
#include "gui/EditorPanel.h"


namespace e3
{
    EditorPanel::EditorPanel()
    {
        wirePanel_ = new WirePanel(this);
        parameterPanel_ = new ParameterPanel();

        viewport_.setViewedComponent(wirePanel_, false);
        viewport_.setFocusContainer(true);

        addAndMakeVisible(viewport_);
        addAndMakeVisible(parameterPanel_);
    }


    void EditorPanel::showInstrument(Instrument* instrument, XmlElement* instrumentXml)
    {
        wirePanel_->showInstrument(instrument, instrumentXml);
    }


    void EditorPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        int separator = jmax(300, (int)(content.getWidth() * 0.75));

        viewport_.setBounds(content.withWidth(separator));      // this is the visible area
        wirePanel_->checkSize();
    }


    //ModuleComponent* EditorPanel::getModule(uint16_t id)
    //{
    //    for (int i = 0; i < modules_.size(); ++i)
    //    {
    //        ModuleComponent* module = modules_[i];
    //        if (module->getModuleId() == id) {
    //            return module;
    //        }
    //    }
    //    return nullptr;
    //}


    //PortComponent* EditorPanel::getPort(const Link& link, PortType portType)
    //{
    //    PortComponent* port = nullptr;
    //    ModuleComponent* module = getModule(link.leftModule_);
    //    if (module) {
    //        port = (portType == kInport) ?
    //            module->getPort(link.rightPort_, kInport) :
    //            module->getPort(link.leftPort_, kOutport);
    //    }
    //    return port;
    //}

} // namespace e3