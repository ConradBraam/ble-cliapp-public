#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_H_

#include <core-util/SharedPointer.h>
#include "CommandResponse.h"
#include "CommandArgs.h"
#include "CommandArgDescription.h"
#include "mbed_error.h"

struct Command {
    const char* (* const name )();
#if defined(ENABLE_COMMAND_HELP)
    const char* (* const help)();
#endif
    ConstArray<CommandArgDescription> (* const argsDescription)();
    std::size_t (* const maximumArgsRequired)();

    /**
     * A command handler is a function which handle commands from the command
     * line. It receive args in input and a response object it has to fullfill.
     * \param args : array of arguments of the function. It does not contain the
     * command name at first argument.
     * \param res : response to the command. It is expected that the handler set the
     * status code then provide a result for the command. Command will be finmished
     * once the CommandResponse pointer will expire. This means that as long as the
     * response exist in the system others command will be rejected.
     * It is not expected that the user will close manually the response everytime,
     * please just let the destructor handle this task.
     */
    void (* const handler)(
        const CommandArgs& args,
        const mbed::util::SharedPointer<CommandResponse>& res
    );
};


/**
 * @brief Simple base class describing a command
 * @details This object describe what a command is. It as to be included into a CommandSuite
 * to be usable. The CommandSuite owning this Command will use the attributes as follow:
 *   - name: The name used to refer to this command in the CommandHandler suite. The user
 *           will be able to access to this command through <commandSuiteName> <nameOfTheCommand>.
 *   - help: The help displayed to the user if it requires it. with the command <commandSuiteName> help <nameOfTheCommand>
 *   - args: The description of the arguments used by this command <commandSuiteName> args <nameOfTheCommand>.
 *           If the command is invoked is invoked with less arguments than the number of arguments present in
 *           the description, the command will failled.
 *   - maximumArgs: The maximum number of arguments that this command can handle.
 *   - handler: The handler of this command.
 */
struct BaseCommand {
    /**
     * name of the command, this function has to be overiden
     * @return The name of the command
     */
    static const char* name() { 
        error("A command should have a name!");
        return NULL;
    }

    /**
     * Help associated with this command
     * @return the help of the command
     */
    static const char* help() {
        return "";
    }

    /**
     * Description of each args of the command, if not overriden, this will
     * return an empty array of args.
     */
    static ConstArray<CommandArgDescription> argsDescription() {
        return ConstArray<CommandArgDescription>();
    }

    /**
     * The maximum number of arguments allowed by this command, by default it
     * is equal to the count of arguments returned by argsDescription.
     * @return the maximum number of arguments allowed
     */
    template<typename T>
    static std::size_t maximumArgsRequired() {
        return T::argsDescription().count();
    }

    /**
     * Handler of the command, this function has to be overriden.
     * @param args arguments of the command
     * @param res response of the command
     */
    static void handler(
        const CommandArgs& args,
        const mbed::util::SharedPointer<CommandResponse>& res
    ) { 
        error("A command handler should be implemented");
    }
};

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_H_
