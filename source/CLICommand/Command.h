#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_H_

#include "CommandResult.h"
#include "CommandArgs.h"
#include "CommandArgDescription.h"

/**
 * A command handler is a function which handle commands from the command 
 * line. It receive args in input and return a code indicating the state 
 * of the command. The result of the command can be stored in the value 
 * provided in parameters:
 * \param args : array of arguments of the function. It does not contain the 
 * command name at first argument.
 * \return a command result which contains the status code and additional infos
 */
typedef CommandResult (*CommandHandler_t)(const CommandArgs& args);


/**
 * @brief Simple POD describing a command
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
struct Command {
	/**
	 * @brief Initialize a Command
	 * @details In this form, the command accept any number of arguments and no help is provided 
	 * 
	 * @param _name the name of the command 
	 * @param _handler the handler of the command
	 */
	constexpr Command(const char* _name, const CommandHandler_t _handler) : 
		name(_name), help(""), argsDescription(), maximumArgsRequired(0xFF), handler(_handler) { }

	/**
	 * @brief Initialize a Command
	 * @details In this form, the command accept any number of arguments 
	 * 
	 * @param _name the name of the command 
	 * @param _help help of this command
	 * @param _handler the handler of the command
	 */
	constexpr Command(const char* _name, const char* _help, const CommandHandler_t _handler) : 
		name(_name), help(_help), argsDescription(), maximumArgsRequired(0xFF), handler(_handler) { }

	/**
	 * @brief Initialize a Command
	 * @details In this form, the command expect a specific number of arguments as described by the 
	 * argsDescription parameter. If the caller does not provide the same number of arguments as the 
	 * ones provided in the argsDescription parameter, the handler will not be called 
	 * 
	 * @param _name the name of the command 
	 * @param _help help of this command
	 * @param _argsDescription the description of the args accepted by this command
	 * @param _handler the handler of the command
	 */
	constexpr Command(const char* _name, const char* _help, ConstArray<CommandArgDescription> _argsDescription, const CommandHandler_t _handler) : 
		name(_name), help(_help), argsDescription(_argsDescription), maximumArgsRequired(_argsDescription.count()), handler(_handler) { }


	/**
	 * @brief Initialize a Command
	 * @details In this form, the command expect a minimum and maximum number of arguments as described by the 
	 * argsDescription parameter and maximumArgsRequired parameter. 
	 * If the caller does not provide the at least as many number of arguments as the ones provided in the 
	 * argsDescription parameter, the handler will not be called. If the caller provide more arguments than the 
	 * number specified by _maximumArgsRequired, the handler will not be called. 
	 * 
	 * @param _name the name of the command 
	 * @param _help help of this command
	 * @param _argsDescription the description of the args accepted by this command
	 * @param _maximumArgsRequired the maximum number of arguments that this command can take
	 * @param _handler the handler of the command
	 */
	constexpr Command(const char* _name, const char* _help, ConstArray<CommandArgDescription> _argsDescription, uint8_t _maximumArgsRequired, const CommandHandler_t _handler) : 
		name(_name), help(_help), argsDescription(_argsDescription), maximumArgsRequired(_maximumArgsRequired), handler(_handler) { }


	const char* name;                                            /// name of this command
	const char* help;                                            /// help of this command
	const ConstArray<CommandArgDescription> argsDescription;     /// description of each args of the command
	const uint8_t maximumArgsRequired;                           /// The maximum args count required 
	const CommandHandler_t handler;                              /// the command handler associated 
};

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_H_