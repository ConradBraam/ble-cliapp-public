#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_HELPER_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_HELPER_H_

#include "Command.h"

/**
 * @brief Start the declaration of a new command
 * @details Users should describe their commands in the next block.
 * @param COMMAND_NAME the name of the class of the command.
 */
#define DECLARE_CMD(COMMAND_NAME) struct COMMAND_NAME : public BaseCommand

/**
 * @brief Declare the field name of the command.
 * 
 * @param COMMAND_NAME the name of the command, as an array of char.
 */
#define CMD_NAME(COMMAND_NAME) static const char* name() { \
    return COMMAND_NAME; \
}

/**
 * @brief Declare the help field of a command.
 * 
 * @param COMMAND_HELP the help string associated with the command.
 */
#define CMD_HELP(COMMAND_HELP) static const char* help() { \
    return COMMAND_HELP; \
}

/**
 * @brief Declare the arguments of the command.
 * 
 * @param a list of CMD_ARG
 */
#define CMD_ARGS(...) static ConstArray<CommandArgDescription> argsDescription() { \
        static const CommandArgDescription argsDescription[] = { __VA_ARGS__  }; \
        return ConstArray<CommandArgDescription>(argsDescription); \
    }

/**
 * @brief Declare an argument of the command.
 * 
 * @param type: type of the argument
 * @param name: name of the argument
 * @param desc: description of the argument
 */
#define CMD_ARG(type, name, desc) { name, desc }


/**
 * @brief Declare the handler of a command.
 * 
 * @param ARGS_NAME name of the args in the handler function.
 * @param RESPONSE_NAME name of the response in the handler function. 
 * 
 * @return [description]
 */
#define CMD_HANDLER(ARGS_NAME, RESPONSE_NAME) \
    static void handler(const CommandArgs& ARGS_NAME, const SharedPointer<CommandResponse>& RESPONSE_NAME)


#if 0
#define SYNTH_HANDLER(HANDLER) static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response) { \
    synth_handler(args, response, HANDLER); \
}
#endif





#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_HELPER_H_
