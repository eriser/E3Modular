
#include <e3_Trace.h>

#include "core/Instrument.h"
#include "gui/Style.h"
#include "gui/EditorPanel.h"
#include "gui/ModuleComponent.h"
#include "gui/PortComponent.h"
#include "gui/ModulePanel.h"


namespace e3 {

    ModulePanel::ModulePanel(EditorPanel* owner) : owner_(owner)
    {      
        selection_.addChangeListener(this);
    }


    void ModulePanel::createModules(Instrument* instrument, XmlElement* instrumentXml)
    {
        removeAllChildren();
        modules_.clear();

        if (instrument != nullptr && instrumentXml != nullptr) {
            panelXml_ = instrumentXml->getChildByName("wire_panel");
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
                createWires();
            }
        }
    }


    void ModulePanel::createWires()
    {
        for (int i = 0; i < modules_.size(); ++i) {
            modules_[i]->createWires();
        }
    }


    void ModulePanel::paint(Graphics& g)
    {
        Colour bkgndCol = findColour(Style::kContentBackground1ColourId);
        g.fillAll(bkgndCol);

        //if (wireData_.wire_) {                                                     // draw the wire, that is just being created
        //    wireData_.wire_->draw(g);
        //}
        for (int i = 0; i < modules_.size(); ++i) {     // draw exising wires
            modules_[i]->paintWires(g);
        }
    }


    void ModulePanel::mouseDown(const MouseEvent& e)
    {
        selectedItem_ = getModuleAt(e.getMouseDownPosition());
        mouseDownSelectStatus_ = selection_.addToSelectionOnMouseDown(selectedItem_, e.mods);

        if (selection_.getNumSelected() == 0)
        {
            addChildComponent(lasso_);
            lasso_.beginLasso(e, this);
            dragging_ = false;
        }
        else
        {
            selection_.beginDrag(e);
            dragging_ = true;
        }
    }


    void ModulePanel::mouseDrag(const MouseEvent& e)
    {
        if(dragging_) 
        {
            selection_.continueDrag(e);
            repaint();
        } 
        else {
            lasso_.toFront(false);
            lasso_.dragLasso(e);
        }
    }
    

    void ModulePanel::mouseUp(const MouseEvent& e)
    {
        if (dragging_) 
        {
            selection_.endDrag(e);
        }  
        else {
            lasso_.endLasso();
            removeChildComponent(&lasso_);
        }

        selection_.addToSelectionOnMouseUp(selectedItem_, e.mods, false, mouseDownSelectStatus_);
        dragging_ = false;
    }


    void ModulePanel::findLassoItemsInArea(Array<SelectableItem*>& results, const Rectangle<int>& area)
    {
        for (int i = 0; i < modules_.size(); ++i) 
        {
            modules_[i]->hitTest(results, area);
        }
        for (int i = 0; i < results.size(); ++i) {
            results[i]->select();
        }
    }


    SelectionManager& ModulePanel::getLassoSelection()
    {
        return selection_;
    }


    void ModulePanel::changeListenerCallback(ChangeBroadcaster* broadcaster)
    {
        ASSERT(broadcaster == &selection_);
        TRACE("num selected items: %d\n", selection_.getNumSelected());

        repaint();  // TODO: get bounding rect of all selected items to mimize painting
    }


    void ModulePanel::deselectAll()
    {
        const SelectionManager::ItemArray& a = selection_.getItemArray();
        for (int i = 0; i < a.size(); ++i) {
            a[i]->deselect();
        }
        //selection_.deselectAll();
        repaint();
    }


    ModuleComponent* ModulePanel::getModuleAt(const Point<int>& pos) const
    {
        for (int i = 0; i < modules_.size(); ++i) {
            ModuleComponent* m = modules_[i];
            if (m->getBounds().contains(pos)) {
                return m;
            }
        }
        return nullptr;
    }


    void ModulePanel::storeModulePosition(uint16_t moduleId, Point<int> pos)
    {
        if (panelXml_ != nullptr) {
            XmlElement* e = panelXml_->getChildByAttribute("id", String(moduleId));
            if (e != nullptr) {
                e->setAttribute("pos", pos.toString());
            }
        }
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


    void ModulePanel::checkSize()
    {
        Rectangle<int> minimum = owner_->getViewportBounds();
        Rectangle<int> used    = getUsedArea();
        Rectangle<int> needed  = minimum.getUnion(used);

        setBounds(needed);
    }


    ModuleComponent* ModulePanel::getModule(uint16_t id)
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


    PortComponent* ModulePanel::getPort(const Link& link, PortType portType)
    {
        PortComponent* port = nullptr;
        ModuleComponent* module = getModule(link.leftModule_);
        if (module) {
            port = (portType == kInport) ?
                module->getPort(link.rightPort_, kInport) :
                module->getPort(link.leftPort_, kOutport);
        }
        return port;
    }

} // namespace e3
