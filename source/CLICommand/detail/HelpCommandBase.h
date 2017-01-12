#ifndef BLE_CLIAPP_CLICOMMAND_DETAIL_HELPCOMMAND_BASE_H_
#define BLE_CLIAPP_CLICOMMAND_DETAIL_HELPCOMMAND_BASE_H_

#include "../Command.h"

/**
 * @brief Abstract base implementation for the Help Command. 
 * @details Avoid the redeclaration of the name, help and argsDescription functions for 
 * every instantiated class of the HelpCommand.
 */
struct HelpCommandBase : public BaseCommand {
    static const char* name() {
        return "help";
    }

    static const char* help() {
        return "Print help about a command, you can list the command by using the command 'list'";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            {
                "string",
                "<commandName>",
                "the name of a command you want help for, use the command 'list' to have a list of available commands"
            }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }
};

#endif //BLE_CLIAPP_CLICOMMAND_DETAIL_HELPCOMMAND_BASE_H_



