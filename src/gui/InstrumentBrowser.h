
#pragma once

#include "JuceHeader.h"
#include "gui/TableComponent.h"


namespace e3 {

    class InstrumentBrowser : public TableComponent
    {
    public:
        InstrumentBrowser();

        Component* refreshComponentForCell( int rowNumber, int columnId, bool isRowSelected,  Component* componentToUpdate) override;
        void setActiveItem(int rowNumber) override;

        enum ColumnIds {
            IdColumn       = 1,
            NameColumn     = 2,
            CategoryColumn = 3,
            CommentColumn  = 4,
        };

    protected:
        std::string getAttributeName( int columnId ) override;
    };

} // namespace e3
