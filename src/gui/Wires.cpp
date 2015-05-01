
#include <string>
#include <sstream>
#include <e3_Trace.h>
#include <e3_Exception.h>
#include "core/Settings.h"
#include "core/Processor.h"
#include "gui/Style.h"
#include "gui/PortComponent.h"
#include "gui/ModuleComponent.h"
#include "gui/ModulePanel.h"
#include "gui/Wires.h"


namespace e3 {

    //--------------------------------------------------------------
    // class Wire
    //--------------------------------------------------------------

    Wire::Wire() {}

    
    Wire::Wire( const Point<int>& first, const Point<int>& last, Link* link ) :
        first_( first ),
        last_( last ),
        link_( link == nullptr ? Link() : *link )
    {}


    void Wire::paint( Graphics& g )
    {
        Style* style = Settings::getInstance().getStyle();
        Colour c = selected_ ? style->findColour( Style::kWire2ColourId ) : style->findColour( Style::kWire1ColourId );
        g.setColour( c );
        g.drawLine( Line<float>( first_.toFloat(), last_.toFloat() ), 1 );
    }


    Rectangle<int> Wire::getBoundingRect()
    {
        return Rectangle<int>( first_, last_ );
    }


    bool Wire::segmentIntersectsRectangle( const Rectangle<int>& r, const Point<int>& a1, const Point<int>& a2 )
    {
        // Find min and max X for the segment
        double minX = std::min<double>( a1.x, a2.x );
        double maxX = std::max<double>( a1.x, a2.x );

        // Find the intersection of the segment's and rectangle's x-projections
        maxX = std::min<double>( maxX, r.getRight() );
        minX = std::max<double>( minX, r.getX() );

        // If their projections do not intersect return false
        if (minX > maxX)  {
            return false;
        }

        // Find corresponding min and max Y for min and max X we found before
        double minY = a1.y;
        double maxY = a2.y;

        // TODO: try juce::Point::DotProduct
        double dx = a2.x - a1.x;
        if (abs( dx ) > 0.0000001)
        {
            double a = (a2.y - a1.y) / dx;
            double b = a1.y - a * a1.x;
            minY = a * minX + b;
            maxY = a * maxX + b;
        }
        if (minY > maxY) {
            std::swap( minY, maxY );
        }
        // Find the intersection of the segment's and rectangle's y-projections
        maxY = std::min<double>( maxY, r.getBottom() );
        minY = std::max<double>( minY, r.getY() );

        return minY <= maxY;        // If Y-projections do not intersect return false
    }


    bool Wire::hitTest( const Rectangle<int>& area )
    {
        //selected_ = area.intersects(Line<int>(front(), back()));
        return segmentIntersectsRectangle( area, first_, last_ );
    }



    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    WireManager::WireManager( ModulePanel* panel ) :
        panel_( panel )
    {}


    void WireManager::addWire( Point<int> first, Point<int> last, Link* link )
    {
        Wire* wire = new Wire( first, last, link );
        
        bool contains = containsWire( wire );
        ASSERT( contains == false );
        
        if (contains == false) {
            wires_.add( wire );
        }
    }


    void WireManager::deleteWire( Wire* wire )
    {
        ASSERT( wire );

        for (int i = 0; i < wires_.size(); ++i) 
        {
            Wire* next = wires_.getUnchecked( i );
            if (next->link_ == wire->link_) {
                wires_.remove( i, true );
                return;
            }
        }
        ASSERT( false );
        //wires_.removeObject( wire, true );
    }


    int WireManager::getNumWires() const
    {
        return wires_.size();
    }


    Wire* WireManager::getWire( int index ) const
    {
        return wires_.getUnchecked( index );
    }


    bool WireManager::containsWire( Wire* wire ) const
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* next = wires_.getUnchecked( i );
            if (next->link_ == wire->link_) {
                return true;
            }
        }
        return false;
    }


    void WireManager::paint( Graphics& g )
    {
        for (int i = 0; i < wires_.size(); ++i) {
            wires_.getUnchecked( i )->paint( g );
        }
        if (state_ == PortActionDocking) {
            dockingWire_.paint( g );
        }
    }


    void WireManager::selectWiresInArea( const Rectangle<int>& area )
    {
        ModifierKeys modifiers = ModifierKeys::getCurrentModifiers();
        bool needsRepaint = false;

        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire       = wires_.getUnchecked( i );
            bool selectWire  = wire->hitTest( area );
            needsRepaint    |= selectWire != wire->isSelected();

            if (modifiers.isCtrlDown() && selectWire == true || !modifiers.isCtrlDown()) {
                wire->select( selectWire );
                if (needsRepaint) {
                    panel_->repaint( wire->getBoundingRect() );
                }
            }
        }
    }


    void WireManager::updateWiresForModule( ModuleComponent* module, bool selectWire )
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire  = wires_.getUnchecked( i );
            Link* link  = &wire->link_;
            int id      = module->getModuleId();

            if (link && link->leftModule_ == id || link->rightModule_ == id)
            {
                PortComponent* leftPort  = module->getPort( link, PortTypeOutport );
                PortComponent* rightPort = module->getPort( link, PortTypeInport );

                if (leftPort && rightPort)
                {
                    Point<int> first, last;
                    wire->first_ = leftPort->getDockingPosition();
                    wire->last_  = rightPort->getDockingPosition();

                    wire->select( selectWire );
                }
            }
        }
    }


    void WireManager::startDocking( PortComponent* port, const Point<int>& pos )
    {
        ASSERT( port );
        if (state_ == PortActionIdle && port != nullptr)
        {
            dockingWire_.first_ = pos;
            dockingWire_.last_  = pos;

            if (checkPort( dockingLink_, port, PortTypeOutport ) == WireErrorNone)  {
                state_ = PortActionDocking;
            }
        }
    }


    void WireManager::startUndocking( PortComponent* rightPort )
    {
        WireError error = checkPort( dockingLink_, rightPort, PortTypeInport );

        int numConnections = rightPort->getNumConnections();
        if (state_ == PortActionIdle && error == WireErrorNone && numConnections >= 1)
        {
            Wire* wire = getWire( &dockingLink_, PortTypeInport, numConnections > 1 );
            if (wire)
            {
                dockingLink_ = wire->link_;
                deleteWire( wire );

                panel_->getProcessor()->removeLink( &dockingLink_ );
                rightPort->setDisconnectedState();
                dockingLink_.resetRight();
                panel_->repaint();

                PortComponent* leftPort  = panel_->getPort( &dockingLink_, PortTypeOutport );
                ASSERT( leftPort );
                if (leftPort != nullptr) 
                {
                    leftPort->setDisconnectedState();
                    dockingWire_.first_ = leftPort->getDockingPosition();
                    dockingWire_.last_  = rightPort->getDockingPosition();
                    state_              = PortActionDocking;
                }
            }
        }
    }


    void WireManager::continueDocking( const Point<int>& pos )
    {
        if (state_ == PortActionDocking)
        {
            PortComponent* port = panel_->getPortAtPosition( pos );
            WireError error     = checkPort( dockingLink_, port, PortTypeInport );

            if (lastVisitedPort_) {
                lastVisitedPort_->unsetDockingState();
            }

            switch (error) {
            case WireErrorNone:  
            case WireErrorSameModule:
                lastVisitedPort_ = port;
                if (getWire( &dockingLink_ )) {         // check for link duplicates
                    port->setRejectState();
                    dockingLink_.resetRight();
                }
                else {
                    port->setDockingState();            // valid, non duplicate link
                }
                break; 

            case WireErrorSameType:
                lastVisitedPort_ = port;
                port->setRejectState(); 
                dockingLink_.resetRight();
                break;

            case WireErrorPortNotFound:                 // mouse not over port
            case WireErrorSamePort:                     // sourcePort == targetPort
                lastVisitedPort_ = nullptr;
                dockingLink_.resetRight();
                break;
            }

            dockingWire_.last_ = pos;
            panel_->repaint();
        }
    }


    void WireManager::endDocking( const Point<int>& )
    {
        if (state_ == PortActionDocking)
        {
            Link* link = nullptr;
            if (dockingLink_.isValid())
            {
                link = panel_->getProcessor()->addLink( &dockingLink_ );
                ASSERT( link && *link == dockingLink_ );
            }

            PortComponent* leftPort  = panel_->getPort( &dockingLink_, PortTypeOutport );
            PortComponent* rightPort = panel_->getPort( &dockingLink_, PortTypeInport );

            if (link && leftPort && rightPort)
            {
                addWire( dockingWire_.first_, rightPort->getDockingPosition(), link );

                leftPort->setConnectedState(); 
                rightPort->setConnectedState();
            }
            else if (lastVisitedPort_ != nullptr) {
                lastVisitedPort_->unsetDockingState();
            }

            resetDocking();
            panel_->repaint();
        }
    }


    WireManager::WireError WireManager::checkPort( Link& link, PortComponent* port, PortType expectedType )
    {
        if (port != nullptr)
        {
            if (expectedType == PortTypeInport) {
                link.rightModule_ = port->getModuleId();
                link.rightPort_   = port->getPortId();
            }
            else if (expectedType == PortTypeOutport) {
                link.leftModule_ = port->getModuleId();
                link.leftPort_   = port->getPortId();
            }
            
            PortType portType = port->getPortType();
            if (portType != expectedType) {
                return link.isSamePort() ? WireErrorSamePort : WireErrorSameType;
            }          
            else {     
                return link.isSameModule() ? WireErrorSameModule : WireErrorNone;
            }
        }
        return WireErrorPortNotFound;
    }


    Wire* WireManager::getWire( Link* link, PortType portType, bool onlySelected ) const
    {
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire = wires_.getUnchecked( i );

            bool expression = false;
            switch (portType) {
            case PortTypeUndefined: expression = (*link == wire->link_); break;
            case PortTypeInport: expression = 
                link->rightModule_ == wire->link_.rightModule_ && 
                link->rightPort_   == wire->link_.rightPort_; 
                break;
            case PortTypeOutport: expression =
                link->leftModule_ == wire->link_.leftModule_ &&
                link->leftPort_   == wire->link_.leftPort_;
                break;
            }
            expression &= (onlySelected == false || wire->isSelected());

            if (expression == true)
            {
                return wire;
            }
        }
        return nullptr;
    }


    void WireManager::resetDocking()
    {
        state_               = PortActionIdle;
        dockingWire_.first_  = Point<int>();
        dockingWire_.last_   = Point<int>();
        dockingLink_         = Link();
        lastVisitedPort_     = nullptr;
    }


    std::string WireManager::printLinks() const
    {
        std::ostringstream os;
        for (int i = 0; i < wires_.size(); ++i)
        {
            Wire* wire = wires_.getUnchecked( i );
            os << wire->link_.toString() << std::endl;
        }
        return os.str();
    }


} // namespace e3