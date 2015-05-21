
#include <e3_Trace.h>

#include "core/GlobalHeader.h"
#include "core/Settings.h"
#include "core/Processor.h"
#include "core/Polyphony.h"
#include "core/Database.h"
#include "gui/Style.h"
#include "gui/Resources.h"
#include "gui/EditorPanel.h"
#include "gui/ModulePanel.h"
#include "gui/ParameterPanel.h"
#include "gui/BrowserPanel.h"
#include "gui/SetupPanel.h"
#include "gui/InstrumentBrowser.h"
#include "gui/DatabaseBrowser.h"
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

        connectSignals();
        
        //browserPanel_->updateContents( processor_->getDatabaseXml() );
        Database::getInstance().build();
        databaseBrowser_->update();
        modulePanel_->createModules( processor_ );
    }


    AudioEditor::~AudioEditor()
    {
        disconnectSignals();
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
        Settings::getInstance().setWindowState( bounds, "plugin" );
    }


    bool AudioEditor::perform( const InvocationInfo& info )
    {
        switch (info.commandID)
        {
        case cmdShowEditor:       tabPanel_->setCurrentTabIndex( kEditorPanel ); break;
        case cmdShowBrowser:      tabPanel_->setCurrentTabIndex( kBrowserPanel );  break;
        case cmdShowSetup:        tabPanel_->setCurrentTabIndex( kSetupPanel ); break;
        case cmdOpenInstrument:   onOpenInstrument(); break;
        case cmdSaveInstrument:   onSaveInstrument(); break;
        case cmdSaveInstrumentAs: onSaveInstrumentAs(); break;
        case cmdNewInstrument:    onNewInstrument(); break;

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
        std::string context     = processor_->isPlugin() ? "plugin" : "standalone";
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
        parameterPanel_    = new ParameterPanel( processor_ );
        editorPanel_       = new EditorPanel();
        browserPanel_      = new BrowserPanel();
        setupPanel_        = new SetupPanel();
        instrumentBrowser_ = new InstrumentBrowser();
        databaseBrowser_   = new DatabaseBrowser();

        editorPanel_->setComponents( modulePanel_, parameterPanel_ );
        browserPanel_->setComponents( instrumentBrowser_, databaseBrowser_ );

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


    void AudioEditor::connectSignals()
    {
        processor_->getPolyphony()->monitorUpdateSignal.Connect( monitor_.get(), &MonitorComponent::monitor );

        modulePanel_->showInstrumentSignal.Connect( parameterPanel_.get(), &ParameterPanel::showInstrument );
        modulePanel_->showModuleSignal.Connect( parameterPanel_.get(), &ParameterPanel::showModule );
    }


    void AudioEditor::disconnectSignals()
    {
        modulePanel_->showInstrumentSignal.Disconnect( parameterPanel_.get(), &ParameterPanel::showInstrument );
        modulePanel_->showModuleSignal.Disconnect( parameterPanel_.get(), &ParameterPanel::showModule );

        processor_->getPolyphony()->monitorUpdateSignal.Disconnect( monitor_.get(), &MonitorComponent::monitor );
    }

    
    void AudioEditor::onOpenInstrument()
    {
        FileChooser fc( "Open Instrument",
            File::getCurrentWorkingDirectory(),
            "*.e3mi",
            true );

        if (fc.browseForFileToOpen())
        {
            std::string path = fc.getResult().getFullPathName().toStdString();
            processor_->loadInstrument( path );
            modulePanel_->createModules( processor_ );
        }
    }


    void AudioEditor::onSaveInstrument()
    {
        processor_->saveInstrument();
    }


    void AudioEditor::onSaveInstrumentAs()
    {
        FileChooser fc( "Save Instrument As",
            File::getCurrentWorkingDirectory(),
            "*.e3mi",
            true );

        if (fc.browseForFileToSave( true ))
        {
            File file = fc.getResult();
            processor_->saveInstrument( file.getFullPathName().toStdString() );
        }
    }


    void AudioEditor::onNewInstrument()
    {
        processor_->loadInstrument();
        modulePanel_->createModules( processor_ );
    }

} // namespace e3