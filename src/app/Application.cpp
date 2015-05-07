
#include "core/Settings.h"
#include "core/Processor.h"
#include "gui/AudioEditor.h"
#include "gui/Style.h"
#include "app/AppWindow.h"
#include "app/AudioEngine.h"
#include "app/Application.h"


namespace e3 {

    Application::Application() {}
    Application::~Application() {}


    void Application::initialise( const String& )
    {
        audioEngine_ = new AudioEngine();
        AudioEditor* editor = dynamic_cast<AudioEditor*>(audioEngine_->getProcessor()->createEditorIfNeeded());

        window_ = new AppWindow( ProjectInfo::displayName );
        window_->setTitleBarButtonsRequired( DocumentWindow::allButtons, false );
        window_->setResizable( true, true );

        window_->setContentOwned( editor, true );

        Settings& settings = Settings::getInstance();
        window_->restoreWindowStateFromString( settings.getWindowState( "Standalone" ) );
        window_->setLookAndFeel( &Style::getInstance() );
        window_->setUsingNativeTitleBar( false );
        window_->setVisible( true );

        Image icon;
        editor->createIcon( icon );
        window_->setIcon( icon );
    }


    void Application::shutdown()
    {
        audioEngine_->stopPlaying();

        if (AudioProcessorEditor* editor = dynamic_cast<AudioProcessorEditor*> (window_->getContentComponent()))
        {
            Settings::getInstance().setWindowState( window_->getWindowStateAsString().toStdString(), "Standalone" );
			audioEngine_->storeAudioSettings();
            audioEngine_->getProcessor()->editorBeingDeleted( editor );
            window_->clearContentComponent();
        }
        window_      = nullptr;
        audioEngine_ = nullptr;
    }


    AudioEngine* Application::getAudioEngine()  const         { return audioEngine_; }
    AudioDeviceManager* Application::getDeviceManager() const { return audioEngine_->getDeviceManager(); }
    AudioProcessor* Application::getProcessor() const         { return audioEngine_->getProcessor(); }

    Application* Application::getInstance()    { return dynamic_cast<Application*>(JUCEApplication::getInstance()); }

    // This macro generates the main() routine that launches the app.
    START_JUCE_APPLICATION( Application )


        AudioProcessor* JUCE_CALLTYPE createPluginFilter()
    {
        return new e3::Processor();
    }
} // namespace e3