#pragma once

#include <string>
#include "JuceHeader.h"


namespace e3 {

    class TableComponent;

    class EditableTableCell : public Label
    {
    public:
        EditableTableCell(TableComponent* owner);

        void paint(Graphics& g) override;
        void setRowAndColumn( int row, int column );

        void mouseDown(const MouseEvent& e) override;
        void mouseDoubleClick(const MouseEvent& e) override;

    protected:
        void textWasEdited() override;

    private:
        TableComponent* owner_ = nullptr;
        int row_               = 0;
        int column_            = 0;
    };

} // namespace e3
