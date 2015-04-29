
#pragma once

#include <cstdint>

#include "JuceHeader.h"
#include "core/Port.h"
//#include "core/GlobalHeader.h"


namespace e3 {

    class Port;

    class PortComponent : public Component
    {
        friend class ModuleComponent;

    public:
        PortComponent(Port* port, ModuleComponent* owner);

        void resized() override;
        void paint(Graphics& g) override;

        void mouseEnter(const MouseEvent& e) override;
        void mouseExit(const MouseEvent& e)  override;
        void mouseDown(const MouseEvent& e) override;
        void mouseMove(const MouseEvent& e) override;

        bool hitTest(int x, int y) override;
        Point<int> getPortPosition();

        //bool testDocking(const Point<int>& pos);
        bool startDocking();
        void endDocking();

        void connect();
        void disconnect();

        int getPortId() const           { return port_->getId(); }
        PortType getPortType() const    { return port_->getType(); }

        enum State {
            kIdle      = 0,
            kHover     = 1,
            kDocking   = 2,
            kConnected = 4
        };


    protected:
        void translateFromParent(Point<int>& pos);
        void translateToParent(Point<int>& pos);

        bool setHoverState(const Point<int>& pos);

        Port* port_;
        ModuleComponent* owner_;
        uint16_t state_;
        uint16_t numConnections_ = 0;
        Rectangle<int> rcText_, rcSquare_, rcHover_, rcConnector_, rcBkgnd_;
    };

} // namespace e3
