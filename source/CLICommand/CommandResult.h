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

	/**
	 * @brief default constructor for a CommandResult, the status code is CMDLINE_RETCODE_FAIL
	 */
	CommandResult() : 
		statusCode(CMDLINE_RETCODE_FAIL), info() {
	}

	/**
	 * @brief Construct a command result with the given status code
	 *
	 * @param sc The status code to set
	 */
	CommandResult(int sc) : 
		statusCode(sc), info() {
	}

	/**
	 * @brief Construct a command result with all its parameters
	 * 
	 * @param sc The status code of the result
	 * @param inf The information associated with the result. It is a JSON value.
	 */
	CommandResult(int sc, const picojson::value& inf) : 
		statusCode(sc), info(inf) {
	}

	/**
	 * @brief Construct a CommandResult which indicate a faillure
	 */
	static CommandResult faillure() {
		return CommandResult(CMDLINE_RETCODE_FAIL);
	}

	/**
	 * @brief Construct a CommandResult which indicate a faillure and provide the reason of the faillure.
	 * 
	 * @param reason The reason of the faillurem it should be a JSON value
	 */
	static CommandResult faillure(const picojson::value& reason) {
		return CommandResult(
			CMDLINE_RETCODE_FAIL, 
			reason
		);
	}

	/**
	 * @brief Construct a CommandResult which indicate a faillure and provide the reason of the faillure.
	 * 
	 * @param reason The reason of the faillurem it should be a str value
	 */
	static CommandResult faillure(const char* reason) {
		return faillure(picojson::value(reason));
	}

	/**
	 * @brief Construct a CommandResult which indicate a success
	 */
	static CommandResult success() {
		return CommandResult(CMDLINE_RETCODE_SUCCESS);
	}

	/**
	 * @brief Construct a CommandResult which indicate a success and provide the reason or another informations
	 * related to the succes.
	 * 
	 * @param reason The reason of the success, it should be a JSON value
	 */
	static CommandResult success(const picojson::value& reason) {
		return CommandResult(
			CMDLINE_RETCODE_SUCCESS, 
			reason
		);
	}

	/**
	 * @brief Construct a CommandResult which indicate a success and provide the reason or another informations
	 * related to the succes.
	 * 
	 * @param reason The reason of the success, it should be a str value
	 */
	static CommandResult success(const char* reason) {
		return success(picojson::value(reason));
	}

	/**
	 * @brief Construct a CommandResult which indicate that a parameter was invalid
	 * 
	 * @param reason Indication about the parameter invalid. It is a JSON value.
	 */
	static CommandResult invalidParameters(const picojson::value& reason) { 
		return CommandResult(
			CMDLINE_RETCODE_INVALID_PARAMETERS, 
			reason
		);
	}

	/**
	 * @brief Construct a CommandResult which indicate that a parameter was invalid
	 * 
	 * @param reason Indication about the parameter invalid. It is a str value.
	 */
	static CommandResult invalidParameters(const char* reason) { 
		return invalidParameters(picojson::value(reason));
	}

	int statusCode;                        /// command status code as described in mbed-client-cli/ns_cmdline.h.  
	picojson::value info;                  /// additional informations
};

#endif //BLE_CLIAPP_COMMAND_RESULT_H_