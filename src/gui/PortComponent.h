
#pragma once

#include <cstdint>
#include <vector>

#include "JuceHeader.h"
#include "core/GlobalHeader.h"
#include "core/ModuleBase.h"

namespace e3 {

    class PortInfo;
    class ModuleComponent;


    class PortComponent : public Component
    {
    public:
        PortComponent(const Rectangle<int>& bounds, PortModel* model, PortType portType, ModuleComponent* owner);

        void mouseEnter(const MouseEvent &e) override;
        void mouseExit(const MouseEvent  &e)  override;
        void mouseDown(const MouseEvent  &e) override;
        void mouseMove(const MouseEvent  &e) override;

        void paint(Graphics& g) override;

        bool testDocking(const Point<int>& pos);
        bool startDocking(Point<int>& pos);
        void endDocking(Point<int>& pos);
        void connect(Point<int>& pos, const Link& link);
        void disconnect(const Link& link);
        void getDockPosition(Point<int>& pos);


        enum State {
            kIdle      = 0,
            kHover     = 1,
            kDocking   = 2,
            kConnected = 4
        };

        PortModel* portModel_;
        PortType portType_;
        ModuleComponent* owner_;
        Rectangle<int> bounds_;
        uint16_t state_;
        std::vector<Link> links_;

    protected:
        void patchToPort(Point<int>& pos);
        void portToPatch(Point<int>& pos);

        bool testHover(const Point<int>& pos);
        bool setHover(const Point<int>& pos);

        Rectangle<int> rcText_, rcSquare_, rcHover_, rcIndicator_, rcInvalid_;
    };

} // namespace e3
