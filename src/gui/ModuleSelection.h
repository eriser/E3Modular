
#pragma once

#include "JuceHeader.h"
#include "gui/ModuleComponent.h"



namespace e3 {

    class ModuleSelection : public SelectedItemSet<ModuleComponent*>
    {
    public:
        void itemSelected(ModuleComponent* module) override    { module->select(true); }
        void itemDeselected(ModuleComponent* module) override  { module->select(false); }

        bool addToSelectionOnMouseDown(ModuleComponent* module, ModifierKeys modifiers)
        {
            if (module == nullptr)  {
                deselectAll();
                return false;
            }
            else {
                return SelectedItemSet<ModuleComponent*>::addToSelectionOnMouseDown(module, modifiers);
            }
        }


        void ModuleSelection::beginDrag(const MouseEvent& e)
        {
            for (ModuleComponent** iterator = begin(), ** last = end(); iterator != last; ++iterator) {
                (*iterator)->beginDrag(e);
            }
        }


        void ModuleSelection::continueDrag(const MouseEvent& e)
        {
            for (ModuleComponent** iterator = begin(), ** last = end(); iterator != last; ++iterator) {
                (*iterator)->continueDrag(e);
            }
        }
        

        void ModuleSelection::endDrag(const MouseEvent& e)
        {
            for (ModuleComponent** iterator = begin(), ** last = end(); iterator != last; ++iterator) {
                (*iterator)->endDrag(e);
            }
        }


        void ModuleSelection::getBoundingRect(Rectangle<int>& r)
        {
            for (ModuleComponent** iterator = begin(), ** last = end(); iterator != last; ++iterator) {
                r = r.getUnion((*iterator)->getBounds());
            }
        }
    };

} // namespace e3