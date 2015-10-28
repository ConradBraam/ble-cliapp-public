#ifndef BLE_CLIAPP_COMMAND_H_
#define BLE_CLIAPP_COMMAND_H_

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
 */
struct Command {
	constexpr Command(const char* _name, const CommandHandler_t _handler) : 
		name(_name), help(NULL), argsDescription(), handler(_handler) { }

	constexpr Command(const char* _name, const char* _help, const CommandHandler_t _handler) : 
		name(_name), help(_help), argsDescription(), handler(_handler) { }

	constexpr Command(const char* _name, const char* _help, ConstArray<CommandArgDescription> _argsDescription, const CommandHandler_t _handler) : 
		name(_name), help(_help), argsDescription(_argsDescription), handler(_handler) { }

	const char* name;                                            /// name of this command
	const char* help;                                            /// help of this command
	const ConstArray<CommandArgDescription> argsDescription;     /// description of each args of the command      
	const CommandHandler_t handler;                              /// the command handler associated 

};

#endif //BLE_CLIAPP_COMMAND_H_