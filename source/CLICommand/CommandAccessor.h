#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_

#include "Command.h"

template<typename T>
struct CommandSingleton {
    static const T command;
};

template<typename T>
const T CommandSingleton<T>::command = T();

/**
 * Accessor to a command of type T. The command is held in a singleton.
 */
template<typename T>
const Command& getCommand() {
    return CommandSingleton<T>::command;
}

typedef const Command& (*CommandAccessor_t)();

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
