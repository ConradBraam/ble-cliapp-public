#ifndef BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_
#define BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_

#include <stdint.h>

#include "util/ConstArray.h"
#include "util/StaticLambda.h"
#include "CommandArgs.h"
#include "Command.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "mbed-client-cli/ns_cmdline.h"

#include <iterator>

#include "CommandSuiteImplementation.h"

#include <minar/minar.h>


struct CommandSuiteImplementation {
    static int commandHandler(int argc, char** argv, const ConstArray<const Command*>& builtinCommands, const ConstArray<Command>& moduleCommands);

    static void whenAsyncCommandEnd(const CommandResponse* response);

    static const Command* getCommand(const char* name, const ConstArray<const Command*>& builtinCommands, const ConstArray<Command>& moduleCommands);

    static void help(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response,
        const ConstArray<const Command*>& builtinCommands, const ConstArray<Command>& moduleCommands);

    static void list(const CommandArgs&, const std::shared_ptr<CommandResponse>& response,
        const ConstArray<const Command*>& builtinCommands, const ConstArray<Command>& moduleCommands);

    static void args(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response,
        const ConstArray<const Command*>& builtinCommands, const ConstArray<Command>& moduleCommands);
};


#endif /* BLE_CLIAPP_COMMAND_SUITE_IMPLEMENTATION_H_ */
