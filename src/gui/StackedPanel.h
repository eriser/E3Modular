
#pragma once

#include <string>
#include <map>
#include "JuceHeader.h"


namespace e3 {

    class StackedPanel : public Component
    {
    public:
        StackedPanel() {}

        void insertPanel(Component* panel, int id);
        void showPanel(int id);

        void paint(Graphics &g) override;
        void resized() override;

    private:
        WeakReference<Component> currentPanel_;
        typedef std::map<int, WeakReference<Component> > PanelMap;
        PanelMap panels_;
    };

} // namespace e3