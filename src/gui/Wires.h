
#pragma once

#include "JuceHeader.h"
#include "core/Port.h"
#include "gui/PortComponent.h"


namespace e3 {

    class Link;
    class ModulePanel;
    class ModuleComponent;


    //--------------------------------------------------------------
    // class Wire
    //--------------------------------------------------------------

    class Wire
    {
        friend class WireManager;

    public:
        Wire();
        Wire(const Point<int>& first, const Point<int>& last, Link* link, WireManager* manager);

        bool hitTest(const Rectangle<int>& area);
        Rectangle<int> getBoundingRect();

        void select(bool doSelect) { selected_ = doSelect; }
        bool isSelected()          { return selected_; }

        void paint(Graphics& g);

    protected:
        static bool segmentIntersectsRectangle(const Rectangle<int>& rc, const Point<int>& a1, const Point<int>& a2);

        Point<int> first_, last_;
        Link* link_           = nullptr;
        WireManager* manager_ = nullptr;
        bool selected_        = false;
    };


    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    class WireManager
    {
    public:
        WireManager(ModulePanel* panel);

        void addWire(Point<int> first, Point<int> last, Link* link);
        void removeWire(Wire* wire);
        Wire* getWire(int index) const;
        int getNumWires() const;

        void selectWiresInArea(const Rectangle<int>& area);
        void updateWiresForModule(ModuleComponent* module, bool selectWire);
        void paint(Graphics& g);

        void startDocking(PortComponent* port, const Point<int>& pos);
        void continueDocking(const Point<int>& pos);
        void endDocking(const Point<int>& pos);

        void startUndocking(PortComponent* port);

    protected:
        Wire* getWire(int moduleId, int portId, bool mustBeSelected) const;

        ModulePanel* panel_;
        OwnedArray<Wire> wires_;
        ScopedPointer<Wire> dockingWire_;
        Link dockingLink_;
        PortComponent* leftPort_  = nullptr;
        PortComponent* rightPort_ = nullptr;
        PortAction action_ = kPortActionIdle;
    };



} // namespace e3