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

	CommandResult(int sc, const picojson::value& inf) : 
		statusCode(sc), info(inf) {
	}

	static CommandResult faillure() {
		return CommandResult(CMDLINE_RETCODE_FAIL);
	}

	static CommandResult faillure(const picojson::value& reason) {
		return CommandResult(
			CMDLINE_RETCODE_FAIL, 
			reason
		);
	}

	static CommandResult faillure(const char* reason) {
		return faillure(picojson::value(reason));
	}


	static CommandResult success() {
		return CommandResult(CMDLINE_RETCODE_SUCCESS);
	}

	static CommandResult success(const picojson::value& reason) {
		return CommandResult(
			CMDLINE_RETCODE_SUCCESS, 
			reason
		);
	}

	static CommandResult success(const char* reason) {
		return success(picojson::value(reason));
	}


	static CommandResult invalidParameters(const picojson::value& reason) { 
		return CommandResult(
			CMDLINE_RETCODE_INVALID_PARAMETERS, 
			reason
		);
	}

	static CommandResult invalidParameters(const char* reason) { 
		return invalidParameters(picojson::value(reason));
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
		const CommandArgs commandArgs(args.drop(2));

		CommandResult result;

		for(size_t i = 0; i < commands.count(); ++i) {
			if(strcmp(commands[i].name, commandName) == 0) {
				result = commands[i].handler(commandArgs);
				break;
			}
		}

		printCommandResult(SuiteDescription::name(), commandName, commandArgs, result);
		return result.statusCode;
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
		namespace pj = picojson;

		pj::value message(pj::object_type, true);

		pj::object& msgObject = message.get<pj::object>();

		// build the name 
		msgObject["name"] = pj::value(std::string(commandGroup) + " " + commandName);

		// build the arguments 
		pj::value arguments(pj::array_type, true);
		for(size_t i = 0; i < args.count(); ++i) {
			arguments.get<pj::array>().push_back(pj::value(args[i]));
		}
		msgObject["arguments"] = pj::value(arguments);

		// set the command status
		msgObject["status"] = pj::value((int64_t) result.statusCode);

		// add additional informations if there is any
		if(result.info.is<pj::null>() == false) {
			// in case of a status code equal to 0, it is the result of the command
			if(result.statusCode == 0) {
				msgObject["result"] = result.info;
			} else {
				// otherwise it is the error reason
				msgObject["error"] = result.info;
			}
		}

		cmd_printf("%s\r\n", message.serialize(true).c_str());
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