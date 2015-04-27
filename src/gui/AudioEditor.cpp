
#include <e3_Trace.h>

#include "core/GlobalHeader.h"
#include "core/Settings.h"
#include "core/Processor.h"
#include "core/Polyphony.h"
#include "gui/Style.h"
#include "gui/Resources.h"
#include "gui/EditorPanel.h"
#include "gui/BrowserPanel.h"
#include "gui/SetupPanel.h"
#include "gui/TabComponent.h"
#include "gui/MonitorComponent.h"

#include "gui/AudioEditor.h"


namespace e3 {

    AudioEditor::AudioEditor(Processor* owner) :
        AudioProcessorEditor(owner),
        processor_(owner)
    {
        createComponents();
        restoreWindowState();

        ApplicationCommandManager* commandManager = getCommandManager();
        commandManager->registerAllCommandsForTarget(this);
        addKeyListener(commandManager->getKeyMappings());
        setWantsKeyboardFocus(true);

        setLookAndFeel(Settings::getInstance().getStyle());

        processor_->getPolyphony()->monitorUpdateSignal.Connect(monitor_.get(), &MonitorComponent::monitor);
        
        browserPanel_->updateContents(processor_->getBankXml());
        editorPanel_->showInstrument(processor_->getInstrument(), browserPanel_->getSelectedInstrumentXml());
    }


    AudioEditor::~AudioEditor()
    {
        processor_->getPolyphony()->monitorUpdateSignal.Disconnect(monitor_.get(), &MonitorComponent::monitor);
        removeKeyListener(getCommandManager()->getKeyMappings());

        Settings::getInstance().store();
    }


    void AudioEditor::paint(Graphics& g)
    {
        g.fillAll(findColour(Style::kBackgroundColourId));
    }


    void AudioEditor::resized()
    {
        if (resizer_ != nullptr)
            resizer_->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);

        int indent = 15;

        Rectangle<int> r = getLocalBounds();
        tabPanel_->setBounds(r.reduced(indent, indent));

        r = Rectangle<int>(r.getWidth() - 190 - indent, r.getHeight() - 25 - indent, 190, 25);
        monitor_->setBounds(r);

        std::string bounds = getScreenBounds().toString().toStdString();
        Settings::getInstance().setWindowState(bounds, "Plugin");
    }


    bool AudioEditor::perform(const InvocationInfo& info)
    {
        switch (info.commandID)
        {
        case cmdShowEditor:     tabPanel_->setCurrentTabIndex(kEditorPanel); break;
        case cmdShowBrowser:    tabPanel_->setCurrentTabIndex(kBrowserPanel);  break;
        case cmdShowSetup:      tabPanel_->setCurrentTabIndex(kSetupPanel); break;
        case cmdOpen:           onOpenBank(); break;
        case cmdSave:           onSaveBank(false); break;
        case cmdSaveAs:         onSaveBank(true); break;
        case cmdNew:            onNewBank(); break;
        case cmdLoadInstrument: onLoadInstrument(); break;

        default: return false;
        }
        return true;
    }
    
    
    void AudioEditor::createIcon(Image& image)
    {
        MemoryInputStream stream(resources::e3m_icon_small_png, resources::e3m_icon_small_pngSize, false);
        image = ImageFileFormat::loadFrom(stream);
    }


    void AudioEditor::restoreWindowState()
    {
        std::string context     = processor_->isPlugin() ? "Plugin" : "Standalone";
        std::string windowState = Settings::getInstance().getWindowState(context);

        StringArray tokens;
        tokens.addTokens(StringRef(windowState), false);
        tokens.removeEmptyStrings();
        tokens.trim();

        if (tokens.size() == 4)
        {
            Rectangle<int> content(
                tokens[0].getIntValue(),
                tokens[1].getIntValue(),
                tokens[2].getIntValue(),
                tokens[3].getIntValue());

            setBounds(content);
        }
        else {
            setSize(1000, 800);
        }
    }


    void AudioEditor::createComponents()
    {
        editorPanel_  = new EditorPanel();
        browserPanel_ = new BrowserPanel();
        setupPanel_   = new SetupPanel();

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtBottom, 10);
        tabPanel_->addTab("Editor", Colours::transparentBlack, editorPanel_, false, kEditorPanel);
        tabPanel_->addTab("Browser", Colours::transparentBlack, browserPanel_, false, kBrowserPanel);
        tabPanel_->addTab("Setup", Colours::transparentBlack, setupPanel_, false, kSetupPanel);

        addAndMakeVisible(tabPanel_);
        tabPanel_->setCurrentTabIndex(kEditorPanel);

        monitor_ = new MonitorComponent();
        addAndMakeVisible(monitor_);

        if (processor_->isPlugin()) 
        {
            resizer_ = new ResizableCornerComponent(this, &resizeLimits_);
            addAndMakeVisible(resizer_);
            Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            resizeLimits_.setSizeLimits(640, 480, r.getWidth(), r.getHeight());
        }
    }


    void AudioEditor::onOpenBank()
    {
        // TODO: store current ModulePanel
        FileChooser fc("Open Bank",
            File::getCurrentWorkingDirectory(),
            "*.e3mb",
            true);

        if (fc.browseForFileToOpen())
        {
            std::string path = fc.getResult().getFullPathName().toStdString();
            processor_->openBank(path);
            browserPanel_->updateContents(processor_->getBankXml());
            processor_->loadInstrument();
            editorPanel_->showInstrument(processor_->getInstrument(), browserPanel_->getSelectedInstrumentXml());
        }
    }


    void AudioEditor::onSaveBank(bool askForFilename)
    {
        if (askForFilename) 
        {
            FileChooser fc("Save Bank As",
                File::getCurrentWorkingDirectory(),
                "*.e3mb",
                true);

            if (fc.browseForFileToSave(true))
            {
                File file = fc.getResult();
                processor_->saveBank(file.getFullPathName().toStdString());
            }
        }
        else {
            processor_->saveBank();
        }
    }


    void AudioEditor::onNewBank()
    {
        processor_->newBank();
        browserPanel_->updateContents(processor_->getBankXml());
        processor_->loadInstrument();
    }


    void AudioEditor::onLoadInstrument()
    {
        XmlElement* e = browserPanel_->getSelectedInstrumentXml();
        if (e != nullptr)
        {
            int hash = e->getIntAttribute("hash");
            processor_->loadInstrument(hash);
            editorPanel_->showInstrument(processor_->getInstrument(), e);
        }
    }


} // namespace e3