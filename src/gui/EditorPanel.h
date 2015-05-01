
#pragma once

#include "JuceHeader.h"

namespace e3
{
    class ModulePanel;
    class ParameterPanel;


    class EditorPanel : public Component
    {
    public:
        void setContent(ModulePanel* modulePanel, ParameterPanel* parameterPanel);
        void resized() override;

    protected:
        Viewport viewport_;
        ModulePanel* modulePanel_;
        ParameterPanel* parameterPanel_;
    };


} // namespace e3