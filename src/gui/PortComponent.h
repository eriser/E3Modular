
#pragma once

#include <cstdint>

#include "JuceHeader.h"
#include "core/Port.h"


namespace e3 {


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
        void mouseDrag(const MouseEvent& e) override;
        void mouseUp(const MouseEvent& e) override;


        bool hitTest(int x, int y) override;
        Point<int> getDockingPosition();
        bool isDockingPosition(const Point<int>& pos) const;

        bool startDocking();
        void cancelDocking();

        void connect();
        void disconnect();
        uint16_t getNumConnections() const { return numConnections_; }

        int getModuleId() const;
        int getPortId() const              { return port_->getId(); }
        PortType getPortType() const       { return port_->getType(); }

        enum State {
            kIdle      = 0,
            kHover     = 1,
            kDocking   = 2
        };


    protected:
        Point<int> translateFromParent(const Point<int>& pos);
        Point<int> translateToParent(const Point<int>& pos);

        //bool setHoverState(const Point<int>& pos);

        Port* port_;
        ModuleComponent* owner_;
        uint16_t state_;
        uint16_t numConnections_ = 0;
        Rectangle<int> rcText_, rcDock_, rcHover_, rcConnector_, rcBkgnd_;
    };

} // namespace e3
