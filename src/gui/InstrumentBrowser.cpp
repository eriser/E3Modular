
#include <string>
#include <e3_Exception.h>
#include <e3_Trace.h>
#include "gui/AudioEditor.h"
#include "gui/EditableTableCell.h"
#include "gui/InstrumentBrowser.h"


namespace e3 {

    InstrumentBrowser::InstrumentBrowser() : TableComponent( "instrument" )
    {
        table_.setMultipleSelectionEnabled(true);
    }


    Component* InstrumentBrowser::refreshComponentForCell( int rowNumber, int columnId, bool, Component* componentToUpdate )
    {
        if (data_ == nullptr) return nullptr;

        if (columnId == NameColumn || columnId == CategoryColumn || columnId == CommentColumn)
        {
            EditableTableCell* editor = dynamic_cast<EditableTableCell*>(componentToUpdate);

            if (editor == nullptr) {
                editor = new EditableTableCell( this );
            }
            editor->setRowAndColumn( rowNumber, columnId );
            return editor;
        }
        else {
            ASSERT( componentToUpdate == nullptr );
            return nullptr;
        }
    }


    void InstrumentBrowser::setActiveItem( int rowNumber )
    {
        UNUSED( rowNumber );
        //TableComponent::setActiveItem( rowNumber );
        //AudioEditor::getCommandManager()->invokeDirectly( AudioEditor::cmdLoadInstrument, false );
    }


    std::string InstrumentBrowser::getAttributeName( int columnId )
    {
        switch (columnId)
        {
        case IdColumn:       return "id";
        case NameColumn:     return "name";
        case CategoryColumn: return "category";
        case CommentColumn:  return "comment";
        }
        ASSERT( false ); 
        return "";
    }


} // namespace e3