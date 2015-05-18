
#pragma once

#include <cstdint>
#include "JuceHeader.h"
#include "core/GlobalHeader.h"
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

        void paint( Graphics& g ) override;
        void paintOverChildren( Graphics& g ) override;

        void createModules( Processor* processor );
        void createModule( int moduleType, Point<int> pos );
        void deleteSelectedModules();
        void saveModulePosition( int moduleId, Point<int> pos, bool isNewModule );

        void mouseDown( const MouseEvent& e ) override;
        void mouseDrag( const MouseEvent& e ) override;
        void mouseUp( const MouseEvent& e ) override;

        ModuleSelection& getLassoSelection() override;
        void findLassoItemsInArea( Array<ModuleComponent*>& results, const Rectangle<int>& area ) override;
        void changeListenerCallback( ChangeBroadcaster* broadcaster ) override;

        void selectAll();
        void deselectAll();
        void focusGained( FocusChangeType ) override;
        void moduleFocusGained( ModuleComponent* module );

        void checkViewport();
        void selectWires( ModuleComponent* module, bool select );
        Rectangle<int> getUsedArea() const;

        ModuleComponent* getModule( int id );
        PortComponent* getPort( const Link& link, PortType portType );

        ModuleComponent* getModuleAtPosition( const Point<int>& pos ) const;
        PortComponent* getPortAtPosition( const Point<int>& pos ) const;

        void portAction( PortComponent* port, PortAction action, const Point<int>& pos );

        Processor* getProcessor() const;
        XmlElement* getPanelXml() const;

        Gallant::Signal1<Instrument*> showInstrumentSignal;
        Gallant::Signal2<Instrument*, Module*> showModuleSignal;

    protected:
        ModuleComponent* createModuleComponent( Module* module, int x, int y );
        void createWires( LinkSet& links );

        void showPopupMenu( Point<int> pos );
        PopupMenu createPopupMenu();
        static void popupMenuCallback( int, ModulePanel* );

        void focusModule( ModuleComponent* module );

        OwnedArray<ModuleComponent> modules_;
        ScopedPointer<WireManager> wires_;

        LassoComponent<ModuleComponent*> lasso_;
        ScopedPointer<ModuleSelection> selection_;
        ModuleComponent* selectedModule_ = nullptr;
        bool mouseDownSelectStatus_ = false;
        bool dragging_ = false;
        Point<int> popupMenuPosition_;

        Processor* processor_;

        enum MenuTags {
            MenuModuleNew      = 1,
            MenuModuleDelete   = 2,
            MenuCopy           = 3,
            MenuCut            = 4,
            MenuPaste          = 5,
            MenuSelectAll      = 6,
            MenuUnselectAll    = 7,
            MenuSubModuleFirst = 10,
        };
    };


} // namespace e3