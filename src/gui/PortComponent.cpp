
#include <algorithm>

#include <e3_Trace.h>
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
        }


        void PortComponent::paint(Graphics& g)
        {
            Colour col1 = findColour(Style::kModulePort1ColourId);
            Colour col2 = findColour(Style::kModulePort2ColourId);
            Colour col = (numConnections_ > 0) ? col2 : col1;

            g.setColour(col);

            if ((state_ & kHover) || (state_ & kDocking))	    // draw port square
            {
                g.fillRect(rcDock_.expanded(2, 2));
            }
            else {
                g.fillRect(rcDock_);
            }

            if (numConnections_ > 0) {
                g.drawRect(rcConnector_);
            }

            if ((state_ & kHover) || (state_ & kDocking))	 // draw label
            {
                //g.setColour(findColour(Style::kModuleColourId));
                g.setColour(col);
                g.fillRect(rcBkgnd_);

                int align = (getPortType() == kInport) ? Justification::centredLeft : Justification::centredRight;
                g.setFont(10);
                g.setColour(findColour(Style::kModuleText1ColourId).darker());
                g.drawText(port_->getLabel(), rcText_, align, true);
            }
        }


        void PortComponent::resized()
        {
            if (getPortType() == kInport)
            {
                rcDock_      = Rectangle<int>(4,  3, 6,  6);
                rcConnector_ = Rectangle<int>(-1,  5, 5,  2);
                rcHover_     = Rectangle<int>(0,  0, 12, getHeight());
                rcText_      = Rectangle<int>(14, 1, 38, 10);
                rcBkgnd_     = Rectangle<int>(10, 1, 42, 10);
            }
            else {
                rcDock_      = Rectangle<int>(getWidth() - 10, 3, 6,  6);
                rcConnector_ = Rectangle<int>(getWidth() - 5,  5, 5,  2);
                rcHover_     = Rectangle<int>(getWidth() - 12, 0, 12, getHeight());
                rcText_      = Rectangle<int>(getWidth() - 54, 1, 38, 10);
                rcBkgnd_     = Rectangle<int>(getWidth() - 54, 1, 42, 10);
            }
        }


        void PortComponent::mouseEnter(const MouseEvent& e)
        {
            mouseMove(e);
        }


        void PortComponent::mouseExit(const MouseEvent&)
        {
            if ((state_ & kDocking) == false) {
                state_ &= ~kHover;
            }
            repaint();
        }


        void PortComponent::mouseDown(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition())) 
            {
                switch (port_->getType()) {
                case kInport:  owner_->portAction(this, kPortActionUndock, getDockingPosition()); break;
                case kOutport: owner_->portAction(this, kPortActionDock, getDockingPosition()); break;
                }
            }
        }


        void PortComponent::mouseDrag(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition()))
            {
                Point<int> pos = translateToParent(e.getPosition())
                    .translated(getPortType() == kInport ? 0 : getWidth(), getY());

                owner_->portAction(this, kPortActionContinueDocking, pos);
            }
        }


        void PortComponent::mouseMove(const MouseEvent& e)
        {
            uint16_t oldState = state_;
            Point<int> pos = e.getPosition();
            hitTest(pos.x, pos.y) ? state_ |= kHover : state_ &= ~kHover; 
            if (state_ != oldState) {
                repaint();
            }
        }


        void PortComponent::mouseUp(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition()))
            {
                owner_->portAction(this, kPortActionEndDocking, translateToParent(e.getPosition()));
            }
        }


        bool PortComponent::startDocking()
        {
            state_ |= kDocking;
            repaint();
            
            return true;
        }


        void PortComponent::cancelDocking()
        {
            state_ &= ~kDocking;
            repaint();
        }


       void PortComponent::connect()
        {
            numConnections_++;
            state_ &= ~kDocking;
        }


       void PortComponent::disconnect()
       {
           ASSERT(numConnections_ >= 1);
           if (numConnections_ >= 1) {
               --numConnections_;
            }

            state_ &= ~kDocking;
        }


        Point<int> PortComponent::getDockingPosition()
        {
            Point<int> pos;
            pos.x = getPortType() == kInport ? 1 : getRight() - 1;
            pos.y = getY() + rcConnector_.getY() + 1;
            
            return translateToParent(pos);
        }


        bool PortComponent::isDockingPosition(const Point<int>& pos) const
        {
            Rectangle<int> dock = rcDock_.getUnion(rcConnector_);
            return dock.contains(pos);
        }


        bool PortComponent::hitTest(int x, int y)
        {
            return rcHover_.contains(x, y);
        }


        Point<int> PortComponent::translateFromParent(const Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getBounds();
            return pos.translated(-rcParent.getX(), -rcParent.getX());
        }


        Point<int> PortComponent::translateToParent(const Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getBounds();
            return pos.translated(rcParent.getX(), rcParent.getY());
        }


        int PortComponent::getModuleId() const         
        { 
            return owner_->getModuleId(); 
        }


} // namespace e3