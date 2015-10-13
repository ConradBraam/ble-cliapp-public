// mbed
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"

// libraries
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"

#define YOTTA_CFG_MBED_CLIENT_TRACE
#include "mbed-client-trace/mbed_client_trace.h"

#define TRACE_GROUP "appl"

// application
#include "tests/A/node_1.h"
#include "tests/A/node_2.h"

#include "tests/HRM/node_1.h"
#include "tests/HRM/node_2.h"

// BLE object
BLE* ble_ptr;

// prototypes
int cmd_test(int argc, char* argv[]);

                    
void init_ble_test(BLE* ble)
{
    ble_ptr = ble;
}    
int cmd_test(int argc, char* argv[])
{
    if( cmd_parameter_index(argc, argv, (char*)"A") == 1 ) {
        if( cmd_parameter_index(argc, argv, (char*)"1") == 2 ) {
            return cmd_test_A_node1(argc, argv, ble_ptr);
        } else
        if( cmd_parameter_index(argc, argv, (char*)"2") == 2 ) {
            return cmd_test_A_node2(argc, argv, ble_ptr);
        } else {
            return CMDLINE_RETCODE_INVALID_PARAMETERS;
        }
    } else if( cmd_parameter_index(argc, argv, (char*)"HRM") == 1 ) {
        if( cmd_parameter_index(argc, argv, (char*)"1") == 2 ) {
            return cmd_test_HRM_node1(argc, argv, ble_ptr);
        } else
        if( cmd_parameter_index(argc, argv, (char*)"2") == 2 ) {
            return cmd_test_HRM_node2(argc, argv, ble_ptr);
        } else {
            return CMDLINE_RETCODE_INVALID_PARAMETERS;
        }
    }
    return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
}
