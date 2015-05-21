
#include <e3_Trace.h>

#include "core/Processor.h"
#include "core/Instrument.h"
#include "core/InstrumentSerializer.h"
#include "modules/ModuleFactory.h"
#include "gui/Style.h"
#include "gui/ModuleComponent.h"
#include "gui/PortComponent.h"
#include "gui/Wires.h"
#include "gui/ModulePanel.h"


namespace e3 {

    ModulePanel::ModulePanel() :
        selection_( new ModuleSelection() ),
        wires_( new WireManager( this ) )
    {
        setWantsKeyboardFocus( true );
        setMouseClickGrabsKeyboardFocus( true );

        selection_->addChangeListener( this );
    }


    void ModulePanel::createModules( Processor* processor )
    {
        selection_->deselectAll();
        removeAllChildren();
        modules_.clear();
        wires_->deleteAll();

        processor_             = processor;
        Instrument* instrument = processor_->getInstrument();
        XmlElement* panelXml   = getPanelXml();

        if (panelXml != nullptr)
        {
            forEachXmlChildElementWithTagName( *panelXml, e, "module" )
            {
                String posString = e->getStringAttribute( "pos" ).toStdString();
                StringArray tokens;
                int numTokens = tokens.addTokens( posString, false );
                ASSERT( numTokens == 2 );

                int id = e->getIntAttribute( "id", -1 );
                Module* module = instrument->getModule( id );
                ASSERT( module );

                if (module != nullptr && numTokens == 2)
                {
                    createModuleComponent( module, tokens[0].getIntValue(), tokens[1].getIntValue() );
                }
            }
            createWires( instrument->getLinks() );
        }
        showInstrumentSignal( instrument );   // update parameterPanel
    }


    void ModulePanel::createWires( LinkSet& links )
    {
        for (LinkSet::iterator it = links.begin(); it != links.end(); ++it)
        {
            const Link& link       = *it;
            PortComponent* inport  = getPort( link, PortTypeInport );
            PortComponent* outport = getPort( link, PortTypeOutport );

            ASSERT( inport );
            ASSERT( outport );
            if (inport && outport)
            {
                inport->setConnectedState();
                outport->setConnectedState();

                wires_->addWire( outport->getDockingPosition(), inport->getDockingPosition(), link );
            }
        }
    }


    void ModulePanel::createModule( int moduleType, Point<int> pos )
    {
        ASSERT( processor_ );
        if (processor_ == nullptr) return;

        Module* module = processor_->addModule( moduleType );
        if (module != nullptr)
        {
            ModuleComponent* comp = createModuleComponent( module, pos.x, pos.y );
            saveModulePosition( module->getId(), pos, true );

            selection_->selectOnly( comp );
            focusModule( comp );
        }
    }


    ModuleComponent* ModulePanel::createModuleComponent( Module* module, int x, int y )
    {
        ASSERT( module );
        if (module != nullptr)
        {
            ModuleComponent* c = new ModuleComponent( this, module );
            modules_.add( c );
            addAndMakeVisible( c );
            c->setTopLeftPosition( x, y );

            return c;
        }
        return nullptr;
    }


    void ModulePanel::deleteSelectedModules()
    {
        ASSERT( processor_ );
        if (processor_ == nullptr) return;

        for (ModuleComponent** it = selection_->begin(), ** last = selection_->end(); it != last; ++it) 
        {
            ModuleComponent* comp = (*it);
            selection_->deselect( comp );
            wires_->deleteWiresForModule( comp );

            processor_->deleteModule( comp->getModule() );

            removeChildComponent( comp );
            modules_.removeObject( comp, true );
        }
        repaint();
    }


    void ModulePanel::paint( Graphics& g )
    {
        Colour bkgndCol = findColour( Style::ContentBackground1ColourId );
        g.fillAll( bkgndCol );
    }


    void ModulePanel::paintOverChildren( Graphics& g )
    {
        wires_->paint( g );
    }


    void ModulePanel::mouseDown( const MouseEvent& e )
    {
        Point<int> pos = e.getMouseDownPosition();

        if (e.mods.isLeftButtonDown())
        {
            wires_->selectWiresInArea( Rectangle<int>( pos.x - 1, pos.y - 1, 2, 2 ) );

            selectedModule_        = getModuleAtPosition( pos );
            mouseDownSelectStatus_ = selection_->addToSelectionOnMouseDown( selectedModule_, e.mods );
            focusModule( selectedModule_ );

            if (selection_->getNumSelected() == 0)
            {
                addChildComponent( lasso_ );
                lasso_.beginLasso( e, this );
                dragging_ = false;
            }
            else
            {
                selection_->beginDrag( e );
                dragging_ = true;
            }
        }
        else if (e.mods.isRightButtonDown())
        {
            showPopupMenu( pos );
        }
    }


    void ModulePanel::mouseDrag( const MouseEvent& e )
    {
        if (dragging_)
        {
            selection_->continueDrag( e );
            repaint();
        }
        else {
            lasso_.toFront( false );
            lasso_.dragLasso( e );
        }
    }


    void ModulePanel::mouseUp( const MouseEvent& e )
    {
        if (dragging_) {
            selection_->endDrag( e );
        }
        else {
            lasso_.endLasso();
            removeChildComponent( &lasso_ );
        }

        bool wasDragging = dragging_ && e.getMouseDownPosition() != e.getPosition();
        selection_->addToSelectionOnMouseUp( selectedModule_, e.mods, wasDragging, mouseDownSelectStatus_ );
        dragging_ = false;
    }


    void ModulePanel::findLassoItemsInArea( Array<ModuleComponent*>& results, const Rectangle<int>& area )
    {
        wires_->selectWiresInArea( area );

        for (int i = 0; i < modules_.size(); ++i)
        {
            ModuleComponent* module = modules_.getUnchecked( i );
            Rectangle<int> bounds   = module->getBounds();
            bool selectModule       = bounds.intersects( area );

            selectModule ? results.addIfNotAlreadyThere( module ) : results.removeAllInstancesOf( module );
            module->select( selectModule );
        }
    }


    ModuleSelection& ModulePanel::getLassoSelection()
    {
        return *selection_;
    }


    void ModulePanel::selectAll()
    {
        for (int i = 0; i < modules_.size(); ++i)
        {
            ModuleComponent* module = modules_.getUnchecked( i );
            if (module->isSelected() == false)
                selection_->addToSelection( module );
        }
    }


    void ModulePanel::deselectAll()
    {
        selection_->deselectAll();
    }

    
    void ModulePanel::changeListenerCallback( ChangeBroadcaster* )
    {
        repaint();
    }


    void ModulePanel::focusModule( ModuleComponent* module )
    {
        module == nullptr ? grabKeyboardFocus() : module->grabKeyboardFocus();
    }


    void ModulePanel::focusGained( FocusChangeType )
    {
        showInstrumentSignal( processor_->getInstrument() );
    }


    void ModulePanel::moduleFocusGained( ModuleComponent* module )
    {
        showModuleSignal( processor_->getInstrument(), module->getModule() );
    }


    Rectangle<int> ModulePanel::getUsedArea() const
    {
        Rectangle<int> result;
        for (int i = 0; i < modules_.size(); ++i)
        {
            Rectangle<int> r = modules_[i]->getBounds();
            result = result.getUnion( r );
        }
        return result;
    }


    void ModulePanel::portAction( PortComponent* port, PortAction action, const Point<int>& pos )
    {
        switch (action) {
        case PortActionDocking:         wires_->startDocking( port, pos ); break;
        case PortActionUndocking:       wires_->startUndocking( port ); break;
        case PortActionContinueDocking: wires_->continueDocking( pos ); break;
        case PortActionEndDocking:      wires_->endDocking( pos ); break;
        }
    }


    void ModulePanel::checkViewport()
    {
        Rectangle<int> minimum = getParentComponent()->getBounds();   
        Rectangle<int> used    = getUsedArea();
        //Rectangle<int> needed  = minimum.getUnion( used );
        Rectangle<int> needed  = minimum.getUnion( used ).withTrimmedBottom( 0 );

        setBounds( needed );
    }


    void ModulePanel::selectWires( ModuleComponent* module, bool select )
    {
        wires_->selectWiresForModule( module, select );
    }


    ModuleComponent* ModulePanel::getModule( int id )
    {
        for (int i = 0; i < modules_.size(); ++i)
        {
            ModuleComponent* module = modules_[i];
            if (module->getModuleId() == id) {
                return module;
            }
        }
        return nullptr;
    }


    PortComponent* ModulePanel::getPort( const Link& link, PortType portType )
    {
        ModuleComponent* module = nullptr;

        if (portType == PortTypeInport && link.isRightValid())
        {
            module = getModule( link.rightModule_ );
            ASSERT( module );
        }

        if (portType == PortTypeOutport && link.isLeftValid())
        {
            module = getModule( link.leftModule_ );
            ASSERT( module );
        }

        if (module != nullptr)
        {
            if (portType == PortTypeInport) {
                return module->getPort( link.rightPort_, PortTypeInport );
            }
            else if (portType == PortTypeOutport) {
                return module->getPort( link.leftPort_, PortTypeOutport );
            }
        }
        return nullptr;
    }


    ModuleComponent* ModulePanel::getModuleAtPosition( const Point<int>& pos ) const
    {
        for (int i = 0; i < modules_.size(); ++i) {
            ModuleComponent* m = modules_[i];
            if (m->getBounds().contains( pos )) {
                return m;
            }
        }
        return nullptr;
    }


    PortComponent* ModulePanel::getPortAtPosition( const Point<int>& pos ) const
    {
        ModuleComponent* module = getModuleAtPosition( pos );
        if (module != nullptr)
        {
            Point<int> p1 = module->getLocalPoint( this, pos );
            return module->getPortAtPosition( p1 );
        }
        return nullptr;
    }


    void ModulePanel::saveModulePosition( int moduleId, Point<int> pos, bool isNewModule )
    {
		InstrumentSerializer::saveModuleComponent( getInstrument(), moduleId, pos, isNewModule );
    }


    void ModulePanel::showPopupMenu( Point<int> pos )
    {
        popupMenuPosition_ = pos;
        Rectangle<int> area = getScreenBounds().translated( pos.x, pos.y ).withSize( 1, 1 );
        PopupMenu::Options options = PopupMenu::Options().withTargetScreenArea( area );

        createPopupMenu().showMenuAsync( options, ModalCallbackFunction::forComponent( popupMenuCallback, this ) );
    }


    void ModulePanel::popupMenuCallback( int result, ModulePanel* panel )
    {
        UNUSED( panel );
        TRACE( "popupMenuCallback, result=%d\n", result );

        switch (result)
        {
        case MenuSelectAll:    panel->selectAll(); break;
        case MenuUnselectAll:  panel->deselectAll(); break;
        case MenuModuleDelete: panel->deleteSelectedModules(); break;

        default: if (result >= MenuSubModuleFirst)
        {
            int moduleType = result - MenuSubModuleFirst;
            panel->createModule( moduleType, panel->popupMenuPosition_ );
        }
        }
    }


    PopupMenu ModulePanel::createPopupMenu()
    {
        PopupMenu menu, modMenu;
        ModuleCatalog& catalog = ModuleFactory::catalog_;
        bool hasSelection = selection_->getNumSelected() > 0;

        for (ModuleCatalog::const_iterator it = catalog.begin(); it != catalog.end(); ++it)
        {
            modMenu.addItem( MenuSubModuleFirst + it->first, it->second );
        }
        menu.addSubMenu( "New Module", modMenu );

        menu.addItem( MenuModuleDelete, "Delete Module", hasSelection );
        menu.addSeparator();
        menu.addItem( MenuCopy, "Copy", false );
        menu.addItem( MenuCut, "Cut", false );
        menu.addItem( MenuPaste, "Paste", false );
        menu.addSeparator();
        menu.addItem( MenuSelectAll, "Select All" );
        menu.addItem( MenuUnselectAll, "Unselect All", hasSelection );

        return menu;
    }



    Processor* ModulePanel::getProcessor() const    { return processor_; }


	Instrument* ModulePanel::getInstrument() const 
	{
		if( processor_ == nullptr ) return nullptr;
		return processor_->getInstrument();
	}


    XmlElement* ModulePanel::getPanelXml() const
    {
		Instrument* instrument = getInstrument();
		if( instrument == nullptr )  return nullptr;

		XmlElement* xml = instrument->getXml();
		if( xml == nullptr ) return nullptr;

		return xml->getChildByName( "panel" );
    }


} // namespace e3
