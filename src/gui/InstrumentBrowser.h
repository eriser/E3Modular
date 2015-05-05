
#pragma once

#include "JuceHeader.h"
#include "gui/TableComponent.h"


namespace e3 {

    class InstrumentBrowser : public TableComponent
    {
    public:
        void loadData(XmlElement* data) override;

        Component* refreshComponentForCell( int rowNumber, int columnId, bool isRowSelected,  Component* componentToUpdate) override;
        void returnKeyPressed(int lastRowSelected) override;

        //void setText(const std::string& text, int rowNumber, int columnId) override;
        //std::string getText(int rowNumber, int columnId) override;

        void setActiveItem(int rowNumber) override;
        int getActiveRowNumber() const override;

        enum ColumnIds {
            NameColumn     = 1,
            CategoryColumn = 2,
            CommentColumn  = 3,
        };

    private:
        std::string getAttributeName(int columnId) override;
    };

} // namespace e3
