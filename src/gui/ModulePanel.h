
#pragma once

#include <cstdint>
#include "JuceHeader.h"
#include "core/Port.h"
#include "gui/ModuleSelection.h"


namespace e3 {

    class Processor;
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
        ModulePanel();

        void paint(Graphics& g) override;
        void ModulePanel::paintOverChildren(Graphics& g) override;

        void createModules(Processor* processor, XmlElement* instrumentXml);
        void storeModulePosition(int moduleId, Point<int> pos);

        void mouseDown(const MouseEvent& e) override;
        void mouseDrag(const MouseEvent& e) override;
        void mouseUp(const MouseEvent& e) override;

        ModuleSelection& getLassoSelection() override;
        void findLassoItemsInArea(Array<ModuleComponent*>& results, const Rectangle<int>& area) override;
        void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

        void checkViewport();
        void updateWiresForModule(ModuleComponent* module, bool select);
        Rectangle<int> getUsedArea() const;

        ModuleComponent* getModule(int id);
        PortComponent* getPort(Link* link, PortType portType);

        ModuleComponent* getModuleAtPosition(const Point<int>& pos) const;
        PortComponent* getPortAtPosition(const Point<int>& pos) const;

        void portAction(PortComponent* port, PortAction action, const Point<int>& pos);

        Processor* getProcessor()
            const;

    protected: 
        void createWires(LinkList& links);

        OwnedArray<ModuleComponent> modules_;
        ScopedPointer<WireManager> wires_;

        LassoComponent<ModuleComponent*> lasso_;
        ScopedPointer<ModuleSelection> selection_;
        ModuleComponent* selectedModule_ = nullptr;
        bool mouseDownSelectStatus_ = false;
        bool dragging_ = false;

        XmlElement* panelXml_;
        Processor* processor_;
    };


} // namespace e3