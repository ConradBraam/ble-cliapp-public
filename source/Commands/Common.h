#ifndef BLE_CLIAPP_COMMANDS_COMMON_H_
#define BLE_CLIAPP_COMMANDS_COMMON_H_

#include "CLICommand/CommandSuite.h"
#include "ble/BLE.h"


static BLE& ble() {
    return BLE::Instance();
}



#endif //BLE_CLIAPP_COMMANDS_COMMON_H_
