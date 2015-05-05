
#include <e3_Trace.h>

#include "core/GlobalHeader.h"
#include "core/Settings.h"
#include "core/Processor.h"
#include "core/Polyphony.h"
#include "gui/Style.h"
#include "gui/Resources.h"
#include "gui/EditorPanel.h"
#include "gui/ModulePanel.h"
#include "gui/ParameterPanel.h"
#include "gui/BrowserPanel.h"
#include "gui/SetupPanel.h"
#include "gui/InstrumentBrowser.h"
#include "gui/TabComponent.h"
#include "gui/MonitorComponent.h"

#include "gui/AudioEditor.h"


namespace e3 {

    AudioEditor::AudioEditor( Processor* processor ) :
        AudioProcessorEditor( processor ),
        processor_( processor )
    {
        Style::getInstance().setXml( Settings::getInstance().getStyleXml() );

        createComponents();
        restoreWindowState();

        ApplicationCommandManager* commandManager = getCommandManager();
        commandManager->registerAllCommandsForTarget( this );
        addKeyListener( commandManager->getKeyMappings() );
        setWantsKeyboardFocus( true );

        setLookAndFeel( &Style::getInstance() );

        processor_->getPolyphony()->monitorUpdateSignal.Connect( monitor_.get(), &MonitorComponent::monitor );
        
        modulePanel_->showInstrumentSignal.Connect( parameterPanel_.get(), &ParameterPanel::showInstrument );
        modulePanel_->showModuleSignal.Connect( parameterPanel_.get(), &ParameterPanel::showModule );

        parameterPanel_->instrumentAttributesSignal.Connect( this, &AudioEditor::parameterPanelAttributesChanged );
        instrumentBrowser_->instrumentAttributesSignal.Connect( this, &AudioEditor::browserPanelAttributesChanged );

        browserPanel_->updateContents( processor_->getBankXml() );
        modulePanel_->createModules( processor_, browserPanel_->getSelectedInstrumentXml() );
    }


    AudioEditor::~AudioEditor()
    {
        modulePanel_->showInstrumentSignal.Disconnect( parameterPanel_.get(), &ParameterPanel::showInstrument );
        modulePanel_->showModuleSignal.Disconnect( parameterPanel_.get(), &ParameterPanel::showModule );

        parameterPanel_->instrumentAttributesSignal.Disconnect( this, &AudioEditor::parameterPanelAttributesChanged );
        instrumentBrowser_->instrumentAttributesSignal.Disconnect( this, &AudioEditor::browserPanelAttributesChanged );

        processor_->getPolyphony()->monitorUpdateSignal.Disconnect( monitor_.get(), &MonitorComponent::monitor );
        removeKeyListener( getCommandManager()->getKeyMappings() );

        Settings::getInstance().store();
    }


    void AudioEditor::paint( Graphics& g )
    {
        g.fillAll( findColour( Style::BackgroundColourId ) );
    }


    void AudioEditor::resized()
    {
        if (resizer_ != nullptr)
            resizer_->setBounds( getWidth() - 16, getHeight() - 16, 16, 16 );

        int indent = 15;

        Rectangle<int> r = getLocalBounds();
        tabPanel_->setBounds( r.reduced( indent, indent ) );

        r = Rectangle<int>( r.getWidth() - 190 - indent, r.getHeight() - 25 - indent, 190, 25 );
        monitor_->setBounds( r );

        std::string bounds = getScreenBounds().toString().toStdString();
        Settings::getInstance().setWindowState( bounds, "Plugin" );
    }


    bool AudioEditor::perform( const InvocationInfo& info )
    {
        switch (info.commandID)
        {
        case cmdShowEditor:     tabPanel_->setCurrentTabIndex( kEditorPanel ); break;
        case cmdShowBrowser:    tabPanel_->setCurrentTabIndex( kBrowserPanel );  break;
        case cmdShowSetup:      tabPanel_->setCurrentTabIndex( kSetupPanel ); break;
        case cmdOpen:           onOpenBank(); break;
        case cmdSave:           onSaveBank( false ); break;
        case cmdSaveAs:         onSaveBank( true ); break;
        case cmdNew:            onNewBank(); break;
        case cmdLoadInstrument: onLoadInstrument(); break;

        default: return false;
        }
        return true;
    }


    void AudioEditor::createIcon( Image& image )
    {
        MemoryInputStream stream( resources::e3m_icon_small_png, resources::e3m_icon_small_pngSize, false );
        image = ImageFileFormat::loadFrom( stream );
    }


    void AudioEditor::restoreWindowState()
    {
        std::string context     = processor_->isPlugin() ? "Plugin" : "Standalone";
        std::string windowState = Settings::getInstance().getWindowState( context );

        StringArray tokens;
        tokens.addTokens( StringRef( windowState ), false );
        tokens.removeEmptyStrings();
        tokens.trim();

        if (tokens.size() == 4)
        {
            Rectangle<int> content(
                tokens[0].getIntValue(),
                tokens[1].getIntValue(),
                tokens[2].getIntValue(),
                tokens[3].getIntValue() );

            setBounds( content );
        }
        else {
            setSize( 1000, 800 );
        }
    }


    void AudioEditor::createComponents()
    {
        modulePanel_       = new ModulePanel();
        parameterPanel_    = new ParameterPanel();
        editorPanel_       = new EditorPanel();
        browserPanel_      = new BrowserPanel();
        setupPanel_        = new SetupPanel();
        instrumentBrowser_ = new InstrumentBrowser();
        presetBrowser_     = new Component();

        editorPanel_->setComponents( modulePanel_, parameterPanel_ );
        browserPanel_->setComponents( instrumentBrowser_, presetBrowser_ );

        tabPanel_ = new TabComponent( TabbedButtonBar::TabsAtBottom, 10 );
        tabPanel_->addTab( "Editor", Colours::transparentBlack, editorPanel_, false, kEditorPanel );
        tabPanel_->addTab( "Browser", Colours::transparentBlack, browserPanel_, false, kBrowserPanel );
        tabPanel_->addTab( "Setup", Colours::transparentBlack, setupPanel_, false, kSetupPanel );

        addAndMakeVisible( tabPanel_ );
        tabPanel_->setCurrentTabIndex( kEditorPanel );

        monitor_ = new MonitorComponent();
        addAndMakeVisible( monitor_ );

        if (processor_->isPlugin())
        {
            resizer_ = new ResizableCornerComponent( this, &resizeLimits_ );
            addAndMakeVisible( resizer_ );
            Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
            resizeLimits_.setSizeLimits( 640, 480, r.getWidth(), r.getHeight() );
        }
    }


    void AudioEditor::onOpenBank()
    {
        // TODO: store current ModulePanel
        FileChooser fc( "Open Bank",
            File::getCurrentWorkingDirectory(),
            "*.e3mb",
            true );

        if (fc.browseForFileToOpen())
        {
            std::string path = fc.getResult().getFullPathName().toStdString();
            processor_->loadBank( path );
            browserPanel_->updateContents( processor_->getBankXml() );
            processor_->loadInstrument();
            modulePanel_->createModules( processor_, browserPanel_->getSelectedInstrumentXml() );
        }
    }


    void AudioEditor::onSaveBank( bool askForFilename )
    {
        if (askForFilename)
        {
            FileChooser fc( "Save Bank As",
                File::getCurrentWorkingDirectory(),
                "*.e3mb",
                true );

            if (fc.browseForFileToSave( true ))
            {
                File file = fc.getResult();
                processor_->saveBank( file.getFullPathName().toStdString() );
            }
        }
        else {
            processor_->saveBank();
        }
    }


    void AudioEditor::onNewBank()
    {
        processor_->newBank();
        browserPanel_->updateContents( processor_->getBankXml() );
        processor_->loadInstrument();
    }


    void AudioEditor::onLoadInstrument()
    {
        XmlElement* xml = browserPanel_->getSelectedInstrumentXml();
        if (xml != nullptr)
        {
            int id = xml->getIntAttribute( "id" );
            processor_->loadInstrument( id );
            modulePanel_->createModules( processor_, xml );
        }
    }


    void AudioEditor::parameterPanelAttributesChanged( const std::string& attributeName, var value )
    {
        TRACE( "AudioEditor::parameterPanelAttributesChanged: name=%s, value=%s\n",
            attributeName.c_str(), value.toString().toRawUTF8() );

        processor_->setInstrumentAttributes( attributeName, value );
        browserPanel_->updateContents( processor_->getBankXml() );  // TODO: update only active row
    }


    void AudioEditor::browserPanelAttributesChanged( int instrumentId, const std::string& attributeName, var value )
    {
        TRACE( "AudioEditor::parameterPanelAttributesChanged: name=%s, value=%s\n",
            attributeName.c_str(), value.toString().toRawUTF8() );

        processor_->setInstrumentAttribute( instrumentId, attributeName, value );
        parameterPanel_->showInstrument(processor_->getInstrument());
    }

} // namespace e3