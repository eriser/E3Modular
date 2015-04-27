
#include <algorithm>

#include <e3_Exception.h>
#include "core/GlobalHeader.h"
#include "core/Module.h"
#include "gui/Style.h"
#include "gui/ModuleComponent.h"
#include "gui/PortComponent.h"

namespace e3 {

    PortComponent::PortComponent(Port* port, ModuleComponent* owner) :
        port_(port),
        owner_(owner),
        state_(kIdle)
        {
            ASSERT(port_ != nullptr);
            //repaint();
        }


        void PortComponent::mouseEnter(const MouseEvent& e)
        {
            return mouseMove(e);
        }


        void PortComponent::resized()
        {
            if (getPortType() == kInport)
            {
                rcSquare_    = Rectangle<int>(4,  3, 6,  6);
                rcConnector_ = Rectangle<int>(0,  5, 4,  2);
                rcHover_     = Rectangle<int>(0,  0, 12, getHeight());
                rcText_      = Rectangle<int>(14, 0, 38, getHeight());
                rcBkgnd_     = Rectangle<int>(10, 0, 42, getHeight());
            }
            else {
                rcSquare_    = Rectangle<int>(getWidth() - 10, 3, 6,  6);
                rcConnector_ = Rectangle<int>(getWidth() - 4,  5, 4,  2);
                rcHover_     = Rectangle<int>(getWidth() - 12, 0, 12, getHeight());
                rcText_      = Rectangle<int>(getWidth() - 52, 0, 38, getHeight());
                rcBkgnd_     = Rectangle<int>(getWidth() - 54, 0, 42, getHeight());
            }
        }


        void PortComponent::mouseExit(const MouseEvent&)
        {
            state_ &= ~kHover;
            repaint(rcBkgnd_);
        }


        void PortComponent::mouseDown(const MouseEvent& e)
        {
            return mouseMove(e);
        }


        void PortComponent::mouseMove(const MouseEvent &e)
        {
            uint16_t oldState = state_;
            setHoverState(e.getMouseDownPosition());
            if (state_ != oldState) {
                repaint(rcBkgnd_);
            }
        }


        bool PortComponent::hitTest(int x, int y)
        {
            return rcHover_.contains(x, y);
        }


        //bool PortComponent::testDocking(const Point<int>& pos)
        //{
        //    Point<int> p(pos);
        //    translateFromParent(p);
        //    return hitTest(p.x, p.y);
        //}


        bool PortComponent::startDocking()
        {
            //translateFromParent(pos);
            //bool result = setHoverState(pos);
            bool result = false;

            if (result)
            {
                //getPortPosition(pos);
                state_ |= kDocking;
                repaint(rcBkgnd_);
            }
            return result;
        }


        void PortComponent::endDocking()
        {
            if (numConnections_ == 0) {
                state_ &= ~kConnected;
            }
            //translateFromParent(pos);
            state_ &= ~kDocking;
            //setHoverState(pos);
            repaint(rcBkgnd_);
        }


       void PortComponent::connect()
        {
            //translateFromParent(pos);
            //setHoverState(pos);
            numConnections_++;
            //getPortPosition(pos);
            state_ |= kConnected;
        }


        void PortComponent::disconnect()
        {
            ASSERT(numConnections_ >= 1);
            numConnections_--;

            state_ &= ~kDocking;
            if (numConnections_ == 0) {
                state_ &= ~kConnected;
            }
        }


        bool PortComponent::setHoverState(const Point<int>& pos)
        {
            hitTest(pos.x, pos.y) ? state_ |= kHover : state_ &= ~kHover;
            return state_ & kHover;
        }


        Point<int> PortComponent::getPosition()
        {
            Point<int> pos;
            pos.x = getPortType() == kInport ? 1 : getRight() - 1;
            pos.y = getY() + getHeight() / 2;
            
            translateToParent(pos);
            return pos;
        }


        void PortComponent::translateFromParent(Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getBounds();
            pos.addXY(-rcParent.getX(), -rcParent.getX());
        }


        void PortComponent::translateToParent(Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getBounds();
            pos.addXY(rcParent.getX(), rcParent.getY());
        }


        void PortComponent::paint(Graphics& g)
        {
            Colour colPort1 = findColour(Style::kModulePort1ColourId);
            Colour colPort2 = findColour(Style::kModulePort2ColourId);
            colPort1        = (state_ & kConnected) ? colPort2 : colPort1;

            g.setColour( colPort1 );

            if( state_ & (kHover || kDocking) )	    // draw port square
            {
                Rectangle<int> rcFrame( rcSquare_ );
                rcFrame.expand( 1, 1 );
                g.fillRect( rcFrame );
            }
            else {
                g.fillRect(rcSquare_);
            }

            if (numConnections_ > 0) {
                g.drawRect( rcConnector_);
            }

            if( state_ & (kHover || kDocking) )	    // draw label
            {
                //Rectangle<int> rcClip = g.getClipBounds();
                //g.reduceClipRegion(rcBkgnd_);
                
                g.setColour(findColour(Style::kModuleColourId));
                g.fillRect( rcBkgnd_);
                
                int align = (getPortType() == kInport) ? Justification::centredLeft : Justification::centredRight;
                g.setFont(9);
                g.setColour(findColour(Style::kModuleText1ColourId));
                g.drawText(port_->getLabel(), rcText_, align, true);
                
                //g.reduceClipRegion(rcClip);
            }
        }

} // namespace e3