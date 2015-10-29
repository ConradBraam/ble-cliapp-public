#include "BLECommands.h"

// isolation
namespace {

static BLE& ble() {
	return BLE::Instance();
}

static const Command shutdown { 
	"shutdown",
	"Shutdown the current BLE instance, calling ble related function after this"  
	"call may lead to faillure.",
	[] (const CommandArgs&) {
		ble_error_t err = ble().shutdown();
		return err ? CommandResult::faillure() : CommandResult::success();
	} 
};

static const Command init {
	"init",
	"Initialize the ble API and underlying BLE stack.\r\n"
	"Be sure to call this function before any other ble API function",
	[] (const CommandArgs&) {
		ble_error_t err = ble().init();
		return err ? CommandResult::faillure() : CommandResult::success();
	}
};

static const Command reset {
	"reset",
	"Reset the ble API and ble stack.\r\n"
	"This function internaly does a reset and an init",
	[] (const CommandArgs&) {
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
};

static const Command getVersion {
	"getVersion",
	"Return the version of the BLE API.\r\n",
	[] (const CommandArgs&) {
		const char* version = ble().getVersion();

		if(version) {
			return CommandResult::success(version);
		}

		return CommandResult::faillure("ble version is not available");
	} 
};

} // end of annonymous namespace

ConstArray<Command> BLECommandSuiteDescription::commands() {
	static const Command commandHandlers[] = {
		shutdown, 
		init, 
		reset, 
		getVersion
	};

	return ConstArray<Command>(commandHandlers);		
}
