
#include "core/GlobalHeader.h"
#include "core/Processor.h"
#include "core/Polyphony.h"
#include "gui/Resources.h"
#include "gui/EditorPanel.h"
#include "gui/BrowserPanel.h"
#include "gui/SetupPanel.h"
#include "gui/TabComponent.h"
#include "gui/Monitor.h"

#include "gui/AudioEditor.h"


namespace e3 {

    ScopedPointer<ApplicationCommandManager> AudioEditor::commandManager_;


    AudioEditor::AudioEditor(Processor* owner) :
        AudioProcessorEditor(owner),
        processor_(owner)
    {
        setWantsKeyboardFocus(true);

        ApplicationCommandManager& commandManager = getCommandManager();
        commandManager.registerAllCommandsForTarget(this);
        addKeyListener(commandManager.getKeyMappings());

        createComponents();
        restoreWindowState();

        XmlElement* styleXml = processor_->getSettings()->getStyle();
        style_ = new Style(styleXml);
        setLookAndFeel(style_);

        processor_->getPolyphony()->monitorUpdateSignal.Connect(monitor_.get(), &Monitor::monitor);
    }


    AudioEditor::~AudioEditor()
    {
        //processor_->getSettings()->store();
        processor_->getPolyphony()->monitorUpdateSignal.Disconnect(monitor_.get(), &Monitor::monitor);
        removeKeyListener(getCommandManager().getKeyMappings());
        commandManager_ = nullptr;
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
        processor_->getSettings()->setWindowState(bounds, "Plugin");
    }


    bool AudioEditor::perform(const InvocationInfo& info)
    {
        switch (info.commandID)
        {
        case showEditor:     tabPanel_->setCurrentTabIndex(kEditorPanel); break;
        case showBrowser:    tabPanel_->setCurrentTabIndex(kBrowserPanel);  break;
        case showAudioSetup: tabPanel_->setCurrentTabIndex(kSetupPanel); break;
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
        std::string windowState = processor_->getSettings()->getWindowState(context);

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
        tabPanel_->setCurrentTabIndex(kBrowserPanel);

        monitor_ = new Monitor();
        addAndMakeVisible(monitor_);

        if (processor_->isPlugin()) 
        {
            resizer_ = new ResizableCornerComponent(this, &resizeLimits_);
            addAndMakeVisible(resizer_);
            Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            resizeLimits_.setSizeLimits(640, 480, r.getWidth(), r.getHeight());
        }
    }

    ApplicationCommandManager& AudioEditor::getCommandManager()
    {
        if (commandManager_ == nullptr)
            commandManager_ = new ApplicationCommandManager();

        return *commandManager_;
    }

} // namespace e3