
#include <e3_Trace.h>
#include <e3_Exception.h>
#include "core/Settings.h"
#include "gui/Style.h"
#include "gui/PortComponent.h"
#include "gui/ModuleComponent.h"
#include "gui/ModulePanel.h"
#include "gui/Wires.h"


namespace e3 {

    //--------------------------------------------------------------
    // class Wire
    //--------------------------------------------------------------

    Wire::Wire(const Point<int>& first, const Point<int>& last, Link* link, WireManager* manager) :
        first_(first),
        last_(last),
        link_(link),
        manager_(manager)
    {
        ASSERT(manager_);
        ASSERT(link_);
    }


    void Wire::paint(Graphics& g)
    {
        Style* style = Settings::getInstance().getStyle();
        Colour c = selected_ ? style->findColour(Style::kWire2ColourId) : style->findColour(Style::kWire1ColourId);
        g.setColour(c);
        g.drawLine(Line<float>(first_.toFloat(), last_.toFloat()), 1);
    }


    void Wire::getBoundingRect(Rectangle<int>& r)
    {
        r = Rectangle<int>(first_, last_);
    }


    bool Wire::segmentIntersectsRectangle(const Rectangle<int>& r, const Point<int>& a1, const Point<int>& a2)
    {
        // Find min and max X for the segment
        double minX = std::min<double>(a1.x, a2.x);
        double maxX = std::max<double>(a1.x, a2.x);

        // Find the intersection of the segment's and rectangle's x-projections
        maxX = std::min<double>(maxX, r.getRight());
        minX = std::max<double>(minX, r.getX());

        // If their projections do not intersect return false
        if (minX > maxX)  {
            return false;
        }

        // Find corresponding min and max Y for min and max X we found before
        double minY = a1.y;
        double maxY = a2.y;

        // TODO: try juce::Point::DotProduct
        double dx = a2.x - a1.x;
        if (abs(dx) > 0.0000001)
        {
            double a = (a2.y - a1.y) / dx;
            double b = a1.y - a * a1.x;
            minY = a * minX + b;
            maxY = a * maxX + b;
        }
        if (minY > maxY) {
            std::swap(minY, maxY);
        }
        // Find the intersection of the segment's and rectangle's y-projections
        maxY = std::min<double>(maxY, r.getBottom());
        minY = std::max<double>(minY, r.getY());

        return minY <= maxY;        // If Y-projections do not intersect return false
    }


    bool Wire::hitTest(const Rectangle<int>& area)
    {
        //selected_ = area.intersects(Line<int>(front(), back()));
        return segmentIntersectsRectangle(area, first_, last_);
    }



    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    WireManager::WireManager(ModulePanel* owner) : panel_(owner)
    {}


    void WireManager::addWire(Point<int> first, Point<int> last, Link* link)
    {
        Wire* wire = new Wire(first, last, link, this);
        wires_.add(wire);
    }


    int WireManager::getNumWires() const
    {
        return wires_.size();
    }


    Wire* WireManager::getWire(int index) const
    {
        return wires_.getUnchecked(index);
    }


    //void WireManager::hitTest(Array<SelectableItem*>& results, const Rectangle<int>& area)
    //{
    //    for (int i = 0; i < wires_.size(); ++i)
    //    {
    //        Wire* wire = wires_.getUnchecked(i);
    //        if (wire->hitTest(area))
    //        {
    //            results.addIfNotAlreadyThere(wire);
    //        }
    //        else {
    //            results.removeAllInstancesOf(wire);
    //        }
    //    }
    //}


    void WireManager::updateWiresForModule(ModuleComponent* module, bool selectWire)
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire  = wires_.getUnchecked(i);
            Link* link  = wire->link_;
            uint16_t id = module->getModuleId();

            if (link->leftModule_ == id || link->rightModule_ == id) 
            {
                PortComponent* leftPort  = panel_->getPort(link, kOutport);
                PortComponent* rightPort = panel_->getPort(link, kInport);

                if (leftPort && rightPort)
                {
                    Point<int> first, last;
                    wire->first_ = leftPort->getPortPosition();
                    wire->last_ = rightPort->getPortPosition();

                    wire->select(selectWire);
                }
            }
        }
    }


    //void WireManager::drag(Wire* wire)
    //{
    //    PortComponent* leftPort  = panel_->getPort(wire->link_, kOutport);
    //    PortComponent* rightPort = panel_->getPort(wire->link_, kInport);

    //    if (leftPort && rightPort)
    //    { 
    //        Point<int> first, last;
    //        wire->first_ = leftPort->getPortPosition();
    //        wire->last_  = rightPort->getPortPosition();
    //    }
    //}


    void WireManager::paint(Graphics& g)
    {
        for (int i = 0; i < wires_.size(); ++i) {
            wires_.getUnchecked(i)->paint(g);
        }
    }


} // namespace e3