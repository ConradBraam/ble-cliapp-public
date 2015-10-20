#ifndef BLE_CLIAPP_COMMAND_SUITE_H_
#define BLE_CLIAPP_COMMAND_SUITE_H_

#include <stdint.h>

#include "ConstArray.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "Stringify.h"

typedef ConstArray<const char*> Args; 
typedef int (*CommandHandler_t)(const Args& args);


struct Command_t {
	const char* name;
	const CommandHandler_t handler;
};



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


	static int commandHandler(int argc, char** argv) {
		const Args args(argc, argv);
		const char* commandName = args[1];
		const ConstArray<Command_t> commands = SuiteDescription::commands();

		int err = CMDLINE_RETCODE_COMMAND_NOT_FOUND;

		for(size_t i = 0; i < commands.count(); ++i) {
			if(strcmp(commands[i].name, commandName) == 0) {
				err = commands[i].handler(args.drop(2));
				break;
			}
		}

		cmd_printf("%s\r\n", cmdline_retcode_to_string(err));
		return err;
	}
};



template<typename CommandSuiteDescription>
void registerCommandSuite() {
	CLICommandSuite<CommandSuiteDescription>::registerSuite();
}





#endif //BLE_CLIAPP_COMMAND_SUITE_H_