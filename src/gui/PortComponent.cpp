
#include <algorithm>

#include <e3_Exception.h>
#include "core/ModuleBase.h"
//#include "gui/ModuleComponent.h"
#include "gui/PortComponent.h"

namespace e3 {

    class ModuleComponent : public Component {};

    //----------------------------------------------------------------
    // class PortComponent
    //----------------------------------------------------------------

    PortComponent::PortComponent(const Rectangle<int>& bounds, PortModel* model, PortType portType, ModuleComponent* owner) :
        portModel_(model),
        portType_(portType),
        owner_(owner),
        bounds_(bounds),
        state_(kIdle)
        {
            ASSERT(portModel_ != nullptr);
            repaint();

            const Rectangle<int>& b = bounds_;
            if (portType_ == kInPort)
            {
                rcSquare_    = Rectangle<int>(b.getX() + 4, b.getY() + 3, b.getX() + 10, b.getY() + 9);
                rcIndicator_ = Rectangle<int>(b.getX(), b.getY() + 5, b.getX() + 4, b.getY() + 7);
                rcHover_     = Rectangle<int>(b.getX(), b.getY(), b.getX() + 12, b.getBottom());
                rcText_      = Rectangle<int>(b.getX() + 13, b.getY(), b.getRight() + 37, b.getBottom());
                rcInvalid_   = Rectangle<int>(b.getX(), b.getY(), b.getRight() + 37, b.getBottom());
            }
            else {
                rcSquare_    = Rectangle<int>(b.getRight() - 10, b.getY() + 3, b.getRight() - 4, b.getY() + 9);
                rcIndicator_ = Rectangle<int>(b.getRight() - 4, b.getY() + 5, b.getRight(), b.getY() + 7);
                rcHover_     = Rectangle<int>(b.getRight() - 12, b.getY(), b.getRight(), b.getBottom());
                rcText_      = Rectangle<int>(b.getX() - 37, b.getY(), b.getRight() - 15, b.getBottom());
                rcInvalid_   = Rectangle<int>(b.getX() - 37, b.getY(), b.getRight(), b.getBottom());
            }
        }


            void PortComponent::mouseEnter(const MouseEvent& e)
        {
            return mouseMove(e);
        }


        void PortComponent::mouseExit(const MouseEvent&)
        {
            state_ &= ~kHover;
            repaint(rcInvalid_);

            //return kMouseEventHandled;
        }


        void PortComponent::mouseDown(const MouseEvent& e)
        {
            return mouseMove(e);
        }


        void PortComponent::mouseMove(const MouseEvent &e)
        {
            uint16_t oldState = state_;
            setHover(e.getMouseDownPosition());
            if (state_ != oldState) {
                repaint(rcInvalid_);
            }
            //return kMouseEventHandled;
        }


        bool PortComponent::testDocking(const Point<int>& pos)
        {
            Point<int> p(pos);
            patchToPort(p);
            return testHover(p);
        }


        bool PortComponent::startDocking(Point<int>& pos)
        {
            patchToPort(pos);
            bool result = setHover(pos);

            if (result)
            {
                getDockPosition(pos);
                state_ |= kDocking;
                repaint(rcInvalid_);
            }
            return result;
        }


        void PortComponent::endDocking(Point<int>& pos)
        {
            if (links_.empty())
                state_ &= ~kConnected;

            patchToPort(pos);
            state_ &= ~kDocking;
            setHover(pos);
            repaint(rcInvalid_);
        }


        void PortComponent::connect(Point<int>& pos, const Link& link)
        {
            patchToPort(pos);
            setHover(pos);
            links_.push_back(link);
            getDockPosition(pos);
            state_ |= kConnected;
        }


        void PortComponent::disconnect(const Link& link)
        {
            std::vector< Link >::iterator pos = std::find(links_.begin(), links_.end(), link);
            if (pos != links_.end()) {
                links_.erase(pos);
            }


            state_ &= ~kDocking;
            if (links_.empty()) {
                state_ &= ~kConnected;
            }
        }


        bool PortComponent::testHover(const Point<int>& pos)
        {
            return rcHover_.contains(pos);
        }


        bool PortComponent::setHover(const Point<int>& pos)
        {
            testHover(pos) ? state_ |= kHover : state_ &= ~kHover;
            return state_ & kHover;
        }


        void PortComponent::getDockPosition(Point<int>& pos)
        {
            Rectangle<int> b = getLocalBounds();
            pos.y = b.getY() + b.getHeight() / 2;
            pos.x = portType_ == kInPort ? b.getX() + 1 : b.getRight() - 1;
            portToPatch(pos);
        }


        void PortComponent::patchToPort(Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getLocalBounds();
            pos.addXY(-rcParent.getX(), -rcParent.getX());
        }


        void PortComponent::portToPatch(Point<int>& pos)
        {
            Rectangle<int> rcParent = owner_->getLocalBounds();
            pos.addXY(rcParent.getX(), rcParent.getY());
        }


        void PortComponent::paint(Graphics& g)
        {
            //CColor colPort1 = (state_ & Connected) ? colors.modulePort2 : colors.modulePort1;
            //CColor colPort2 = colors.modulePort2;
            //if( owner_->data_->catalog_ == MASTER ) {
            //    colPort1 = (state_ & Connected) ? colors.masterPort2 : colors.masterPort1;
            //    colPort2 = colors.masterPort2;
            //}
            //pContext->setFillColor( colPort1 );
            //pContext->setFrameColor( colPort2 );

            //if( state_ & (Hover || Docking) )	    // draw port square
            //{
            //    Rectangle<int> rcFrame( rcSquare_ );
            //    rcFrame.inset( -1, -1 );
            //    pContext->setLineWidth( 1 );
            //    pContext->drawRect( rcFrame, kDrawFilledAndStroked );
            //}
            //else {
            //    pContext->drawRect( rcSquare_, kDrawFilled );
            //}

            //if( links_.empty() == false ) {	    	// draw connection indicator
            //    pContext->drawRect( rcIndicator_, kDrawFilled );
            //}

            //if( state_ & (Hover || Docking) )	    // draw label
            //{
            //    Rectangle<int> oldClip;
            //    pContext->getClipRect( oldClip );
            //    pContext->setClipRect( rcInvalid_ );

            //    pContext->setFont( fontArial9, fontArial9->getSize() );
            //    pContext->setFontColor( colors.moduleText2 );
            //    
            //    pContext->setFillColor( MakeCColor( owner_->colBkgnd_.red, owner_->colBkgnd_.green, owner_->colBkgnd_.blue, 255 ));
            //    pContext->drawRect( rcText_, kDrawFilled );

            //    pContext->drawStringUTF8(info_->label_.c_str(), rcText_, portType_ == kInPort ? kLeftText : kRightText, true);
            //    pContext->setClipRect( oldClip );
            //}
            //setDirty( false );
        }

} // namespace e3