
#include "gui/Style.h"
#include "CommandTarget.h"
#include "gui/TabComponent.h"
#include "gui/TableComponent.h"
#include "gui/BrowserPanel.h"


namespace e3
{
    BrowserPanel::BrowserPanel() : Component()
    {
        instrumentBrowser_ = new TableComponent();
        presetBrowser_     = new Component();

        instrumentBrowser_->addColumn("Name", TableComponent::kInstrumentNameColumn);
        instrumentBrowser_->addColumn("Category", TableComponent::kInstrumentCategoryColumn);
        instrumentBrowser_->addColumn("Comment", TableComponent::kInstrumentCommentColumn);

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtTop, 10);
        tabPanel_->addTab("Instruments", Colours::transparentBlack, instrumentBrowser_, false, 0);
        tabPanel_->addTab("Presets", Colours::transparentBlack, presetBrowser_, false, 1);

        addAndMakeVisible(tabPanel_);

        openButton_ = new TextButton("Open");
        openButton_->setCommandToTrigger(CommandTarget::getCommandManager(), CommandTarget::cmdOpen, true);
        addAndMakeVisible(openButton_);

        saveButton_ = new TextButton("Save");
        saveButton_->setCommandToTrigger(CommandTarget::getCommandManager(), CommandTarget::cmdSave, true);
        addAndMakeVisible(saveButton_);

        saveAsButton_ = new TextButton("Save As");
        saveAsButton_->setCommandToTrigger(CommandTarget::getCommandManager(), CommandTarget::cmdSaveAs, true);
        addAndMakeVisible(saveAsButton_);

        newButton_ = new TextButton("New");
        newButton_->setCommandToTrigger(CommandTarget::getCommandManager(), CommandTarget::cmdNew, true);
        addAndMakeVisible(newButton_);
    }


    void BrowserPanel::resized()
    {
        Rectangle<int> content = getLocalBounds();
        int separator = jmax(300, (int)(content.getWidth() * 0.75));

        tabPanel_->setBounds(content.withWidth(separator));

        openButton_->setBounds(separator +   62, 40, 120, 25);
        saveButton_->setBounds(separator +   62, 80, 120, 25);
        saveAsButton_->setBounds(separator + 62, 120, 120, 25);
        newButton_->setBounds(separator +    62, 160, 120, 25);
    }


    void BrowserPanel::updateContents(XmlElement* root)
    {
        instrumentBrowser_->loadData(root);
    }


    XmlElement* BrowserPanel::getSelectedInstrumentXml()
    {
        return instrumentBrowser_->getActiveItem();
    }

} // namespace e3