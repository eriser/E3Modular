
#pragma once

#include <cstdint>
#include <map>
#include "JuceHeader.h"

namespace e3
{
    //class PortComponent;
    //class ModuleComponent;


    class EditorPanel : public Component
    {
    public:
        EditorPanel();

        //PortComponent* getPort(const Link& link, PortType portType);
        //ModuleComponent* getModule(uint16_t id);


    protected:
        //typedef std::map< uint16_t, ModuleComponent* > ModuleComponentMap;
        //
        //OwnedArray<ModuleComponent> modules_;
    };


} // namespace e3