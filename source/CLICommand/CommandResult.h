#ifndef BLE_CLIAPP_COMMAND_RESULT_H_
#define BLE_CLIAPP_COMMAND_RESULT_H_

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "util/picojson.h"

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



#endif //BLE_CLIAPP_COMMAND_RESULT_H_