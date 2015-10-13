/** This initialize and connect application used commands to cli -library
 */

#include "ble/BLE.h"
#include "cmd_ble_test.h"
//#include "cmd_ifconfig.h"
#include "cmd_ifconfig_ble.h"

// libraries
#include "mbed-client-cli/ns_cmdline.h"

void initialize_app_commands(void)
{
    init_ble_test(get_ble_instance());
    cmd_add("ifconfig", cmd_ifconfig, "Configure Network", MAN_IFCONFIG);
    cmd_add("test", cmd_test, "Do device-side tests", MAN_TEST);
}

