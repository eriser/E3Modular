
#pragma once

#include "JuceHeader.h"
#include "gui/Selection.h"

namespace e3 {

    class Link;
    class ModulePanel;
    class ModuleComponent;


    //--------------------------------------------------------------
    // class Wire
    //--------------------------------------------------------------

    class Wire : public SelectableItem
    {
        friend class WireManager;

    public:
        Wire(const Point<int>& first, const Point<int>& last, Link* link, WireManager* manager);

        bool hitTest(const Rectangle<int>& area);
        void getBoundingRect(Rectangle<int>& r);

        void paint(Graphics& g);

    protected:
        static bool segmentIntersectsRectangle(const Rectangle<int>& rc, const Point<int>& a1, const Point<int>& a2);

        Point<int> first_, last_;
        Link* link_;
        WireManager* manager_;
    };


    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    class WireManager
    {
    public:
        WireManager(ModulePanel* owner);

        void addWire(Point<int> first, Point<int> last, Link* link);
        int getNumWires() const;
        Wire* getWire(int index) const;

        //void hitTest(Array<SelectableItem*>& results, const Rectangle<int>& area);
        void updateWiresForModule(ModuleComponent* module, bool selectWire);
        void paint(Graphics& g);

    protected:
        OwnedArray<Wire> wires_;
        ModulePanel* panel_;
    };



} // namespace e3