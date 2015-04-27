
#pragma once

#include "JuceHeader.h"
#include "gui/Selection.h"


namespace e3 {

    class Instrument;
    class EditorPanel;
    class ModuleComponent;
    class PortComponent;


    class ModulePanel : public Component,
                        public LassoSource<SelectableItem*>,
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

        SelectionManager& getLassoSelection() override;
        void findLassoItemsInArea(Array<SelectableItem*>& results, const Rectangle<int>& area) override;
        void changeListenerCallback(ChangeBroadcaster* broadcaster) override;
        void deselectAll();

        void checkSize();
        Rectangle<int> getUsedArea() const;

        ModuleComponent* getModule(uint16_t id);
        PortComponent* getPort(const Link& link, PortType portType);

    protected: 
        void createWires();
        ModuleComponent* getModuleAt(const Point<int>& pos) const;

        OwnedArray<ModuleComponent> modules_;

        LassoComponent<SelectableItem*> lasso_;
        SelectionManager selection_;
        SelectableItem* selectedItem_ = nullptr;
        bool mouseDownSelectStatus_ = false;
        bool dragging_ = false;

        XmlElement* panelXml_;
        EditorPanel* owner_;
    };


} // namespace e3