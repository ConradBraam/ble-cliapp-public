#ifndef BLE_CLIAPP_CLICOMMAND_DETAIL_LISTCOMMANDBASE_H_
#define BLE_CLIAPP_CLICOMMAND_DETAIL_LISTCOMMANDBASE_H_

#include "../Command.h"

/**
 * @brief Abstract base implementation for the Help Command. 
 * @details Avoid the redeclaration of the name and help functions for 
 * every instantiated class of the HelpCommand.
 */
struct ListCommandBase : public BaseCommand {
    static const char* name() {
        return "list";
    }

    static const char* help() {
        return "list all the command in a module";
    }
};

#endif //BLE_CLIAPP_CLICOMMAND_DETAIL_LISTCOMMANDBASE_H_



