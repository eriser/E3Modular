
#include <e3_Trace.h>

#include "gui/AudioEditor.h"
#include "gui/CommandTarget.h"


namespace e3 {

    ScopedPointer<ApplicationCommandManager> CommandTarget::commandManager_;

    CommandTarget::~CommandTarget()
    {
        commandManager_ = nullptr;
    }


    void CommandTarget::getAllCommands(Array<CommandID>& commands)
    {
        // this returns the set of all commands that this target can perform..
        const CommandID ids[] = { 
            cmdShowEditor,
            cmdShowBrowser,
            cmdShowSetup,
            cmdOpen,
            cmdSave,
            cmdSaveAs,
            cmdNew,
            cmdLoadInstrument
        };
        commands.addArray(ids, numElementsInArray(ids));
    }


    void CommandTarget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
    {
        switch (commandID)
        {
        case cmdShowEditor:
            result.setInfo("Instrument Editor", "Display the instrument editor", "View", 0);
            result.addDefaultKeypress('e', ModifierKeys::commandModifier);
            break;
        case cmdShowBrowser:
            result.setInfo("Sound Browser", "Display the sound browser", "View", 0);
            result.addDefaultKeypress('b', ModifierKeys::commandModifier);
            break;
        case cmdShowSetup:
            result.setInfo("Setup", "Display the setup panel", "View", 0);
            result.addDefaultKeypress(',', ModifierKeys::commandModifier);
            break;
        case cmdOpen:
            result.setInfo("Open...", "Opens a bank or instrument", "File", 0);
            result.addDefaultKeypress('o', ModifierKeys::commandModifier);
            break;
        case cmdSave:
            result.setInfo("Save", "Saves a bank or instrument", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case cmdSaveAs:
            result.setInfo("Save As...", "Opens a file choose to save a bank or instrument", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case cmdNew:
            result.setInfo("New", "Creates a new bank or instrument", "File", 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            break;
        case cmdLoadInstrument:
            result.setInfo("Load Instrument", "Loads an Instrument", "File", 0);
            break;
        default:
            break;
        }
    }


    ApplicationCommandManager* CommandTarget::getCommandManager()
    {
        if (commandManager_ == nullptr)
            commandManager_ = new ApplicationCommandManager();

        return commandManager_;
    }



} // namespace e3