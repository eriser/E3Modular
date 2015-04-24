
#include <e3_Trace.h>

#include "core/Instrument.h"
#include "gui/Style.h"
#include "gui/EditorPanel.h"
#include "gui/ModuleComponent.h"
#include "gui/WirePanel.h"


namespace e3 {

    WirePanel::WirePanel(EditorPanel* owner) : owner_(owner)
    {}

    void WirePanel::paint(Graphics& g)
    {
        Colour bkgndCol = findColour(Style::kContentBackground1ColourId);
        g.fillAll(bkgndCol);
    }


    void WirePanel::showInstrument(Instrument* instrument, XmlElement* instrumentXml)
    {
        if (instrument != nullptr && instrumentXml != nullptr) {
            panelXml_ = instrumentXml->getChildByName("wire_panel");
            if (panelXml_)
            {
                children_.clear();

                forEachXmlChildElementWithTagName(*panelXml_, e, "module")
                {
                    String posString = e->getStringAttribute("pos").toStdString();
                    StringArray tokens;
                    int numTokens = tokens.addTokens(posString, false);

                    uint16_t id = (uint16_t)e->getIntAttribute("id", -1);
                    Module* module = instrument->getModule(id);

                    if (module != nullptr && numTokens >= 2)
                    {
                        ModuleComponent* c = new ModuleComponent(this, module);
                        children_.add(c);
                        addAndMakeVisible(c);
                        c->setTopLeftPosition(tokens[0].getIntValue(), tokens[1].getIntValue());
                    }
                }
            }
        }
    }


    void WirePanel::storeModulePosition(uint16_t moduleId, Point<int> pos)
    {
        if (panelXml_ != nullptr) {
            XmlElement* e = panelXml_->getChildByAttribute("id", String(moduleId));
            if (e != nullptr) {
                e->setAttribute("pos", pos.toString());
            }
        }
    }


    Rectangle<int> WirePanel::getUsedArea() const
    {
        Rectangle<int> result;
        for (int i = 0; i < children_.size(); ++i)
        {
            Rectangle<int> r = children_[i]->getBounds();
            result = result.getUnion(r);
        }
        return result;
    }


    void WirePanel::checkSize()
    {
        Rectangle<int> minimum = owner_->getViewportBounds();
        Rectangle<int> used    = getUsedArea();
        Rectangle<int> needed  = minimum.getUnion(used);

        setBounds(needed);
    }

} // namespace e3
