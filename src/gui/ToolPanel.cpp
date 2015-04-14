

#include <e3_Trace.h>
#include <e3_Exception.h>

#include "gui/VoiceMonitor.h"
#include "gui/Style.h"
#include "gui/ToolPanel.h"


namespace e3 {

    ToolPanel::ToolPanel() : Component()
    {
        midiMonitor_ = new VoiceMonitor();
        addAndMakeVisible(midiMonitor_);
    }


    void ToolPanel::addPanelButton(const std::string& buttonLabel, int indexToInsert)
    {
        TextButton* button = new TextButton();
        button->setRadioGroupId(42);  // TODO: use some kind of constant
        button->setClickingTogglesState(true);
        button->setButtonText(buttonLabel);
        button->addListener(this);
        
        addAndMakeVisible(button);
        panelButtons_.insert(indexToInsert, button);
    }


    void ToolPanel::resized()
    {
        Rectangle<int> buttonArea(0, 10, 120, 25);

        for (int i = 0; i < panelButtons_.size(); ++i)
        {
            panelButtons_[i]->setBounds(buttonArea);
            buttonArea = buttonArea.translated(130, 0);
        }

        int left = getLocalBounds().getWidth();
        midiMonitor_->setBounds(Rectangle<int>(left - 142, 10, 142, 25));
    }


    void ToolPanel::selectPanel(int id)
    {
        Button* button = panelButtons_[id];
        if (button != nullptr) {
            button->triggerClick();
        }
    }


    void ToolPanel::buttonClicked(Button* button)
    {
        int index = panelButtons_.indexOf(button);
        if (index > -1) {
            panelSelectedSignal(index);
        }
    }


    void ToolPanel::paint(Graphics &g)
    {
        Colour c = findColour(Style::kBackgroundColourId);
        g.fillAll(c);
    }


    void ToolPanel::monitorMidiEvent(VoiceMonitorEvent evt)
    {
        midiMonitor_->monitor(evt);
    }

} // namespace e3