
#pragma once

#include "JuceHeader.h"
#include "core/Port.h"
#include "gui/PortComponent.h"


namespace e3 {

    class Link;
    class Processor;
    class ModulePanel;
    class ModuleComponent;


    //--------------------------------------------------------------
    // class Wire
    //--------------------------------------------------------------

    class Wire
    {
        friend class WireManager;

    public:
        Wire();
        Wire( const Point<int>& first, const Point<int>& last, Link* link=nullptr );

        bool hitTest( const Rectangle<int>& area );
        Rectangle<int> getBoundingRect();

        void select( bool doSelect ) { selected_ = doSelect; }
        bool isSelected()            { return selected_; }

        void paint( Graphics& g );

    protected:
        static bool segmentIntersectsRectangle( const Rectangle<int>& rc, const Point<int>& a1, const Point<int>& a2 );

        Point<int> first_, last_;
        Link link_;
        bool selected_ = false;
    };


    //--------------------------------------------------------------
    // class WireManager
    //--------------------------------------------------------------

    class WireManager
    {
    public:
        WireManager( ModulePanel* panel );

        void addWire( Point<int> first, Point<int> last, Link* link );
        void deleteWire( Wire* wire );
        Wire* getWire( int index ) const;
        bool containsWire( Wire* wire ) const;
        int getNumWires() const;

        void paint( Graphics& g );

        void selectWiresInArea( const Rectangle<int>& area );
        void updateWiresForModule( ModuleComponent* module, bool selectWire );

        void startDocking( PortComponent* port, const Point<int>& pos );
        void startUndocking( PortComponent* port );
        void continueDocking( const Point<int>& pos );
        void endDocking( const Point<int>& pos );

    protected:
        enum WireError {
            WireErrorNone         = 0,
            WireErrorSameType     = 1,
            WireErrorPortNotFound = 2,
            WireErrorSameModule   = 3,
            WireErrorSamePort     = 4
        };


        Wire* getWire( Link* link, PortType portType=PortTypeUndefined, bool onlySelected=false ) const;
        WireError checkPort( Link& link, PortComponent* port, PortType expectedType );
        void resetDocking();
        std::string printLinks() const;

        ModulePanel* panel_;
        OwnedArray<Wire> wires_;

        Wire dockingWire_;
        Link dockingLink_;
        PortComponent* lastVisitedPort_  = nullptr;
        PortAction state_ = PortActionIdle;
    };



} // namespace e3