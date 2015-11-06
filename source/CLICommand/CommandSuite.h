#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include <stdint.h>

#include "util/ConstArray.h"
#include "util/StaticLambda.h"
#include "CommandArgs.h"
#include "CommandResult.h"
#include "Command.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "mbed-client-cli/ns_cmdline.h"

#include <iterator>

#include "dynamic/Value.h"
#include "util/StaticString.h"

struct CmdPrintfOutputIterator : public std::iterator<std::output_iterator_tag,void,void,void,void> { 
    CmdPrintfOutputIterator& operator=(const char value) {
        cmd_printf("%c", value);
        return *this;
    }

    CmdPrintfOutputIterator& operator*() {
        return *this;
    }

    CmdPrintfOutputIterator& operator++() {
        return *this;
    }

    CmdPrintfOutputIterator& operator++(int) {
        return *this;
    }
};


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

        CommandResult result = executeCommandHandler(commandName, commandArgs);
        if(result.statusCode != CMDLINE_RETCODE_EXCUTING_CONTINUE) {
            printCommandResult(SuiteDescription::name(), commandName, commandArgs, result);         
        }
        return result.statusCode;
    }

    /**
     * execute a command handler
     */
    static CommandResult executeCommandHandler(const char* commandName, const CommandArgs commandArgs) {
        const Command* command = CommandSuite::getCommand(commandName);
        if(command) {
            // check arguments 
            if(commandArgs.count() < command->argsDescription.count()) {
                return CommandResult::invalidParameters("not enough arguments"); 
            }

            if(commandArgs.count() > command->maximumArgsRequired) {
                return CommandResult::invalidParameters("too many arguments"); 
            }
            return command->handler(commandArgs);
        } else {
            return CommandResult::faillure("invalid command name, you can get all the command name for this module by using the command 'list'");
        }
    }

    /**
     * @brief Format and print the command result through the command line
     * @details The result printed is a JSON describing the command and its results. 
     * The JSON format is as follow : 
     * {
     *     "command_name": "name of the command", 
     *     "arguments": [
     *         "arg0",
     *         "arg1",
     *         ...
     *     ],
     *     "status": 0, // The status of the command as described in mbed-client-cli/ns_cmdline.h. 
     *     "result": <some json value>, // this field is only present if the status is equal to zero
     *     "error": <some json value>  // this field is only present if the status is NOT equal to zero
     * }
     * 
     * @param commandGroup The group name of the command executed 
     * @param commandName The name of the command executed 
     * @param args Arguments of the command
     * @param result The result of the command execution
     */
    static void printCommandResult(const char* commandGroup, const char* commandName, const CommandArgs& args, const CommandResult& result) {
        using dynamic::Value;
        using container::DynamicString;

        Value message;

        // build the name 
        std::string name((std::string(commandGroup) + " " + commandName)); 
        message["name"_ss] = name.c_str();

        // build the arguments 
        message["arguments"_ss] = dynamic::Value::Array_t { };
        for(size_t i = 0; i < args.count(); ++i) {
            message["arguments"_ss].push_back(DynamicString(args[i]));
        }

        // set the command status
        message["status"_ss] = (int64_t) result.statusCode;

        // add additional informations if there is any
        if(result.info != nullptr) {
            // in case of a status code equal to 0, it is the result of the command
            if(result.statusCode == (int64_t) 0) {
                message["result"_ss] = result.info;
            } else {
                // otherwise it is the error reason
                message["error"_ss] = result.info;
            }
        }

        CmdPrintfOutputIterator os;
        message.serialize(os, true);
    }

    static void commandReady(const char* commandName, const CommandArgs& args, const CommandResult& result) {
        printCommandResult(SuiteDescription::name(), commandName, args, result);
        cmd_ready(result.statusCode);
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
    STATIC_LAMBDA(const CommandArgs& args) {
        const Command* command = CommandSuite::getCommand(args[0]);
        if(!command) {
            return CommandResult::invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
        }

        return CommandResult::success(command->help);
    }
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::list {
    "list",
    "list all the command in a module",
    STATIC_LAMBDA(const CommandArgs&) {
        dynamic::Value res;

        // builtin commands
        for(size_t i = 0; i < sizeof(CommandSuite::builtinCommands) / sizeof(CommandSuite::builtinCommands[0]); ++i) {
            res.push_back(container::StaticString(CommandSuite::builtinCommands[i]->name));
        }

        const ConstArray<Command> commands = SuiteDescription::commands();
        for(size_t i = 0; i < commands.count(); ++i) {
            res.push_back(container::StaticString(commands[i].name));
        }

        return CommandResult::success(res);
    }
};

template<typename SuiteDescription>
const Command CommandSuite<SuiteDescription>::args {
    "args",
    "print the args of a command",
    (const CommandArgDescription[]) {
        { "commandName", "The name of the the command you want the args" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        using container::StaticString;
        const Command* command = CommandSuite::getCommand(args[0]);
        if(!command) {
            return CommandResult::invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
        }

        dynamic::Value res;
        for(size_t i = 0; i < command->argsDescription.count(); ++i) {
            res.push_back(dynamic::Value {} 
                [container::DynamicString(command->argsDescription[i].name)] = StaticString(command->argsDescription[i].desc)
            );  
        }

        return CommandResult::success(res);
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