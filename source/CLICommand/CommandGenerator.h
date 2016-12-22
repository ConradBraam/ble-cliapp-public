#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_GENERATOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_GENERATOR_H_

#include "Command.h"
#include "CommandHandlerGenerator.h"

/**
 * @brief Translate BaseClass and derived classes into an instance of Command.
 * @details This structure expose a single immutable static field: command. 
 * command is the Command instance generated from the class T.
 * 
 * @tparam T The class used to generate a Command. It should inherit from BaseCommand
 * or expose the same static member functions present in BaseCommand.
 */
template<typename T>
struct CommandGenerator { 
    /**
     * The runtime Command generated from T.
     */
    static const Command command;
};

/**
 * @brief Instanciation of the command for the class T.
 */
template<typename T>
const Command CommandGenerator<T>::command =  {
        &T::name,
#if defined(ENABLE_COMMAND_HELP)
        &T::help,
#endif
        &T::argsDescription,
        &T::resultDescription,
        &T::template maximumArgsRequired<T>,
        &CommandHandlerGenerator::handler<T>
};

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_GENERATOR_H_
