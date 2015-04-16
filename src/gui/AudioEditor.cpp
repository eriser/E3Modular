
#ifdef BUILD_TARGET_APP
#include "app/AudioEngine.h"
#include "app/Application.h"
#include "core/Processor.h"
#include "core/Polyphony.h"
#endif

#include "core/GlobalHeader.h"
#include "gui/Resources.h"
#include "gui/EditorPanel.h"
#include "gui/BrowserPanel.h"
#include "gui/TabComponent.h"
#include "gui/VoiceMonitor.h"

#include "gui/AudioEditor.h"
#include "core/Processor.h"


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

        processor_->getPolyphony()->monitorUpdateSignal.Connect(voiceMonitor_.get(), &VoiceMonitor::monitor);
    }


    AudioEditor::~AudioEditor()
    {
        //processor_->getSettings()->store();
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

        r = Rectangle<int>(r.getWidth() - 142 - indent, r.getHeight() - 25 - indent, 142, 25);
        voiceMonitor_->setBounds(r);

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



#ifdef BUILD_TARGET_APP
    void AudioEditor::createComponents()
    {
        AudioEngine* engine         = Application::getApp()->getAudioEngine();
        AudioDeviceManager* manager = engine->getDeviceManager();
        Processor* processor        = engine->getProcessor();

        AudioDeviceSelectorComponent* selector = new AudioDeviceSelectorComponent(
            *manager,
            processor->getNumInputChannels(),
            processor->getNumInputChannels(),
            processor->getNumOutputChannels(),
            processor->getNumOutputChannels(),
            true, false,
            true, false);

        editorPanel_  = new EditorPanel();
        browserPanel_ = new BrowserPanel();
        setupPanel_   = selector;

        tabPanel_ = new TabComponent(TabbedButtonBar::TabsAtBottom, 10);
        tabPanel_->addTab("Editor", Colours::transparentBlack, editorPanel_, false, kEditorPanel);
        tabPanel_->addTab("Browser", Colours::transparentBlack, browserPanel_, false, kBrowserPanel);
        tabPanel_->addTab("Setup", Colours::transparentBlack, setupPanel_, false, kSetupPanel);

        addAndMakeVisible(tabPanel_);
        tabPanel_->setCurrentTabIndex(kBrowserPanel);

        voiceMonitor_ = new VoiceMonitor();
        addAndMakeVisible(voiceMonitor_);

        if (processor_->isPlugin()) 
        {
            resizer_ = new ResizableCornerComponent(this, &resizeLimits_);
            addAndMakeVisible(resizer_);
            Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            resizeLimits_.setSizeLimits(640, 480, r.getWidth(), r.getHeight());
        }
    }
#endif

#ifdef BUILD_TARGET_VST
    void AudioEditor::createComponents()
    {
        editorPanel_  = new EditorPanel();
        browserPanel_ = new BrowserPanel();
        stackedPanel_ = new StackedPanel();
        toolPanel_    = new ToolPanel();

        stackedPanel_->insertPanel(editorPanel_, kEditorPanel);
        stackedPanel_->insertPanel(browserPanel_, kBrowserPanel);

        toolPanel_->addPanelButton("Editor", kEditorPanel);
        toolPanel_->addPanelButton("Browser", kBrowserPanel);

        toolPanel_->panelSelectedSignal.Connect(stackedPanel_.get(), &StackedPanel::showPanel);
    }
#endif

    ApplicationCommandManager& AudioEditor::getCommandManager()
    {
        if (commandManager_ == nullptr)
            commandManager_ = new ApplicationCommandManager();

        return *commandManager_;
    }

} // namespace e3