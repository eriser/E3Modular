
#include "gui/TableComponent.h"

namespace e3 {

    TableComponent::TableComponent()
    {
        addAndMakeVisible(table_);
        table_.setModel(this);
        table_.setHeaderHeight(25);
        table_.getHeader().setStretchToFitActive(true);

        table_.getHeader().addColumn("#", 1, 100, 50, 400, TableHeaderComponent::defaultFlags);
        table_.getHeader().addColumn("Name", 2, 100, 50, 400, TableHeaderComponent::defaultFlags);
        table_.getHeader().addColumn("Category", 3, 100, 50, 400, TableHeaderComponent::defaultFlags);
        table_.getHeader().addColumn("Comment", 4, 100, 50, 400, TableHeaderComponent::defaultFlags);

    }


    int TableComponent::getNumRows()
    {
        return numRows_;
    }


    void TableComponent::resized()
    {
        Rectangle<int> r = getLocalBounds();
        table_.setBounds(r);
        //table_.setBoundsInset(BorderSize<int>(8));
    }


    void TableComponent::paintRowBackground(Graphics& g, int, int, int, bool rowIsSelected)
    {
        //Colour c = rowIsSelected ? findColour(kTabButtonOnBackgroundColourId) : findColour();
        //g.fillAll(c);
    }


    void TableComponent::paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {

    }



} // namespace e3