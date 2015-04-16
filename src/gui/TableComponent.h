
#pragma once

#include "JuceHeader.h"

namespace e3 {

    class TableComponent : public Component, public TableListBoxModel
    {
    public:
        TableComponent();

        int getNumRows() override;
        void resized() override;

        void paintRowBackground(Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    private:
        TableListBox table_;     // the table component itself
        int numRows_ = 0;

    };

} // namespace e3
