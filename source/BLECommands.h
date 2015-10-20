#ifndef BLE_CLIAPP_BLE_COMMANDS_H_
#define BLE_CLIAPP_BLE_COMMANDS_H_

#include "CLICommandSuite.h"
#include "ble/BLE.h"
#include "Stringify.h"

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

	static ConstArray<Command_t> commands() {
		static const Command_t commandHandlers[] = {
			{ "shutdown", bleShutdown }, 
			{ "init", bleInit }, 
			{ "reset", bleReset }, 
			{ "get_version", bleGetVersion }
		};

		return ConstArray<Command_t>(commandHandlers);		
	}


private:
	static int bleShutdown(const Args&) {
		ble_error_t err = ble().shutdown();
		cmd_printf("%s\r\n", to_string(err));
		return err ? CMDLINE_RETCODE_FAIL : CMDLINE_RETCODE_SUCCESS;
	} 

	static int bleInit(const Args&) {
		ble_error_t err = ble().init();
		cmd_printf("%s\r\n", to_string(err));
		return err ? CMDLINE_RETCODE_FAIL : CMDLINE_RETCODE_SUCCESS;
	} 

	static int bleReset(const Args&) {
		ble_error_t err = ble().shutdown();
		if(err) {
			cmd_printf("bleReset error, failled to shutdown the ble instance : %s\r\n", to_string(err));
			return CMDLINE_RETCODE_FAIL;
		}

		err = ble().init();

		if(err) {
			cmd_printf("bleReset error, failled to init the ble instance : %s\r\n", to_string(err));
			return CMDLINE_RETCODE_FAIL;
		}

		cmd_printf("ble has been correctly reset\r\n");

		return CMDLINE_RETCODE_SUCCESS;		
	} 

	static int bleGetVersion(const Args&) {
		const char* version = ble().getVersion();

		if(version) {
			cmd_printf("ble version is : %s\r\n", version);
			return CMDLINE_RETCODE_SUCCESS;
		}

		cmd_printf("ble version is not available\r\n");
		return CMDLINE_RETCODE_FAIL;
	}


	static BLE& ble() {
		return BLE::Instance();
	}

};




#endif //BLE_CLIAPP_BLE_COMMANDS_H_