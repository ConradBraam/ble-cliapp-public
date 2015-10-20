#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include <stdint.h>

#include "ConstArray.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "Stringify.h"
#include "picojson.h"

/**
 * typedef for Arguments which are just a constant array of strings
 */
typedef ConstArray<const char*> CommandArgs; 

/**
 * @brief  Simple POD which encapsultate the result of a command.
 * @details The result of a command comes in two parts : 
 *   - A command status code as described in mbed-client-cli/ns_cmdline.h. 
 *   - informations which will contains additional result or error message.
 * As a rule of thumb, the field info is treated as an error information if 
 * the statusCode is not equal to 0. 
 */
struct CommandResult {

	CommandResult() : 
		statusCode(CMDLINE_RETCODE_FAIL), info() {
	}

	CommandResult(int sc) : 
		statusCode(sc), info() {
	}

	// TODO : add other helper functions 

	int statusCode;                        /// command status code as described in mbed-client-cli/ns_cmdline.h.  
	picojson::value info;                  /// additional informations
};


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
 *    - static ConstArray<Command_t> commands() : The array of commands presents in the suite
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
 * static ConstArray<Command_t> commands() {
 *    static const Command_t commandHandlers[] = {
 *        { "foo", doFoo }, 
 *        { "bar", doBar }
 *    }
 *    
 *    return ConstArray<Command_t>(commandHandlers);
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
class CLICommandSuite {

public:
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
		const ConstArray<Command_t> commands = SuiteDescription::commands();

		CommandResult result;

		for(size_t i = 0; i < commands.count(); ++i) {
			if(strcmp(commands[i].name, commandName) == 0) {
				result = commands[i].handler(args.drop(2));
				break;
			}
		}

		return result.statusCode;
	}
};


/**
 * @brief Register a command suite into the system
 * 
 * @tparam CommandSuiteDescription The command suite to register
 */
template<typename CommandSuiteDescription>
void registerCommandSuite() {
	CLICommandSuite<CommandSuiteDescription>::registerSuite();
}





#endif //BLE_CLIAPP_COMMAND_SUITE_H_