
#pragma once

#include <cstdint>
#include "JuceHeader.h"
#include "core/Port.h"
#include "gui/ModuleSelection.h"


namespace e3 {

    class Instrument;
    class EditorPanel;
    class ModuleComponent;
    class PortComponent;
    class WireManager;


    class ModulePanel : public Component,
                        public LassoSource<ModuleComponent*>,
                        public ChangeListener
    {
    public:
        ModulePanel(EditorPanel* owner);

        void paint(Graphics& g) override;
        void createModules(Instrument* instrument, XmlElement* instrumentXml);
        void storeModulePosition(uint16_t moduleId, Point<int> pos);

        void mouseDown(const MouseEvent& e) override;
        void mouseDrag(const MouseEvent& e) override;
        void mouseUp(const MouseEvent& e) override;

        ModuleSelection& getLassoSelection() override;
        void findLassoItemsInArea(Array<ModuleComponent*>& results, const Rectangle<int>& area) override;
        void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

        void checkSize();
        void updateWiresForModule(ModuleComponent* module, bool select);
        Rectangle<int> getUsedArea() const;

        ModuleComponent* getModule(uint16_t id);
        PortComponent* getPort(Link* link, PortType portType);

    protected: 
        void createWires(Instrument* instrument);
        ModuleComponent* getModuleAt(const Point<int>& pos) const;

        OwnedArray<ModuleComponent> modules_;
        ScopedPointer<WireManager> wires_;

        LassoComponent<ModuleComponent*> lasso_;
        ScopedPointer<ModuleSelection> selection_;
        ModuleComponent* selectedModule_ = nullptr;
        bool mouseDownSelectStatus_ = false;
        bool dragging_ = false;

        XmlElement* panelXml_;
        EditorPanel* owner_;
    };


} // namespace e3