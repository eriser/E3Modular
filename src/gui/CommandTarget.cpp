
#include <e3_Trace.h>

#include "gui/AudioEditor.h"
#include "gui/CommandTarget.h"

namespace e3 {

    void CommandTarget::getAllCommands(Array<CommandID>& commands)
    {
        // this returns the set of all commands that this target can perform..
        const CommandID ids[] = { 
            showEditor,
            showBrowser,
            showAudioSetup
        };
        commands.addArray(ids, numElementsInArray(ids));
    }


    void CommandTarget::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
    {
        switch (commandID)
        {
        case showEditor:
            result.setInfo("Instrument Editor", "Display the instrument editor", "General", 0);
            result.addDefaultKeypress('e', ModifierKeys::commandModifier);
            break;

        case showBrowser:
            result.setInfo("Sound Browser", "Display the sound browser", "General", 0);
            result.addDefaultKeypress('b', ModifierKeys::commandModifier);
            break;

        case showAudioSetup:
            result.setInfo("Audio Setup", "Display the audio setup panel", "General", 0);
            result.addDefaultKeypress(',', ModifierKeys::commandModifier);
            break;

        default:
            break;
        }
    }

} // namespace e3