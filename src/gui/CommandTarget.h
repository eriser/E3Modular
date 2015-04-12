
#pragma once
#include "JuceHeader.h"

namespace e3 {
    class AudioEditor;

    
    class CommandTarget : public ApplicationCommandTarget
    {
    public:
        ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
        void getAllCommands(Array<CommandID>& commands) override;
        void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;

        enum CommandIDs
        {
            showEditor     = 0x2100,
            showBrowser    = 0x2101,
            showAudioSetup = 0x2102,
        };
    };

}  // namespace e3