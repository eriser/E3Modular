
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
#include "gui/ToolPanel.h"
#include "gui/StackedPanel.h"

#include "gui/AudioEditor.h"
#include "core/Processor.h"


namespace e3 {

    ScopedPointer<ApplicationCommandManager> AudioEditor::commandManager_;


    AudioEditor::AudioEditor(Processor* owner) :
        AudioProcessorEditor(owner),
        processor_(owner)
    {
        createComponents();

        if (processor_->isPlugin())  {
            addAndMakeVisible(resizer_ = new ResizableCornerComponent(this, &resizeLimits_));
            Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            resizeLimits_.setSizeLimits(640, 480, r.getWidth(), r.getHeight());
        }

        std::string context = processor_->isPlugin() ? "Plugin" : "Standalone";
        std::string windowState = processor_->getSettings()->getWindowState("Plugin");

        if (restoreWindowStateFromString(windowState) == false) {
            setSize(1000, 800);
        }

        addAndMakeVisible(*toolPanel_);
        addAndMakeVisible(*stackedPanel_);
        toolPanel_->selectPanel(kBrowserPanel);

        ApplicationCommandManager& commandManager = getCommandManager();
        commandManager.registerAllCommandsForTarget(this);
        addKeyListener(commandManager.getKeyMappings());

        setWantsKeyboardFocus(true);

        XmlElement* styleXml = processor_->getSettings()->getStyle();
        style_ = new Style(styleXml);
        setLookAndFeel(style_);

        processor_->getPolyphony()->monitorUpdateSignal.Connect(toolPanel_.get(), &ToolPanel::monitorMidiEvent);
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

        Rectangle<int> r = getLocalBounds();
        stackedPanel_->setBounds(Rectangle<int>(r.getX() + 10, 10, r.getWidth() - 20, r.getHeight() - 70));
        toolPanel_->setBounds(Rectangle<int>(r.getX() + 10, r.getHeight() - 60, r.getWidth() - 20, 50));

        std::string bounds = getScreenBounds().toString().toStdString();
        processor_->getSettings()->setWindowState(bounds, "Plugin");
    }


    bool AudioEditor::perform(const InvocationInfo& info)
    {
        switch (info.commandID)
        {
        case showEditor:     toolPanel_->selectPanel(kEditorPanel); break;
        case showBrowser:    toolPanel_->selectPanel(kBrowserPanel);  break;
        case showAudioSetup: toolPanel_->selectPanel(kAudioPanel); break;
        default: return false;
        }
        return true;
    }
    
    
    bool AudioEditor::restoreWindowStateFromString(const std::string& s)
    {
        StringArray tokens;
        tokens.addTokens(StringRef(s), false);
        tokens.removeEmptyStrings();
        tokens.trim();

        const int firstCoord = 0;

        if (tokens.size() != firstCoord + 4)
            return false;

        Rectangle<int> newPos(tokens[firstCoord].getIntValue(),
            tokens[firstCoord + 1].getIntValue(),
            tokens[firstCoord + 2].getIntValue(),
            tokens[firstCoord + 3].getIntValue());

        setBounds(newPos);
        return true;
    }


    void AudioEditor::createIcon(Image& image)
    {
        MemoryInputStream stream(resources::e3m_icon_small_png, resources::e3m_icon_small_pngSize, false);
        image = ImageFileFormat::loadFrom(stream);
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
        audioPanel_   = selector;
        stackedPanel_ = new StackedPanel();
        toolPanel_    = new ToolPanel();

        stackedPanel_->insertPanel(editorPanel_, kEditorPanel);
        stackedPanel_->insertPanel(browserPanel_, kBrowserPanel);
        stackedPanel_->insertPanel(audioPanel_, kAudioPanel);

        toolPanel_->addPanelButton("Editor", kEditorPanel);
        toolPanel_->addPanelButton("Browser", kBrowserPanel);
        toolPanel_->addPanelButton("Setup", kAudioPanel);

        toolPanel_->panelSelectedSignal.Connect(stackedPanel_.get(), &StackedPanel::showPanel);
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