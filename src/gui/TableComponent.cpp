
#include <string>

#include <e3_Exception.h>
#include <e3_Trace.h>

#include "gui/Style.h"
#include "gui/AudioEditor.h"
#include "gui/EditableTableCell.h"
#include "gui/TableComponent.h"


namespace e3 {

    class XmlSorter
    {
    public:
        XmlSorter( const std::string attributeToSort, bool forwards ) :
            attributeToSort_( attributeToSort ),
            direction_( forwards ? 1 : -1 )
        {}

        int compareElements( XmlElement* first, XmlElement* second ) const
        {
            int result = first->getStringAttribute( StringRef( attributeToSort_ ) )
                .compareNatural( second->getStringAttribute( StringRef( attributeToSort_ ) ) );

            return direction_ * result;
        }

    private:
        std::string attributeToSort_;
        int direction_;
    };


    TableComponent::TableComponent()
    {
        addAndMakeVisible( table_ );
        table_.setModel( this );
        table_.setHeaderHeight( 25 );
        table_.getHeader().setStretchToFitActive( true );
    }


    void TableComponent::addColumn( const std::string& name, int id, int width )
    {
        table_.getHeader().addColumn( name, id, width, std::min<int>(width, 50) );
    }


    int TableComponent::getNumRows()
    {
        return numRows_;
    }


    bool TableComponent::isRowSelected( int rowNumber )
    {
        return table_.isRowSelected( rowNumber );
    }


    void TableComponent::resized()
    {
        Rectangle<int> r = getLocalBounds();
        table_.setBounds( r );
    }


    void TableComponent::paintRowBackground( Graphics&, int, int, int, bool )
    {
    }


    void TableComponent::paintCell( Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected )
    {
        if (data_ == nullptr) return;

        // highlight active item
        if (isActiveItem( rowNumber )) {
            g.setColour( findColour( Style::HighlightColourId ) );
        }
        else {
            g.setColour( findColour( Style::ContentBackground1ColourId ) );
        }
        g.fillRect( Rectangle<float>( 0, 0, (float)width, height - 0.5f ) );

        // highlight selected row
        if (rowIsSelected) {
            g.setColour( findColour( Style::HighlightColourId ).withAlpha( 0.1f ) );
            g.fillRect( Rectangle<float>( 0, 0, (float)width - 1, height - 0.5f ) );
            g.setColour( Colours::black.withAlpha( 0.6f ) );
            g.drawRect( Rectangle<float>( -1, 0, (float)width + 1, height - 0.5f ), 1.f );
        }

        // draw text
        std::string text = getText( rowNumber, columnId );
        g.setColour( Colours::black );
        g.drawText( text, 2, 0, width - 4, height, Justification::centredLeft, true );

        // draw column separator
        g.setColour( findColour( Style::ContentBackground2ColourId ) );
        g.fillRect( Rectangle<float>( (float)width - 1, 0, 1, (float)height ) );
    }


    void TableComponent::sortOrderChanged( int newSortColumnId, bool isForwards )
    {
        if (newSortColumnId != 0)
        {
            XmlSorter sorter( getAttributeName( newSortColumnId ), isForwards );
            data_->sortChildElements( sorter );

            table_.updateContent();
        }
    }


    void TableComponent::cellDoubleClicked( int rowNumber, int, const MouseEvent& )
    {
        setActiveItem( rowNumber );
    }


    void TableComponent::returnKeyPressed( int lastRowSelected )
    {
        setActiveItem( lastRowSelected );
    }


    void TableComponent::setActiveItem( int rowNumber )
    {
        activeItem_ = data_->getChildElement( rowNumber );
        repaint();
    }


    XmlElement* TableComponent::getActiveItem() const
    {
        return activeItem_;
    }


    bool TableComponent::isActiveItem( int rowNumber ) const
    {
        XmlElement* e = data_->getChildElement( rowNumber );
        return e == activeItem_ && activeItem_ != nullptr;
    }


    var TableComponent::getDragSourceDescription( const SparseSet<int>& selectedRows )
    {
        int rowNumber    = selectedRows[0];
        XmlElement* item = data_->getChildElement( rowNumber );

        return (item != nullptr) ? item->getIntAttribute( "id" ) : -1;
    }


    bool TableComponent::isInterestedInDragSource( const SourceDetails& )
    {
        return table_.getHeader().getSortColumnId() == 1 && table_.getHeader().isSortedForwards();
    }


    void TableComponent::itemDragMove( const SourceDetails &dragSourceDetails )
    {
        // force scrolling if needed while dragging
        const Point<int>& p = dragSourceDetails.localPosition;
        int rowNumber       = table_.getInsertionIndexForPosition( p.x, p.y );

        table_.scrollToEnsureRowIsOnscreen( rowNumber );
    }


    void TableComponent::itemDropped( const SourceDetails& dragSourceDetails )
    {
        int oldIndex = dragSourceDetails.description;
        XmlElement* itemMoved = data_->getChildByAttribute( "id", String( oldIndex ) );

        if (itemMoved != nullptr)
        {
            // sort by id
            //XmlSorter sorter1( "id", true );
            //data_->sortChildElements( sorter1 );

            // get index to where the item was moved
            const Point<int>& p = dragSourceDetails.localPosition;
            int insertIndex     = table_.getInsertionIndexForPosition( p.x, p.y );

            // move the item in the xml list
            data_->removeChildElement( itemMoved, false );
            data_->insertChildElement( itemMoved, insertIndex );

            // update id's
            int index = 0;
            forEachXmlChildElementWithTagName( *data_, e, "instrument" ) {
                e->setAttribute( "id", index++ );
            }
                
            // re-sort in configured sorting order
            //table_.getHeader().reSortTable();
            table_.updateContent();
            table_.selectRow( insertIndex );
        }
    }


    std::string TableComponent::getText( int rowNumber, int columnId )
    {
        const XmlElement* xml = data_->getChildElement( rowNumber );
        if (xml != nullptr)
        {
            const std::string name = getAttributeName( columnId );
            return xml->getStringAttribute( StringRef( name ) ).toStdString();
        }
        return "";
    }


    void TableComponent::setText( const std::string& text, int rowNumber, int columnId )
    {
        if (data_ != nullptr) {
            XmlElement* xml = data_->getChildElement( rowNumber );
            if (xml != nullptr) 
            {
                int id           = xml->getIntAttribute( "id", -1 );
                std::string name = getAttributeName( columnId );
                instrumentAttributesSignal( id, name, var(text) );
            }
        }
    }


} // namespace e3