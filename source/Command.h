#ifndef BLE_CLIAPP_COMMAND_H_
#define BLE_CLIAPP_COMMAND_H_

#include "CommandResult.h"
#include "CommandArgs.h"

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
struct Command_t {
	const char* name;                                      /// name of this command
	const CommandHandler_t handler;                        /// the command handler associated 
};

#endif //BLE_CLIAPP_COMMAND_H_