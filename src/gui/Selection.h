
#pragma once

#include <vector>
#include "JuceHeader.h"



namespace e3 {

    class SelectableItem 
    {
    public:
        void select()     { selected_ = true; }
        void deselect()   { selected_ = false; }
        bool isSelected() { return selected_; }

        virtual void beginDrag(const MouseEvent&)    {}
        virtual void continueDrag(const MouseEvent&) {}
        virtual void endDrag(const MouseEvent&)      {}

    protected:
        bool selected_ = false;
    };

    
    class SelectionManager : public SelectedItemSet<SelectableItem*>
    {
    public:
        void itemSelected(SelectableItem* item) override    { item->select(); }
        void itemDeselected(SelectableItem* item) override  { item->deselect(); }

        bool addToSelectionOnMouseDown(SelectableItem* item, ModifierKeys modifiers)
        {
            if (item == nullptr)  {
                deselectAll();
                return false;
            }
            else {
                return SelectedItemSet<SelectableItem*>::addToSelectionOnMouseDown(item, modifiers);
            }
        }


        void SelectionManager::beginDrag(const MouseEvent& e)
        {
            for (SelectableItem** item = begin(), ** last = end(); item != last; ++item) {
                (*item)->beginDrag(e);
            }
        }


        void SelectionManager::continueDrag(const MouseEvent& e)
        {
            for (SelectableItem** item = begin(), ** last = end(); item != last; ++item) {
                (*item)->continueDrag(e);
            }
        }
        

        void SelectionManager::endDrag(const MouseEvent& e)
        {
            for (SelectableItem** item = begin(), ** last = end(); item != last; ++item) {
                (*item)->endDrag(e);
            }
        }
    };

} // namespace e3