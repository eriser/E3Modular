
#pragma once

#include <vector>
#include "JuceHeader.h"



namespace e3 {

    class SelectableItem 
    {
    public:
        void select()           { selected_ = true; }
        void deselect()         { selected_ = false; }
        bool isSelected()       { return selected_; }

        virtual void beginDrag(const MouseEvent&) {}
        virtual void continueDrag(const MouseEvent&) {}
        virtual void endDrag(const MouseEvent&) {}

    protected:
        bool selected_ = false;
    };

    /*
    class __SelectionManager__ : public std::vector<SelectableItem*>, public ChangeBroadcaster
    {
    public:
        void selectOnly(SelectableItem* item) {}
        void addToSelection(SelectableItem* item) {}
        void addToSelectionBasedOnModifiers(SelectableItem* item, ModifierKeys modifiers) {}
        bool addToSelectionOnMouseDown(SelectableItem* item, ModifierKeys modifiers) { return false; }
        void addToSelectionOnMouseUp(SelectableItem* item, ModifierKeys modifiers,
            const bool wasItemDragged,
            const bool resultOfMouseDownSelectMethod) {}

        void deselect(SelectableItem* item) {}
        void deselectAll() {}

        int getNumSelected() const { return size(); }
        SelectableItem* getSelectedItem(int index) const  { return at(index); }
        bool isSelected(SelectableItem* item) const { return std::find(begin(), end(), item) != end(); }

        void setDragging(bool b)    { dragging_ = b; }
        bool isDragging() const     { return dragging_; }

        void changed()
        {
            sendChangeMessage();
        }

    protected:
        bool dragging_ = false;
    };
    */

    
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
        
        
        //    void setDragging(bool b)    { dragging_ = b; }
    //    bool isDragging() const     { return dragging_; }
    //    
    //protected:
    //    bool dragging_ = false;
    };

} // namespace e3