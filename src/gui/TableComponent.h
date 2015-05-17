
#pragma once

#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {

    class TableComponent : 
        public Component, 
        public TableListBoxModel, 
        public DragAndDropContainer,
        public DragAndDropTarget
    {
    public:
        TableComponent();

        virtual void addColumn(const std::string& name, int id, int width);
        virtual void loadData( XmlElement* ) = 0;

        int getNumRows() override;
        bool isRowSelected(int rowNumber);

        void resized() override;

        void paintRowBackground(Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        void returnKeyPressed(int lastRowSelected) override;
        void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& e) override;

        virtual void setText( const std::string& text, int rowNumber, int columnId );
        virtual std::string getText( int rowNumber, int columnId );

        virtual void setActiveItem( int rowNumber );
        virtual XmlElement* getActiveItem() const;
        virtual int getActiveRowNumber() const = 0;
        virtual bool isActiveItem( int rowNumber ) const;

        var getDragSourceDescription( const SparseSet<int>& selectedRows ) override;
        bool isInterestedInDragSource( const SourceDetails& dragSourceDetails ) override;
        void itemDragMove( const SourceDetails &dragSourceDetails ) override;
        void itemDropped( const SourceDetails& dragSourceDetails ) override;

        Gallant::Signal3<int, const std::string&, var> instrumentAttributesSignal;  // instrumentId, attributeName, value

    protected:
        virtual std::string getAttributeName( int columnId ) = 0;

        TableListBox table_;                  // the table component itself

        XmlElement* data_       = nullptr;    // component doesn't own this pointer
        XmlElement* activeItem_ = nullptr;    // currently selected item
        int numRows_            = 0;
    };

} // namespace e3
