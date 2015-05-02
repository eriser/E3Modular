
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
        state_(Idle)
        {
            ASSERT(port_ != nullptr);
        }


        void PortComponent::paint(Graphics& g)
        {
            Colour col1 = findColour(Style::kModulePort1ColourId);
            Colour col2 = findColour(Style::kModulePort2ColourId);
            Colour col  = (numConnections_ > 0) ? col2 : col1;

            if (state_ & Reject)
            {
                g.setColour( Colours::red );
                g.fillEllipse(rcDock_.expanded(3, 3).toFloat());
                g.setColour( Colours::white );
                g.fillRect( rcDock_.reduced( 0, 2 ) );
            }
            else if ((state_ & Hover) || (state_ & Docking))	    // draw port square
            {
                g.setColour( col );
                g.fillRect( rcDock_.expanded( 2, 2 ) );
            }
            else {
                g.setColour( col );
                g.fillRect( rcDock_ );
            }

            if (numConnections_ > 0 && !(state_ && Reject)) {
                g.setColour( col );
                g.drawRect( rcConnector_ );
            }

            if ((state_ & Hover) || (state_ & Docking))	 // draw label
            {
                //g.setColour(findColour(Style::kModuleColourId));
                g.setColour(col);
                g.fillRect(rcBkgnd_);

                int align = (getPortType() == PortTypeInport) ? Justification::centredLeft : Justification::centredRight;
                g.setFont(10);
                g.setColour(findColour(Style::kModuleText1ColourId).darker());
                g.drawText(port_->getLabel(), rcText_, align, true);
            }
        }


        void PortComponent::resized()
        {
            if (getPortType() == PortTypeInport)
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
            if ((state_ & Docking) == false) {
                state_ &= ~Hover;
            }
            repaint();
        }


        void PortComponent::mouseDown(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition())) 
            {
                PortAction action = (getPortType() & PortTypeInport) ? PortActionUndocking : PortActionDocking;
                owner_->portAction( this, action, getDockingPosition() );

                //switch (port_->getType()) {
                //case PortTypeInport:  owner_->portAction(this, PortActionUndocking, getDockingPosition()); break;
                //case PortTypeOutport: owner_->portAction(this, PortActionDocking, getDockingPosition()); break;
                //}
            }
        }


        void PortComponent::mouseDrag(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition()))
            {
                Point<int> pos = translateToParent(e.getPosition())
                    .translated(getPortType() == PortTypeInport ? 0 : getWidth(), getY());

                owner_->portAction(this, PortActionContinueDocking, pos);
            }
        }


        void PortComponent::mouseMove(const MouseEvent& e)
        {
            uint16_t oldState = state_;
            Point<int> pos = e.getPosition();
            hitTest(pos.x, pos.y) ? state_ |= Hover : state_ &= ~Hover; 
            if (state_ != oldState) {
                repaint();
            }
        }


        void PortComponent::mouseUp(const MouseEvent& e)
        {
            if (isDockingPosition(e.getMouseDownPosition()))
            {
                owner_->portAction(this, PortActionEndDocking, translateToParent(e.getPosition()));
            }
        }


        void PortComponent::setDockingState()
        {
            state_ |= Docking;
            repaint();
        }


        void PortComponent::setRejectState()
        {
            state_ |= Reject;
            repaint();
        }


        void PortComponent::unsetDockingState()
        {
            state_ &= ~Docking;
            state_ &= ~Reject;
            repaint();
        }


       void PortComponent::setConnectedState()
        {
            numConnections_++;
            state_ &= ~Docking;
        }


       void PortComponent::setDisconnectedState()
       {
           ASSERT(numConnections_ >= 1);
           if (numConnections_ >= 1) {
               --numConnections_;
            }

            state_ &= ~Docking;
        }


        Point<int> PortComponent::getDockingPosition()
        {
            Point<int> pos;
            pos.x = getPortType() == PortTypeInport ? 1 : getRight() - 1;
            pos.y = getY() + rcConnector_.getY() + 1;
            
            return translateToParent(pos);
        }


        bool PortComponent::isDockingPosition(const Point<int>& pos) const
        {
            return rcHover_.contains( pos );
        }


        bool PortComponent::hitTest(int x, int y)
        {
            return rcHover_.contains(x, y);
        }


        Point<int> PortComponent::translateFromParent(const Point<int>& pos) const
        {
            Rectangle<int> rcParent = owner_->getBounds();
            return pos.translated(-rcParent.getX(), -rcParent.getX());
        }


        Point<int> PortComponent::translateToParent(const Point<int>& pos) const
        {
            Rectangle<int> rcParent = owner_->getBounds();
            return pos.translated(rcParent.getX(), rcParent.getY());
        }


        int PortComponent::getModuleId() const         
        { 
            return owner_->getModuleId(); 
        }


} // namespace e3