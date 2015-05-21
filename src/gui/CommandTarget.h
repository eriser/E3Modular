
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
            cmdShowEditor       = 0x2100,
            cmdShowBrowser      = 0x2101,
            cmdShowSetup        = 0x2102,
            cmdOpenInstrument   = 0x2110,
            cmdSaveInstrument   = 0x2111,
            cmdSaveInstrumentAs = 0x2112,
            cmdNewInstrument    = 0x2113,
        };

        ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
        void getAllCommands(Array<CommandID>& commands) override;
        void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;

        static ApplicationCommandManager* getCommandManager();

    private:
        static ScopedPointer<ApplicationCommandManager> commandManager_;
    };

}  // namespace e3