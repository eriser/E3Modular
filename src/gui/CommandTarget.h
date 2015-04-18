
#pragma once
#include "JuceHeader.h"

namespace e3 {
    class AudioEditor;

    
    class CommandTarget : public ApplicationCommandTarget
    {
    public:
        ~CommandTarget() override;

        enum CommandIDs
        {
            cmdShowEditor     = 0x2100,
            cmdShowBrowser    = 0x2101,
            cmdShowSetup      = 0x2102,
            cmdOpen           = 0x2110,
            cmdSave           = 0x2111,
            cmdSaveAs         = 0x2112,
            cmdNew            = 0x2113,
            cmdLoadInstrument = 0x2120,
        };

        ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
        void getAllCommands(Array<CommandID>& commands) override;
        void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;

        static ApplicationCommandManager* getCommandManager();

    private:
        static ScopedPointer<ApplicationCommandManager> commandManager_;
    };

}  // namespace e3