#ifndef BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_
#define BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_

/**
 * @brief simple description of an argument of a command
 */
struct CommandArgDescription {
#if defined(ENABLE_COMMAND_ARG_DESCRIPTION)
    const char* name;                                /// The name of the argument
    const char* desc;                                /// The description of the argument
#else
    const bool name:1;                                /// The name of the argument
    const bool desc:1;                                /// The description of the argument
#endif
};

#endif //BLE_CLIAPP_COMMAND_ARGS_DESCRIPTION_H_
