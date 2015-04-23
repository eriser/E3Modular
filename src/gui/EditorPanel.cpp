
#include "core/GlobalHeader.h"
#include "core/Module.h"
//#include "gui/PortComponent.h"
//#include "gui/ModuleComponent.h"
#include "gui/EditorPanel.h"


namespace e3
{
    EditorPanel::EditorPanel()
    {}

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