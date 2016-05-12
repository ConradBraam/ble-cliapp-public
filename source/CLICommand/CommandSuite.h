#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include "util/ConstArray.h"
#include "CommandArgs.h"
#include "Command.h"
#include "CommandSuiteImplementation.h"
#include "CommandAccessor.h"

/**
 * @brief Allow to easily group and add a suite of commands into the cli system.
 * @details This class is parametized by a description which should provide information to run.
 *
 * @tparam SuiteDescription The class describing the command suite. The class should
 * provide the following static methods :
 *    - static const char* name() const : The name of the suite. Each commands present in the suite
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
 * static const char* name() const {
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
 * static ConstArray<CommandAccessor_t> commands() {
 *  	static const CommandAccessor_t commandHandlers[] = {
 *      	&getCommand<FooCommand>,
 *          &getCommand<BarCommand>
 *      };
 *
 *      return ConstArray<CommandAccessor_t>(commandHandlers);
 * }
 *
 * };
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

    static ConstArray<CommandAccessor_t> getModuleCommands() {
        return SuiteDescription::commands();
    }

    static ConstArray<CommandAccessor_t> getBuiltinCommands() {
        static const CommandAccessor_t builtinCommands[] = {
            getCommand<HelpCommand>,
            getCommand<ListCommand>,
            getCommand<ArgsCommand>
        };
        return ConstArray<CommandAccessor_t>(builtinCommands);
    }


    struct HelpCommand : public Command {
        virtual const char* name() const {
            return "help";
        }

        virtual const char* help() const {
            return "Print help about a command, you can list the command by using the command 'list'";
        }

        virtual ConstArray<CommandArgDescription> argsDescription() const {
            static const CommandArgDescription argsDescription[] = {
                {
                    "<commandName>",
                    "the name of a command you want help for, use the command 'list' to have a list of available commands"
                }
            };
            return ConstArray<CommandArgDescription>(argsDescription);
        }

        virtual void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response) const {
            CommandSuiteImplementation::help(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    };


    struct ListCommand : public Command {
        virtual const char* name() const {
            return "list";
        }

        virtual const char* help() const {
            return "list all the command in a module";
        }

        virtual void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response) const {
            CommandSuiteImplementation::list(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    };


    struct ArgsCommand : public Command {
        virtual const char* name() const {
            return "args";
        }

        virtual const char* help() const {
            return "print the args of a command";
        }

        virtual ConstArray<CommandArgDescription> argsDescription() const {
            static const CommandArgDescription argsDescription[] = {
                { "commandName", "The name of the the command you want the args" }
            };
            return ConstArray<CommandArgDescription>(argsDescription);
        }

        virtual void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response) const {
            CommandSuiteImplementation::args(
                args,
                response,
                getBuiltinCommands(),
                getModuleCommands()
            );
        }
    };
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
