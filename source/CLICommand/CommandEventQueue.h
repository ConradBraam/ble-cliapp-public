#ifndef BLE_CLIAPP_CLICOMMAND_COMMANDTASKQUEUE_H_
#define BLE_CLIAPP_CLICOMMAND_COMMANDTASKQUEUE_H_

#include "EventQueue/EventQueue.h"

/**
 * @brief Returns the event queue used by the CLICommand module. 
 */
eq::EventQueue* getCLICommandEventQueue();


/**
 * @brief Set the event queue which will be used by the CLICommand module.
 */
void initCLICommandEventQueue(eq::EventQueue* event_queue);


#endif //BLE_CLIAPP_CLICOMMAND_COMMANDTASKQUEUE_H_
