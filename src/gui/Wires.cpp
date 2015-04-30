
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
    }


    void Wire::paint(Graphics& g)
    {
        Style* style = Settings::getInstance().getStyle();
        Colour c = selected_ ? style->findColour(Style::kWire2ColourId) : style->findColour(Style::kWire1ColourId);
        g.setColour(c);
        g.drawLine(Line<float>(first_.toFloat(), last_.toFloat()), 1);
    }


    Rectangle<int> Wire::getBoundingRect()
    {
       return Rectangle<int>(first_, last_);
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

    WireManager::WireManager(ModulePanel* panel) :
        panel_(panel),
        dockingWire_(new Wire(Point<int>(), Point<int>(), nullptr, this))
    {}


    void WireManager::addWire(Point<int> first, Point<int> last, Link* link)
    {
        Wire* wire = new Wire(first, last, link, this);
        wires_.add(wire);
    }


    void WireManager::removeWire(Wire* wire)
    {
        wires_.removeObject(wire, true);
    }


    int WireManager::getNumWires() const
    {
        return wires_.size();
    }


    Wire* WireManager::getWire(int index) const
    {
        return wires_.getUnchecked(index);
    }


    Wire* WireManager::getWire(int moduleId, int portId, bool mustBeSelected) const
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire = getWire(i);
            Link* link = wire->link_;
            if (link->rightModule_ == moduleId && link->rightPort_ == portId &&
                (mustBeSelected == false || wire->isSelected()))
            {
                return wire;
            }
        }
        return nullptr;
    }


    void WireManager::selectWiresInArea(const Rectangle<int>& area)
    {
        ModifierKeys modifiers = ModifierKeys::getCurrentModifiers();
        bool needsRepaint = false;

        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire       = getWire(i);
            bool selectWire  = wire->hitTest(area);
            needsRepaint    |= selectWire != wire->isSelected();

            if (modifiers.isCtrlDown() && selectWire == true || !modifiers.isCtrlDown()) {
                wire->select(selectWire);
                if (needsRepaint) {
                    panel_->repaint(wire->getBoundingRect());
                }
            }
        }
    }


    void WireManager::updateWiresForModule(ModuleComponent* module, bool selectWire)
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire  = wires_.getUnchecked(i);
            Link* link  = wire->link_;
            uint16_t id = module->getModuleId();

            if (link && link->leftModule_ == id || link->rightModule_ == id) 
            {
                PortComponent* leftPort  = module->getPort(link, kOutport);
                PortComponent* rightPort = module->getPort(link, kInport);

                if (leftPort && rightPort)
                {
                    Point<int> first, last;
                    wire->first_ = leftPort->getDockingPosition();
                    wire->last_  = rightPort->getDockingPosition();

                    wire->select(selectWire);
                }
            }
        }
    }


    void WireManager::paint(Graphics& g)
    {
        for (int i = 0; i < wires_.size(); ++i) {
            wires_.getUnchecked(i)->paint(g);
        }
        if (action_ == kPortActionDock) {
            dockingWire_->paint(g);
        }
    }


    void WireManager::startDocking(PortComponent* port, const Point<int>& pos)
    {
        leftPort_            = port;
        dockingWire_->first_ = pos;
        dockingWire_->last_  = pos;
        action_              = kPortActionDock;
    }


    void WireManager::startUndocking(PortComponent* port)
    {
        dockingWire_->link_ = nullptr;

        if (port->getNumConnections() >= 1)
        {
            int moduleId        = port->getModuleId();
            int portId          = port->getPortId();
            bool mustBeSelected = (port->getNumConnections() > 1);
            Wire* wire          = getWire(moduleId, portId, mustBeSelected);

            if (wire != nullptr) 
            {
                dockingWire_->link_ = wire->link_;
                removeWire(wire);
            }
        }

        if (dockingWire_->link_)
        {
            port->disconnect();

            rightPort_           = port;
            leftPort_            = panel_->getPort(dockingWire_->link_, kOutport);
            dockingWire_->first_ = leftPort_->getDockingPosition();
            dockingWire_->last_  = rightPort_->getDockingPosition();
            action_              = kPortActionDock;
        }
    }


    void WireManager::continueDocking(const Point<int>& pos)
    {
        if (action_ == kPortActionDock)
        {
            PortComponent* port = panel_->getPortAtPosition(pos);   // check if the mouse hits a port
            if (port != nullptr)                                    // mouse hits a port and we inform the port
            {
                rightPort_ = port;
                rightPort_->startDocking();
            }
            else if (rightPort_ != nullptr)                         // mouse has left the port
            {
                rightPort_->cancelDocking();
                rightPort_ = nullptr;
            }

            dockingWire_->last_ = pos;
            panel_->repaint();
        }
    }


    void WireManager::endDocking(const Point<int>&)
    {
        if (action_ == kPortActionDock) 
        {
            if (leftPort_ != nullptr && rightPort_ != nullptr) 
            {
                Link* link = new Link();
                link->leftModule_ = leftPort_->getModuleId();
                link->rightModule_ = rightPort_->getModuleId();
                link->leftPort_ = leftPort_->getPortId();
                link->rightPort_ = rightPort_->getPortId();

                Point<int> pos = rightPort_->getDockingPosition();
                addWire(dockingWire_->first_, pos, link);
                leftPort_->connect();
                rightPort_->connect();
            }
            else if (leftPort_ != nullptr) {
                leftPort_->disconnect();
            }

            action_              = kPortActionIdle;
            dockingWire_->first_ = Point<int>();
            dockingWire_->last_  = Point<int>();
            dockingWire_->link_  = nullptr;
            leftPort_            = nullptr;
            rightPort_           = nullptr;

            panel_->repaint();
        }
    }

} // namespace e3