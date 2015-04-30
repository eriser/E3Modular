
#pragma once

#include "JuceHeader.h"
#include "core/Module.h"
#include "core/Link.h"


namespace e3 {

    class Module;
    class ModulePanel;
    class PortComponent;


    class ModuleComponent : public Component
    {
    public:
        ModuleComponent(ModulePanel* owner, Module* module, bool collapsed);

        void paint(Graphics& g) override;
        void resized() override;
        void select(bool doSelect);
        bool isSelected() const     { return selected_; }

        void beginDrag(const MouseEvent& e);
        void continueDrag(const MouseEvent& e);
        void endDrag(const MouseEvent& e);

        void mouseDoubleClick(const MouseEvent& e) override;

        //void focusAndSelect(bool focus, bool select);
        //void moveTo(Rectangle<int>& r);
        void createPorts();
        PortComponent* getPort(int portId, PortType portType);
        PortComponent* getPort(Link* link, PortType portType);
        void getPortPosition(int portId, PortType portType, Point<int>& pos);
        PortComponent* getPortAtPosition(const Point<int>& pos) const;

        void portAction(PortComponent* port, PortAction action, const Point<int>& pos);

        //void collapseOrExpand(bool collapse);
        //bool isCollapsed();

        //void setColors();
        //void drawBackgroundRect(Graphics& g, Rectangle<int>& updateRect);

        Module* getModule() const;
        uint16_t getModuleId() const;


    protected:
        void calculateSize();
        PortComponent* getPortAtPosition(const Point<int>& pos, const OwnedArray<PortComponent>& list) const;

        //void setPortVisibility();
        //void deleteWire(const Link& link);
        //int getTextHeight(const std::string& s, const Font& font);

        ModulePanel* panel_;
        Module* module_;

        const int portHeight_ = 12;
        bool selected_        = false;
        bool focused_         = false;
        bool collapsed_       = false;

        Colour colBkgnd_, colFrame_, colPort_, colText_;

        ComponentBoundsConstrainer dragConstrainer_;
        ComponentDragger dragger_;
        Point<int> positionBeforeDragging_;

        typedef OwnedArray<PortComponent> PortList;
        PortList inports_;
        PortList outports_;
    };

} // namespace e3