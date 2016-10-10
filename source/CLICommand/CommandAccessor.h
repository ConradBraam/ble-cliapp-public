#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_

#include "Command.h"

template<typename T>
struct CommandAccessor { 
    static const Command command;
};

template<typename T>
const Command CommandAccessor<T>::command =  {
        &T::name,
#if defined(ENABLE_COMMAND_HELP)
        &T::help,
#endif
        &T::argsDescription,
        &T::template maximumArgsRequired<T>,
        &T::handler
};

typedef const Command* (*CommandAccessor_t)();

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
