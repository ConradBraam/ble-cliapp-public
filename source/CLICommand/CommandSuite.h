#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include "util/ConstArray.h"
#include "util/StaticLambda.h"
#include "CommandArgs.h"
#include "Command.h"
#include "CommandSuiteImplementation.h"

/**
 * @brief Allow to easily group and add a suite of commands into the cli system.
 * @details This class is parametized by a description which should provide information to run.
 *
 * @tparam SuiteDescription The class describing the command suite. The class should
 * provide the following static methods :
 *    - static const char* name() : The name of the suite. Each commands present in the suite
 *    will be available through this entry point.
 *    - static const char* info() : Informations about this command suite
 *    - static const char* man() : The manual of this command suite
 *    - static ConstArray<Command> commands() : The array of commands presents in the suite
 *
 * \code
 *
 * class DummyCommandSuite {
 *
 * public:
 *
 * static const char* name() {
 *    return "dummy";
 * }
 *
 * static const char* info() {
 *     return "dummy info"
 * }
 *
 * static const char* man() {
 *     return "dummy <command> <command arguments>\r\n"\
 *            "    * dummy foo : print foo\r\n"
 *            "    * dummy bar : print bar\r\n";
 * }
 *
 * static ConstArray<Command> commands() {
 *    static const Command commandHandlers[] = {
 *        { "foo", doFoo },
 *        { "bar", doBar }
 *    }
 *
 *    return ConstArray<Command>(commandHandlers);
 * }
 *
 *
 * private:
 *
 * static void doFoo(const CommandArgs& args, const std::shared_ptr<CommandResponse>& resp) {
 *     response->success("foo");
 * }
 *
 * static CommandResult doBar(const CommandArgs& args, const std::shared_ptr<CommandResponse>& resp) {
 *     response->success("bar")
 * }
 *
 * }
 *
 * \endcode
 *
 */
template<typename SuiteDescription>
class CommandSuite {

public:
    /**
     * @brief Register this command suite into the cli system
     */
    static void registerSuite() {
        cmd_add(
            SuiteDescription::name(),
            commandHandler,
            SuiteDescription::info(),
            SuiteDescription::man()
        );
    }

private:
    /**
     * @brief  Entry point for the command handler of the suite.
     * @details This function demultiplex command and args from CLI and execute the right comamnd.
     * It also collect results and format message result.
     *
     * @return a command status code as described in mbed-client-cli/ns_cmdline.h.
     */
    static int commandHandler(int argc, char** argv) {
        return CommandSuiteImplementation::commandHandler(
            argc,
            argv,
            getBuiltinCommands(),
            getModuleCommands()
        );
    }

    static ConstArray<Command> getModuleCommands() {
        return SuiteDescription::commands();
    }

    static ConstArray<Command> getBuiltinCommands() {
        return ConstArray<Command>(builtinCommands);
    }

    // builtin commands
    static const Command builtinCommands[3];
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::builtinCommands[3] = {
    Command {
        "help",
        "Print help about a command, you can list the command by using the command 'list'",
        (const CommandArgDescription[]) {
            { "<commandName>", "the name of a command you want help for, use the command 'list' to have a list of available commands" }
        },
        STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
            CommandSuiteImplementation::help(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    },
    Command {
        "list",
        "list all the command in a module",
        STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
            CommandSuiteImplementation::list(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    },
    Command {
        "args",
        "print the args of a command",
        (const CommandArgDescription[]) {
            { "commandName", "The name of the the command you want the args" }
        },
        STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
            CommandSuiteImplementation::args(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    }
};

/**
 * @brief Register a command suite into the system
 *
 * @tparam CommandSuiteDescription The command suite to register
 */
template<typename CommandSuiteDescription>
void registerCommandSuite() {
    CommandSuite<CommandSuiteDescription>::registerSuite();
}

#endif //BLE_CLIAPP_COMMAND_SUITE_H_
