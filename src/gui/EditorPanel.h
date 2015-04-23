
#pragma once

#include <cstdint>
#include <map>
#include "JuceHeader.h"

namespace e3
{
    class Instrument;
    //class PortComponent;
    //class ModuleComponent;
    class WirePanel;
    class ParameterPanel;


    class EditorPanel : public Component
    {
    public:
        EditorPanel();

        void resized() override;
        Rectangle<int> getViewportBounds() const { return viewport_.getBounds(); }

        void showInstrument(Instrument* instrument, XmlElement* instrumentXml);


        //PortComponent* getPort(const Link& link, PortType portType);
        //ModuleComponent* getModule(uint16_t id);


    protected:
        Viewport viewport_;
        ScopedPointer<WirePanel> wirePanel_;
        ScopedPointer<ParameterPanel> parameterPanel_;
        //typedef std::map< uint16_t, ModuleComponent* > ModuleComponentMap;
        //
        //OwnedArray<ModuleComponent> modules_;
    };


} // namespace e3