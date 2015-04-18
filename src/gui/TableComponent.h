
#pragma once

#include "JuceHeader.h"


namespace e3 {

    class TableComponent : public Component, public TableListBoxModel
    {
    public:
        TableComponent();

        void addColumn(const std::string& name, int id);
        void loadData(XmlElement* data);

        int getNumRows() override;
        bool isRowSelected(int rowNumber);

        void resized() override;

        void paintRowBackground(Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

        Component* refreshComponentForCell(
            int rowNumber, int columnId, bool isRowSelected,  Component* existingComponentToUpdate) override;
        void sortOrderChanged(int newSortColumnId, bool isForwards) override;
        void returnKeyPressed(int lastRowSelected) override;
        void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& e) override;

        void setText(const std::string& text, int rowNumber, int columnId);
        std::string getText(int rowNumber, int columnId);

        XmlElement* getActiveItem() const;
        void setActiveItem(int rowNumber);
        bool isActiveItem(int rowNumber) const;
        int getActiveRowNumber() const;

        enum BankColumnIds {
            kInstrumentNameColumn     = 1,
            kInstrumentCategoryColumn = 2,
            kInstrumentCommentColumn  = 3,
        };

    private:
        std::string getAttributeName(int columnId);

        TableListBox table_;                // the table component itself

        XmlElement* data_ = nullptr;            // component doesn't own this pointer
        XmlElement* activeItem_ = nullptr;    // currently selected item
        int numRows_ = 0;
    };

} // namespace e3
