#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_

#include "Command.h"

template<typename T>
struct CommandAccessor { 
    static const CommandTable command;
};

template<typename T>
const CommandTable CommandAccessor<T>::command =  {
        &T::name,
        &T::help,
        &T::argsDescription,
        &T::template maximumArgsRequired<T>,
        &T::handler
};

typedef const CommandTable* (*CommandAccessor_t)();

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
