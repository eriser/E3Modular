
#pragma once

#include "JuceHeader.h"

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
        Wire(const Point<int>& first, const Point<int>& last, Link* link, WireManager* manager);

        bool hitTest(const Rectangle<int>& area);
        void getBoundingRect(Rectangle<int>& r);

        void select(bool doSelect) { selected_ = doSelect; }
        bool isSelected()          { return selected_; }

        void paint(Graphics& g);

    protected:
        static bool segmentIntersectsRectangle(const Rectangle<int>& rc, const Point<int>& a1, const Point<int>& a2);

        Point<int> first_, last_;
        Link* link_;
        WireManager* manager_;
        bool selected_ = false;
    };


    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    class WireManager : public ChangeBroadcaster
    {
    public:
        void addWire(Point<int> first, Point<int> last, Link* link);
        int getNumWires() const;
        Wire* getWire(int index) const;

        void selectWiresInArea(const Rectangle<int>& area);
        void updateWiresForModule(ModuleComponent* module, bool selectWire);
        void paint(Graphics& g);

    protected:
        OwnedArray<Wire> wires_;
    };



} // namespace e3