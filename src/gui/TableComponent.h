
#pragma once

#include "JuceHeader.h"
#include "core/GlobalHeader.h"


namespace e3 {

    class TableComponent : public Component, public TableListBoxModel
    {
    public:
        TableComponent();

        virtual void addColumn(const std::string& name, int id);
        virtual void loadData( XmlElement* ) {}

        int getNumRows() override;
        bool isRowSelected(int rowNumber);

        void resized() override;

        void paintRowBackground(Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        //void returnKeyPressed(int lastRowSelected) override;
        void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& e) override;

        virtual void setText( const std::string& text, int rowNumber, int columnId );
        virtual std::string getText( int rowNumber, int columnId );

        virtual XmlElement* getActiveItem() const;
        virtual void setActiveItem( int rowNumber ) = 0;
        virtual int getActiveRowNumber() const = 0;
        virtual bool isActiveItem( int rowNumber ) const;

        Gallant::Signal3<int, const std::string&, var> instrumentAttributesSignal;  // instrumentId, attributeName, value

    protected:
        virtual std::string getAttributeName( int columnId ) = 0;

        TableListBox table_;                  // the table component itself

        XmlElement* data_       = nullptr;    // component doesn't own this pointer
        XmlElement* activeItem_ = nullptr;    // currently selected item
        int numRows_            = 0;
    };

} // namespace e3
