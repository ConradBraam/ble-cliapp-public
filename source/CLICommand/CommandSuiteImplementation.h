#ifndef BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_
#define BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_

#include "util/ConstArray.h"
#include "CommandArgs.h"
#include "Command.h"
#include "CommandAccessor.h"

/**
 * @brief Implementation of command suite. This is used to reduce template instantiations.
 * It is not meant to be used directly.
 */
struct CommandSuiteImplementation {
    static int commandHandler(
        int argc, char** argv,
        const ConstArray<const Command*>& builtinCommands,
        const ConstArray<const Command*>& moduleCommands
    );

    /**
     * @brief builtin help command implementation
     */
    static void help(
        const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response,
        const ConstArray<const Command*>& builtinCommands,
        const ConstArray<const Command*>& moduleCommands
    );

    /**
     * @brief builtin list command implementation
     */
    static void list(
        const CommandArgs&, const mbed::util::SharedPointer<CommandResponse>& response,
        const ConstArray<const Command*>& builtinCommands,
        const ConstArray<const Command*>& moduleCommands
    );
};


#endif /* BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_ */
