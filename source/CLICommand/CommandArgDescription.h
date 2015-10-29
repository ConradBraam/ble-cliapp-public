#ifndef BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_
#define BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_

/**
 * @brief simple description of an argument of a command
 */
struct CommandArgDescription { 
	const char* name;                                /// The name of the argument
	const char* desc;                                /// The description of the argument
};

#endif //BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_