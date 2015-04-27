
#pragma once

#include <string>
#include <map>

#include "JuceHeader.h"
#include "core/Module.h"
#include "core/Link.h"
#include "gui/Wire.h"
#include "gui/Selection.h"


namespace e3 {

    class Module;
    class ModulePanel;
    class PortComponent;
    class Wire;

    typedef std::map< const Link, Wire* > WireMap;

    class ModuleComponent : public Component, public SelectableItem
    {
    public:
        ModuleComponent(ModulePanel* owner, Module* module, bool collapsed);
        ~ModuleComponent();

        void paint(Graphics& g) override;
        void resized() override;

        void beginDrag(const MouseEvent& e) override;
        void continueDrag(const MouseEvent& e) override;
        void endDrag(const MouseEvent& e) override;

        void mouseDoubleClick(const MouseEvent& e) override;

        //void focusAndSelect(bool focus, bool select);
        //void moveTo(Rectangle<int>& r);
        PortComponent* getPort(int portId, PortType portType);
        void getPortPosition(int portId, PortType portType, Point<int>& pos);
        //void collapseOrExpand(bool collapse);
        //bool isCollapsed();

        void createPorts();
        void createWires();
        void addWire(const Link& link, Wire* wire);
        void paintWires(Graphics& g);
        Wire* getWire(const Link& link);
        void hitTest(Array<SelectableItem*>& results, const Rectangle<int>& area);
        void hitTestWires(Array<SelectableItem*>& results, const Rectangle<int>& area);


        //void removeWires(uint16_t targetId);
        //void removeWire(const Link& link, PortType portType);
        //void positionWires(ModuleComponent* movedComponent);
        //void drawWires(Graphics& g);
        //void hitTestWires(const Rectangle<int>& rc, WireMap& map);
        //Wire* getWire(const Link& link);

        //void setColors();
        //void drawBackgroundRect(Graphics& g, Rectangle<int>& updateRect);

        Module* getModule() const;
        uint16_t getModuleId() const;


    protected:
        void calculateSize();
        void updateWires();


        //void setPortVisibility();
        //void deleteWire(const Link& link);
        //int getTextHeight(const std::string& s, const Font& font);


        ModulePanel* owner_;
        Module* module_;

        const int portHeight_ = 12;
        bool selected_        = false;
        bool focused_         = false;
        bool collapsed_       = false;

        Colour colBkgnd_, colFrame_, colPort_, colText_;

        WireMap wires_;

        ComponentBoundsConstrainer dragConstrainer_;
        ComponentDragger dragger_;
        Point<int> positionBeforeDragging_;

        typedef OwnedArray<PortComponent> PortList;
        PortList inports_;
        PortList outports_;
    };

} // namespace e3