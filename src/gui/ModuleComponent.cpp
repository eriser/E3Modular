
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

    ModuleComponent::ModuleComponent(ModulePanel* owner, Module* module, bool collapsed) :
        owner_(owner),
        module_(module),
        collapsed_(collapsed)
    {
        setInterceptsMouseClicks(false, true);  // owner handles selection, focus and dragging
        createPorts();
        calculateSize();
        //setVisible((module != nullptr) && (module->style_ & kModuleStyleVisible));
        //setColors();
    }


    ModuleComponent::~ModuleComponent()
    {
        for (WireMap::iterator it = wires_.begin(); it != wires_.end(); it++) {
            delete it->second;
        }
    }


    Module* ModuleComponent::getModule() const   { return module_; }
    uint16_t ModuleComponent::getModuleId() const { return module_->id_; }


    void ModuleComponent::paint(Graphics& g)
    {
        Colour bkgndCol = findColour(Style::kModuleColourId);
        g.fillAll(bkgndCol);

        int id = Style::kModuleBorderNormalColourId;
        if (isSelected()) id = Style::kModuleBorderSelectedColourId;
        if (hasKeyboardFocus(true)) id = Style::kModuleBorderFocusedColourId;

        g.setColour(findColour(id));
        g.drawRect(getLocalBounds());

        g.setColour(findColour(Style::kModuleText1ColourId));
        g.drawText(module_->label_, 0, 0, getWidth(), getHeight(), Justification::centred, true);
    }


    void ModuleComponent::beginDrag(const MouseEvent& e)
    {
        MouseEvent e1 = e.getEventRelativeTo(this);
        
        positionBeforeDragging_ = getPosition();
        dragger_.startDraggingComponent(this, e);
    }


    void ModuleComponent::continueDrag(const MouseEvent& e) 
    {
        dragger_.dragComponent(this, e, &dragConstrainer_);
        updateWires();
        owner_->checkSize();
    }


    void ModuleComponent::endDrag(const MouseEvent&)
    {
        Point<int> pos = getPosition();
        if (positionBeforeDragging_ != pos) {
            owner_->storeModulePosition(module_->id_, pos);
        }
    }


    void ModuleComponent::mouseDoubleClick(const MouseEvent&)
    {
        if (module_)
        {
            collapsed_ = !collapsed_;
            //collapseOrExpand(collapsed);
        }
    }


    PortComponent* ModuleComponent::getPort(int portId, PortType portType)
    {
        PortList& list = (portType == kInport) ? inports_ : outports_;

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


    void ModuleComponent::getPortPosition(int portId, PortType portType, Point<int>& pos)
    {
        if (collapsed_ == false)
        {
            PortComponent* port = getPort(portId, portType);
            ASSERT(port);
            if (port != nullptr) {
                pos = port->getPosition();
            }
        }
        else
        {
            Rectangle<int> bounds = getLocalBounds();
            pos.y = bounds.getY() + bounds.getHeight() / 2;
            pos.x = (portType == kInport) ? bounds.getX() + 1 : bounds.getRight() - 1;

            Rectangle<int> rcParent = owner_->getLocalBounds();
            pos.addXY(-rcParent.getX(), -rcParent.getY());
        }
    }


    void ModuleComponent::createPorts()
    {
        for (size_t i = 0; i < module_->inports_.size(); i++)
        {
            Port* port = module_->inports_[i];
            PortComponent* component = new PortComponent(port, this);
            inports_.add(component);
            addAndMakeVisible(component);
        }

        for (size_t i = 0; i < module_->outports_.size(); i++)
        {
            Port* port = module_->outports_[i];
            PortComponent* component = new PortComponent(port, this);
            outports_.add(component);
            addAndMakeVisible(component);
        }
    }


    void ModuleComponent::resized()
    {
        dragConstrainer_.setMinimumOnscreenAmounts(getHeight(), getWidth(), 0, 0);
    }


    void ModuleComponent::calculateSize()
    {
        Rectangle<int> content = getLocalBounds();

        if (collapsed_ == false)
        {
            int numPorts = jmax(inports_.size(), outports_.size());
            int height   = portHeight_ + numPorts * portHeight_;
            content.setHeight(height);
        }
        else {
            content.setHeight(portHeight_ * 2);
        }
        int width = 100;
        content.setWidth(width);

        int portWidth = width / 2;
        int top       = portHeight_ / 2;
        int offset    = top;

        for (int i = 0; i < inports_.size(); i++)
        {
            inports_[i]->setBounds(Rectangle<int>(0, offset, portWidth, portHeight_));
            offset += portHeight_;
        }

        offset = top;
        for (int i = 0; i < outports_.size(); i++)
        {
            outports_[i]->setBounds(Rectangle<int>(width - portWidth, offset, portWidth, portHeight_));
            offset += portHeight_;
        }
        setBounds(content);
    }


    void ModuleComponent::createWires()
    {
        for (uint16_t i = 0; i < module_->links_.size(); i++)
        {
            Link& link             = module_->getLink(i);
            PortComponent* inport  = getPort(link.rightPort_, kInport);
            PortComponent* outport = owner_->getPort(link, kOutport);

            inport->connect();
            outport->connect();

            ModuleComponent* source = owner_->getModule(link.leftModule_);
            if (source != nullptr)
            {
                Wire* wire = new Wire(outport->getPosition(), inport->getPosition());
                addWire(link, wire);
            }
        }
    }


    void ModuleComponent::addWire(const Link& link, Wire* wire)
    {
        WireMap::iterator pos = wires_.find(link);
        ASSERT(pos == wires_.end());
        ASSERT(wire);

        wires_.insert(WireMap::value_type(link, wire));
    }


    Wire* ModuleComponent::getWire(const Link& link)
    {
        WireMap::iterator pos = wires_.find(link);
        return (pos != wires_.end()) ? pos->second : nullptr;
    }


    void ModuleComponent::hitTest(Array<SelectableItem*>& results, const Rectangle<int>& area)
    {
        Rectangle<int> bounds = getBounds();
        if (bounds.intersects(area)) {
            results.addIfNotAlreadyThere(this);
        }
        else {
            results.removeAllInstancesOf(this);
        }
        hitTestWires(results, area);
    }


    void ModuleComponent::hitTestWires(Array<SelectableItem*>& results, const Rectangle<int>& area)
    {
        for (WireMap::iterator it = wires_.begin(); it != wires_.end() && isVisible(); it++)
        {
            Wire* wire = it->second;
            if (wire->hitTest(area))
            {
                results.addIfNotAlreadyThere(wire);
            }
            else {
                results.removeAllInstancesOf(wire);
            }
        }
    }


    void ModuleComponent::paintWires(Graphics& g)
    {
        for (WireMap::iterator it = wires_.begin(); it != wires_.end() && isVisible(); it++) {
            it->second->paint(g);
        }
    }


    void ModuleComponent::updateWires()
    {
        for (uint16_t i = 0; i < module_->links_.size() && isVisible(); i++)
        {
            const Link& link = module_->getLink(i);
            Wire* wire = getWire(link);
            ASSERT(wire);

            Point<int> pos;
            getPortPosition(link.leftPort_, kInport, pos);
            wire->back() = pos;

            if (link.leftModule_ == module_->id_)     // link to self
            {
                getPortPosition(link.leftPort_, kOutport, pos);
                wire->front() = pos;
            }
        }
    }


    //
    //void ModuleComponent::removeWires(uint16_t targetId)          // remove all wires, from inputs and outputs
    //{
    //    for (uint16_t i = 0; i < data_->links_.size(); i++)      // iterate over all links from and to this box
    //    {
    //        Link& link = module_->getLink(i);

    //        if (link.leftModule_ == targetId) {         // remove wires connected to own inputs
    //            removeWire(link, kInport);
    //        }

    //        if (module_->id_ == targetId)                    // remove wires connected to outputs of other box
    //        {
    //            PortComponent* outPort = owner_->getPort(link, kOutport);
    //            outPort->disconnect(link);
    //        }
    //    }
    //}

 
    /*
    void ModuleComponent::removeWire(const Link& linkInfo, PortType portType)
    {
        if (portType == kOutport)
        {
            ModulePort* outPort = getPort(linkInfo.outputId_, OUT_PORT);
            outPort->disconnect(linkInfo);
        }
        else {
            ModulePort* inPort = getPort(linkInfo.inputId_, IN_PORT);
            inPort->disconnect(linkInfo);
            deleteWire(linkInfo);
        }
    }


    void ModuleComponent::positionWires(ModuleComponent* movedBox)
    {
        uint16_t targetId = movedBox->data_->id_;
        CPoint pos;

        for (uint16_t i = 0; i < data_->links_.size() && isVisible(); i++)
        {
            LinkInfo& linkInfo = data_->getLink(i);
            Wire* wire = getWire(linkInfo);
            ASSERT(wire);

            if (movedBox != this) {
                if (linkInfo.sourceId_ == targetId)
                {
                    movedBox->getDockPosition(linkInfo.outputId_, OUT_PORT, pos);
                    wire->front() = pos;
                }
            }
            else {
                getDockPosition(linkInfo.inputId_, IN_PORT, pos);
                wire->back() = pos;

                if (linkInfo.sourceId_ == data_->id_)     // link to self
                {
                    getDockPosition(linkInfo.outputId_, OUT_PORT, pos);
                    wire->front() = pos;
                }
            }
        }
    }


    void ModuleComponent::deleteWire(const LinkInfo& linkInfo)
    {
        WireMap::iterator pos = wires_.find(linkInfo);
        if (pos != wires_.end())
        {
            Wire* wire = pos->second;
            wires_.erase(pos);
            delete wire;
        }
        else ASSERT(false);
    }


    void ModuleComponent::hitTestWires(const CRect& rc, WireMap& map)
    {
        for (WireMap::iterator it = wires_.begin(); it != wires_.end() && isVisible(); it++)
        {
            Wire* wire = it->second;
            if (wire->hitTest(rc))
            {
                map.insert(WireMap::value_type(it->first, it->second));
            }
        }
    }


    void ModuleComponent::moveTo(CRect& r)
    {
        setViewSize(r);
        setMouseableArea(r);
        data_->setPosition((INT16)r.left, (INT16)r.top);
    }


    void ModuleComponent::collapseOrExpand(bool collapse)
    {
        if (collapse != data_->collapsed_)
        {
            data_->collapsed_ = collapse;
            setPortVisibility();
            calculateSize();

            owner_->onBoxResized(this);
        }
    }


    bool ModuleComponent::isCollapsed()
    {
        return data_->collapsed_;
    }


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
            pos.x = (portType == kInport) ? bounds.getX() + 1 : bounds.getRight() - 1;

            Rectangle<int> rcParent = owner_->getLocalBounds();
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