
#pragma once

#include "JuceHeader.h"


namespace e3 {

    class Instrument;
    class EditorPanel;
    class ModuleComponent;

    class WirePanel : public Component
    {
    public:
        WirePanel(EditorPanel* owner);

        void paint(Graphics& g) override;
        void showInstrument(Instrument* instrument, XmlElement* instrumentXml);
        void storeModulePosition(uint16_t moduleId, Point<int> pos);

        void checkSize();
        Rectangle<int> getUsedArea() const;

    protected: 

        OwnedArray<ModuleComponent> children_;

        XmlElement* panelXml_;
        EditorPanel* owner_;
    };


} // namespace e3