#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include <stdint.h>

#include "util/ConstArray.h"
#include "util/StaticLambda.h"
#include "CommandArgs.h"
#include "Command.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "mbed-client-cli/ns_cmdline.h"

#include <iterator>

#include "dynamic/Value.h"
#include "util/StaticString.h"
#include <minar/minar.h>


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
 * static CommandResult doFoo(const CommandArgs& args) {
 *     cmd_printf("foo");
 *     return CMDLINE_RETCODE_SUCCESS;
 * }
 *
 * static CommandResult doBar(const CommandArgs& args) {
 *     cmd_printf("bar");
 *     return CMDLINE_RETCODE_SUCCESS;
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

    /**
     * @brief  Entry point for the command handler of the suite.
     * @details This function demultiplex command and args from CLI and execute the right comamnd.
     * It also collect results and format message result.
     *
     * @return a command status code as described in mbed-client-cli/ns_cmdline.h.
     */
    static int commandHandler(int argc, char** argv) {
        const CommandArgs args(argc, argv);
        const char* commandName = args[1];
        const CommandArgs commandArgs(args.drop(2));

        return executeCommandHandler(commandName, commandArgs);
    }

    /**
     * execute a command handler, return the status code
     */
    static int executeCommandHandler(const char* commandName, const CommandArgs commandArgs) {
        std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>();

        response->setCommandName(commandName);
        response->setArguments(commandArgs);

        const Command* command = CommandSuite::getCommand(commandName);

        if(command) {
            // check arguments
            if(commandArgs.count() < command->argsDescription.count()) {
                response->invalidParameters("not enough arguments");
                return response->getStatusCode();
            }

            if(commandArgs.count() > command->maximumArgsRequired) {
                response->invalidParameters("too many arguments");
                return response->getStatusCode();
            }

            // execute the handler
            command->handler(commandArgs, response);

            // if response is not referenced elsewhere, this means that the execution is done,
            // just return the status code set
            // otherwise, tell the system that the execution continue and install continuation
            // callback
            if(response.unique()) {
                return response->getStatusCode();
            } else {
                response->setOnClose(whenAsyncCommandEnd);
                return CMDLINE_RETCODE_EXCUTING_CONTINUE;
            }

        } else {
            response->faillure("invalid command name, you can get all the command name for this module by using the command 'list'");
            return response->getStatusCode();
        }
    }

    static void whenAsyncCommandEnd(const CommandResponse* response) {
        minar::Scheduler::postCallback(mbed::util::FunctionPointer1<void, int>(cmd_ready).bind(response->getStatusCode()));
    }

    // builtin commands
    static const Command* builtinCommands[3];
    static const Command help;
    static const Command list;
    static const Command args;

    static const Command* getCommand(const char* name) {
        // builtin commands
        for(size_t i = 0; i < sizeof(CommandSuite::builtinCommands) / sizeof(CommandSuite::builtinCommands[0]); ++i) {
            if(strcmp(name, CommandSuite::builtinCommands[i]->name) == 0) {
                return CommandSuite::builtinCommands[i];
            }
        }

        const ConstArray<Command> commands = SuiteDescription::commands();
        for(size_t i = 0; i < commands.count(); ++i) {
            if(strcmp(name, commands[i].name) == 0) {
                return &commands[i];
            }
        }

        return nullptr;
    }
};

template<typename SuiteDescription>
const Command* CommandSuite<SuiteDescription>::builtinCommands[3] = {
    &CommandSuite::help,
    &CommandSuite::list,
    &CommandSuite::args
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::help {
    "help",
    "Print help about a command, you can list the command by using the command 'list'",
    (const CommandArgDescription[]) {
        { "<commandName>", "the name of a command you want help for, use the command 'list' to have a list of available commands" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
        const Command* command = CommandSuite::getCommand(args[0]);
        if(!command) {
            response->invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
        } else {
            response->success(command->help);
        }
    }
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::list {
    "list",
    "list all the command in a module",
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        using namespace serialization;

        response->setStatusCode(CommandResponse::SUCCESS);

        auto& os = response->getResultStream();

        os << startArray;
        // builtin commands
        for(size_t i = 0; i < sizeof(CommandSuite::builtinCommands) / sizeof(CommandSuite::builtinCommands[0]); ++i) {
            os << CommandSuite::builtinCommands[i]->name;
        }

        const ConstArray<Command> commands = SuiteDescription::commands();
        for(size_t i = 0; i < commands.count(); ++i) {
            os << commands[i].name;
        }

        os << endArray;
    }
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::args {
    "args",
    "print the args of a command",
    (const CommandArgDescription[]) {
        { "commandName", "The name of the the command you want the args" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
        using namespace serialization;

        const Command* command = CommandSuite::getCommand(args[0]);
        if(!command) {
            response->invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
            return;
        }

        auto& os = response->getResultStream();

        os << startArray;
        for(size_t i = 0; i < command->argsDescription.count(); ++i) {
            os << startObject << key(command->argsDescription[i].name) << command->argsDescription[i].desc << endObject;
        }
        os << endArray;
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
