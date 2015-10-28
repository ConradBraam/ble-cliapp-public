#ifndef BLE_CLIAPP_BLE_COMMANDS_H_
#define BLE_CLIAPP_BLE_COMMANDS_H_

#include "CLICommand/CommandSuite.h"
#include "ble/BLE.h"

class BLECommandSuiteDescription {

public:
	static const char* name() { 
		return "ble";
	} 
 
	static const char* info() {
		return "All commands applicable to BLE instance";
	} 

	static const char* man() {
		return "BLE <command> <command arguments>\r\n"\
		       "	* ble shutdown : shutdown the ble instance of this device\r\n"\
		       "	* ble init : initialise the ble instance of this device\r\n"\
		       "	* ble reset : reset the ble instance of this device\r\n"\
		       "	* ble get_version : return the current version of the BLE stack\r\n";	
	} 

	static ConstArray<Command> commands() {
		static const Command commandHandlers[] = {
			{ "shutdown", bleShutdown }, 
			{ "init", bleInit }, 
			{ "reset", bleReset }, 
			{ "getVersion", bleGetVersion }
		};

		return ConstArray<Command>(commandHandlers);		
	}


private:

	/**
	 * @brief  Shutdown the current BLE instance, calling ble related function after this 
	 * call may lead to faillure
	 * 
	 * @param  no args 
	 * @return just the status code indicating a success or an error 
	 */
	static CommandResult bleShutdown(const CommandArgs&) {
		ble_error_t err = ble().shutdown();
		return err ? CommandResult::faillure() : CommandResult::success();
	} 

	/**
	 * @brief Initialize the ble API and underlying BLE stack
	 * @detail Be sure to call this function before any other ble API function
	 * 
	 * @param  NONE
	 * @return status code indicating a success or an error 
	 */
	static CommandResult bleInit(const CommandArgs&) {
		ble_error_t err = ble().init();
		return err ? CommandResult::faillure() : CommandResult::success();
	} 


	/**
	 * @brief  reset the ble API and ble stack 
	 * @details This function internaly does a reset and an init. 
	 * 
	 * @param  NONE
	 * @return the status code and in case of faillure a message indicating 
	 * what operation has failled
	 */
	static CommandResult bleReset(const CommandArgs&) {
		ble_error_t err = ble().shutdown();
		if(err) {
			return CommandResult::faillure("Failled to shutdown the ble instance");
		}

		err = ble().init();

		if(err) {
			return CommandResult::faillure("Failled to init the ble instance");
		}

		return CommandResult::success();
	} 

	/**
	 * @brief  Return the version of the BLE API 
	 * 
	 * @param  NONE
	 * @return The status code and the version in case of success.
	 */
	static CommandResult bleGetVersion(const CommandArgs&) {
		const char* version = ble().getVersion();

		if(version) {
			return CommandResult::success(version);
		}

		return CommandResult::faillure("ble version is not available");
	}


	static BLE& ble() {
		return BLE::Instance();
	}
};




#endif //BLE_CLIAPP_BLE_COMMANDS_H_