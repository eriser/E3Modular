
#include <e3_Trace.h>

#include "core/Processor.h"
#include "core/Instrument.h"
#include "gui/Style.h"
#include "gui/ModuleComponent.h"
#include "gui/PortComponent.h"
#include "gui/Wires.h"
#include "gui/ModulePanel.h"


namespace e3 {

    ModulePanel::ModulePanel() : 
        selection_(new ModuleSelection()),
        wires_(new WireManager(this))
    {      
        selection_->addChangeListener(this);
    }


    void ModulePanel::createModules(Processor* processor, XmlElement* instrumentXml)
    {
        removeAllChildren();
        modules_.clear();

        processor_ = processor;
        Instrument* instrument = processor_->getInstrument();

        if (instrument != nullptr && instrumentXml != nullptr) 
        {
            panelXml_ = instrumentXml->getChildByName("module_panel");
            if (panelXml_)
            {
                forEachXmlChildElementWithTagName(*panelXml_, e, "module")
                {
                    String posString = e->getStringAttribute("pos").toStdString();
                    StringArray tokens;
                    int numTokens = tokens.addTokens(posString, false);

                    uint16_t id = (uint16_t)e->getIntAttribute("id", -1);
                    Module* module = instrument->getModule(id);

                    if (module != nullptr && numTokens >= 2)
                    {
                        bool collapsed = e->getBoolAttribute("collapsed", false);
                        ModuleComponent* c = new ModuleComponent(this, module, collapsed);
                        modules_.add(c);
                        addAndMakeVisible(c);
                        c->setTopLeftPosition(tokens[0].getIntValue(), tokens[1].getIntValue());
                    }
                }
                createWires(instrument->getLinks());
            }
        }
    }


    void ModulePanel::createWires(LinkList& links)
    {
        for (LinkList::iterator it = links.begin(); it != links.end(); ++it)
        {
            Link* link             = &(*it);
            PortComponent* inport  = getPort(link, PortTypeInport);
            PortComponent* outport = getPort(link, PortTypeOutport);

            inport->setConnectedState();
            outport->setConnectedState();

            wires_->addWire(outport->getDockingPosition(), inport->getDockingPosition(), link);
        }
    }


    void ModulePanel::paint(Graphics& g)
    {
        Colour bkgndCol = findColour(Style::kContentBackground1ColourId);
        g.fillAll(bkgndCol);
    }


    void ModulePanel::paintOverChildren(Graphics& g)
    {
        wires_->paint(g);
    }


    void ModulePanel::mouseDown(const MouseEvent& e)
    {
        Point<int> pos = e.getMouseDownPosition();

        wires_->selectWiresInArea(Rectangle<int>(pos.x - 1, pos.y - 1, 2, 2));

        selectedModule_        = getModuleAtPosition(pos);
        mouseDownSelectStatus_ = selection_->addToSelectionOnMouseDown(selectedModule_, e.mods);

        if (selection_->getNumSelected() == 0)
        {
            addChildComponent(lasso_);
            lasso_.beginLasso(e, this);
            dragging_ = false;
        }
        else
        {
            selection_->beginDrag(e);
            dragging_ = true;
        }
    }


    void ModulePanel::mouseDrag(const MouseEvent& e)
    {
        if(dragging_) 
        {
            selection_->continueDrag(e);
            repaint();
        } 
        else {
            lasso_.toFront(false);
            lasso_.dragLasso(e);
        }
    }
    

    void ModulePanel::mouseUp(const MouseEvent& e)
    {
        if (dragging_) {
            selection_->endDrag(e);
        }  
        else {
            lasso_.endLasso();
            removeChildComponent(&lasso_);
        }

        bool wasDragging = dragging_ && e.getMouseDownPosition() != e.getPosition();
        selection_->addToSelectionOnMouseUp(selectedModule_, e.mods, wasDragging, mouseDownSelectStatus_);
        dragging_ = false;
    }


    void ModulePanel::findLassoItemsInArea(Array<ModuleComponent*>& results, const Rectangle<int>& area)
    {
        wires_->selectWiresInArea(area);

        for (int i = 0; i < modules_.size(); ++i)
        {
            ModuleComponent* module = modules_.getUnchecked(i);
            Rectangle<int> bounds   = module->getBounds();
            bool selectModule       = bounds.intersects(area);

            selectModule ? results.addIfNotAlreadyThere(module) : results.removeAllInstancesOf(module);
            module->select(selectModule);
        }
    }


    ModuleSelection& ModulePanel::getLassoSelection()
    {
        return *selection_;
    }


    void ModulePanel::changeListenerCallback(ChangeBroadcaster*)
    {
        repaint();
    }


    Rectangle<int> ModulePanel::getUsedArea() const
    {
        Rectangle<int> result;
        for (int i = 0; i < modules_.size(); ++i)
        {
            Rectangle<int> r = modules_[i]->getBounds();
            result = result.getUnion(r);
        }
        return result;
    }


    void ModulePanel::portAction(PortComponent* port, PortAction action, const Point<int>& pos)
    {
        switch (action) {
        case PortActionDocking:         wires_->startDocking(port, pos); break;
        case PortActionUndocking:       wires_->startUndocking(port); break;
        case PortActionContinueDocking: wires_->continueDocking(pos); break;
        case PortActionEndDocking:      wires_->endDocking(pos); break;
        }
    }


    void ModulePanel::checkViewport()
    {
        Rectangle<int> minimum = getParentComponent()->getBounds();     // this is a viewport
        Rectangle<int> used    = getUsedArea();
        Rectangle<int> needed  = minimum.getUnion(used);

        setBounds(needed);
    }


    void ModulePanel::updateWiresForModule(ModuleComponent* module, bool select)
    {
        wires_->updateWiresForModule(module, select);
    }


    ModuleComponent* ModulePanel::getModule(int id)
    {
        for (int i = 0; i < modules_.size(); ++i)
        {
            ModuleComponent* module = modules_[i];
            if (module->getModuleId() == id) {
                return module;
            }
        }
        return nullptr;
    }


    PortComponent* ModulePanel::getPort(Link* link, PortType portType)
    {
        ModuleComponent* module = nullptr;
        
        if (portType == PortTypeInport && link->isRightValid())
        {
            module = getModule( link->rightModule_ );
            ASSERT( module );
        }

        if (portType == PortTypeOutport && link->isLeftValid())
        {
            module = getModule( link->leftModule_ );
            ASSERT( module );
        }

        if (module != nullptr)
        {
            if (portType == PortTypeInport) {
                return module->getPort( link->rightPort_, PortTypeInport );
            }
            else if (portType == PortTypeOutport) {
                return module->getPort( link->leftPort_, PortTypeOutport );
            }
        }
        return nullptr;
    }


    ModuleComponent* ModulePanel::getModuleAtPosition(const Point<int>& pos) const
    {
        for (int i = 0; i < modules_.size(); ++i) {
            ModuleComponent* m = modules_[i];
            if (m->getBounds().contains(pos)) {
                return m;
            }
        }
        return nullptr;
    }


    PortComponent* ModulePanel::getPortAtPosition(const Point<int>& pos) const
    {
        ModuleComponent* module = getModuleAtPosition(pos);
        if (module != nullptr)
        {
            Point<int> p1 = module->getLocalPoint(this, pos);
            return module->getPortAtPosition(p1);
        }
        return nullptr;
    }


    void ModulePanel::storeModulePosition(int moduleId, Point<int> pos)
    {
        if (panelXml_ != nullptr) {
            XmlElement* e = panelXml_->getChildByAttribute("id", String(moduleId));
            if (e != nullptr) {
                e->setAttribute("pos", pos.toString());
            }
        }
    }


    Processor* ModulePanel::getProcessor() const    { return processor_; }

} // namespace e3
