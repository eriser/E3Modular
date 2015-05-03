
#include <algorithm>

#include <e3_Trace.h>

#include "core/GlobalHeader.h"
#include "core/Module.h"
#include "core/Settings.h"
#include "gui/Style.h"
#include "gui/ModulePanel.h"
#include "gui/PortComponent.h"
#include "gui/ModuleComponent.h"


namespace e3 {

    ModuleComponent::ModuleComponent( ModulePanel* owner, Module* module ) :
        panel_( owner ),
        module_( module )
    {
        setInterceptsMouseClicks( false, true );  // owner handles selection, focus and dragging
        setWantsKeyboardFocus( true );

        createPorts();
        calculateSize();
    }


    void ModuleComponent::createPorts()
    {
        for (int i = 0; i < module_->getNumInports(); i++)
        {
            Port* port = module_->getInport( i );
            PortComponent* component = new PortComponent( port, this );
            inports_.add( component );
            addAndMakeVisible( component );
        }

        for (int i = 0; i < module_->getNumOutports(); i++)
        {
            Port* port = module_->getOutport( i );
            PortComponent* component = new PortComponent( port, this );
            outports_.add( component );
            addAndMakeVisible( component );
        }
    }


    void ModuleComponent::resized()
    {
        dragConstrainer_.setMinimumOnscreenAmounts( getHeight(), getWidth(), 0, 0 );
    }


    void ModuleComponent::calculateSize()
    {
        Rectangle<int> content = getLocalBounds();

        int numPorts = jmax( inports_.size(), outports_.size() );
        int height = portHeight_ + numPorts * portHeight_;
        content.setHeight( height );

        int width = 100;
        content.setWidth( width );

        int portWidth = width / 2;
        int top = portHeight_ / 2;
        int offset = top;

        for (int i = 0; i < inports_.size(); i++)
        {
            inports_[i]->setBounds( Rectangle<int>( 0, offset, portWidth, portHeight_ ) );
            offset += portHeight_;
        }

        offset = top;
        for (int i = 0; i < outports_.size(); i++)
        {
            outports_[i]->setBounds( Rectangle<int>( width - portWidth, offset, portWidth, portHeight_ ) );
            offset += portHeight_;
        }
        setBounds( content );
    }


    void ModuleComponent::paint( Graphics& g )
    {
        Rectangle<int> r = getLocalBounds();

        Colour bkgndCol = findColour( Style::kModuleColourId );
        g.setColour( bkgndCol );
        g.fillRect( r );

        int id = Style::kModuleBorderNormalColourId;
        if (isSelected()) id = Style::kModuleBorderSelectedColourId;
        if (hasKeyboardFocus( true )) id = Style::kModuleBorderFocusedColourId;

        g.setColour( findColour( id ) );
        g.drawRect( r );

        g.setColour( findColour( Style::kModuleText1ColourId ) );
        g.drawText( module_->getLabel(), 0, 0, getWidth(), getHeight(), Justification::centred, true );
    }


    void ModuleComponent::beginDrag( const MouseEvent& e )
    {
        MouseEvent e1 = e.getEventRelativeTo( this );

        positionBeforeDragging_ = getPosition();
        dragger_.startDraggingComponent( this, e );
    }


    void ModuleComponent::continueDrag( const MouseEvent& e )
    {
        dragger_.dragComponent( this, e, &dragConstrainer_ );
        panel_->checkViewport();
        panel_->selectWires( this, isSelected() );
    }


    void ModuleComponent::endDrag( const MouseEvent& )
    {
        Point<int> pos = getPosition();
        if (positionBeforeDragging_ != pos) {
            panel_->storeModulePosition( module_->getId(), pos );
        }
    }


    void ModuleComponent::focusGained( FocusChangeType )
    {
        TRACE( "ModuleComponent::focusGained, id=%d\n", getModuleId() );
        panel_->showModuleSignal( getModule() );
    }


    Module* ModuleComponent::getModule() const   { return module_; }
    int ModuleComponent::getModuleId() const     { return module_->getId(); }


    PortComponent* ModuleComponent::getPort( int portId, PortType portType )
    {
        PortList& list = (portType == PortTypeInport) ? inports_ : outports_;

        for (int i = 0; i < list.size(); i++)
        {
            PortComponent* port = list[i];
            if (port && port->getPortId() == portId)
            {
                return port;
            }
        }
        return nullptr;
    }


    PortComponent* ModuleComponent::getPort( const Link& link, PortType portType )
    {
        return panel_->getPort( link, portType );
    }


    void ModuleComponent::getPortPosition( int portId, PortType portType, Point<int>& pos )
    {
        PortComponent* port = getPort( portId, portType );
        ASSERT( port );
        if (port != nullptr) {
            pos = port->getDockingPosition();
        }
    }


    PortComponent* ModuleComponent::getPortAtPosition( const Point<int>& pos ) const
    {
        PortComponent* port = getPortAtPosition( pos, inports_ );
        if (port == nullptr) {
            port = getPortAtPosition( pos, outports_ );
        }
        return port;
    }


    PortComponent* ModuleComponent::getPortAtPosition( const Point<int>& pos, const OwnedArray<PortComponent>& list ) const
    {
        for (int i = 0; i < list.size(); ++i)
        {
            PortComponent* port = list.getUnchecked( i );
            Point<int> p = port->getLocalPoint( this, pos );
            if (port->isDockingPosition( p )) {
                return port;
            }
        }
        return nullptr;
    }


    void ModuleComponent::portAction( PortComponent* port, PortAction action, const Point<int>& pos )
    {
        panel_->portAction( port, action, pos );
    }


    void ModuleComponent::select( bool doSelect )
    {
        if (doSelect != isSelected()) {
            panel_->selectWires( this, doSelect );
        }
        selected_ = doSelect;
    }


    /*
    void ModuleComponent::focusAndSelect(bool focus, bool select)
    {
    bool mustInvalid = selected_ != select || focused_ != focus;

    if (selected_ != select) {
    selected_ = select;
    }
    if (focused_ != focus) {
    focused_ = focus;
    }
    if (mustInvalid) setColors();
    }


    void ModuleComponent::setColors()
    {
    if (data_->catalog_ != MASTER)
    {
    colBkgnd_ = data_->polyphony_ == MONOPHONIC ? colors.moduleMono : colors.module;
    colFrame_ = selected_ ? colors.moduleFrame2 : colors.moduleFrame1;
    colFrame_ = focused_ ? colors.moduleFrame3 : colFrame_;
    colText_ = focused_ ? colors.moduleText2 : colors.moduleText1;
    colPort_ = colors.moduleFrame1;
    }
    else
    {
    colBkgnd_ = colors.master;
    colFrame_ = selected_ ? colors.masterFrame2 : colors.masterFrame1;
    colFrame_ = focused_ ? colors.masterFrame3 : colFrame_;
    colText_ = focused_ ? colors.masterText2 : colors.masterText1;
    colPort_ = colors.masterFrame1;
    }
    setBackgroundColor(colBkgnd_);
    }


    void ModuleComponent::setPortVisibility()
    {
    for (CCView* pSv = pFirstView; pSv; pSv = pSv->pNext)
    {
    if (pSv->pView->isTypeOf("ModulePort")) {
    ModulePort* port = dynamic_cast<ModulePort*>(pSv->pView);
    if (port) {
    port->setVisible(data_->collapsed_ == false);
    }
    }
    }
    }


    ModulePort* ModuleComponent::getPort(uint16_t portId, PortAlignment portAlign)
    {
    for (CCView* pSv = pFirstView; pSv; pSv = pSv->pNext)
    {
    if (pSv->pView->isTypeOf("ModulePort")) {
    ModulePort* port = dynamic_cast<ModulePort*>(pSv->pView);
    if (port && port->info_->id_ == portId && port->align_ == portAlign)
    {
    return port;
    }
    }
    }
    VERIFY(false);
    return NULL;
    }


    void ModuleComponent::getPortPosition(uint16_t portId, PortType portType, Point<int>& pos)
    {
    if (module_->collapsed_ == false)
    {
    PortComponent* port = getPort(portId, portType);
    port->getDockPosition(pos);
    }
    else
    {
    Rectangle<int> bounds = getLocalBounds();
    pos.y = bounds.getY() + bounds.getHeight() / 2;
    pos.x = (portType == PortTypeInport) ? bounds.getX() + 1 : bounds.getRight() - 1;

    Rectangle<int> rcParent = panel_->getLocalBounds();
    pos.offset(-rcParent.getX(), -rcParent.getY());
    }
    }


    void ModuleComponent::drawBackgroundRect(CDrawContext *pContext, CRect& updateRect)
    {
    CViewContainer::drawBackgroundRect(pContext, updateRect);

    CRect rc(size);
    rc.offset(-rc.left, -rc.top);

    pContext->setLineWidth(1);
    pContext->setFillColor(colBkgnd_);
    pContext->setFrameColor(colFrame_);
    pContext->drawRect(rc, kDrawFilledAndStroked);

    if (data_ && data_->collapsed_)    // draw port dummies in collapsed state
    {
    ModuleInfo* info = ModuleInfo::getInfo(data_->catalog_);

    pContext->setFillColor(colPort_);
    if (info && info->inports_.empty() == false)
    {
    CRect rcSquare = CRect(rc.left + 4, rc.top + 9, rc.left + 10, rc.top + 15);
    pContext->drawRect(rcSquare, kDrawFilled);
    }
    if (info && info->outPorts_.empty() == false)
    {
    CRect rcSquare = CRect(rc.right - 10, rc.top + 9, rc.right - 4, rc.top + 15);
    pContext->drawRect(rcSquare, kDrawFilled);
    }
    }
    setDirty(false);
    }
    */

} // namespace e3