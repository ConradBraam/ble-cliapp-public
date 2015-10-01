// mbed
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"

// libraries
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-cli/ns_types.h"
#include "mbed-client-trace/mbed_client_trace.h"

#define TRACE_GROUP "appl"

// application
#include "commands.h"
#include "test_A/node_1.h"
#include "test_A/node_2.h"

#include "test_HRM/node_1.h"
#include "test_HRM/node_2.h"

// BLE object
BLE ble;

// prototypes
int cmd_test(int argc, char* argv[]);
int cmd_ifconfig(int argc, char* argv[]);
int cmd_ifconfig_ble(int argc, char* argv[]);
int cmd_ifconfig_print(int argc, char* argv[]);


// help function for cmd_print
char* cmd_print_address(Gap::Address_t addr);

#define MAN_IFCONFIG    "ifconfig (<iface>) ([options])\r\n"\
                    "<iface>                Interface, default: ble0\r\n"\
                    "ifup                   Initializa\r\n"\
                    "ifdown                 Interface down\r\n"\
                    "reset                  Reset interface\r\n"\
                    "--set-dev-name <name>  Set Device Name\r\n"\
                    "--addr <addr>          Set Address\r\n"\
                    "--adv-timeout <t>      Set Advertising timeout\r\n"\
                    "--adv-interval <t>     Set Advertising interval\r\n"\
                    "start-adv              StartAdvertising\r\n"\
                    "clear-adv-payload      Clear Adv payload\r\n"\
                    "--acc-adv-tx-pwr <dBm> Set Acc adv tx power\r\n"\
                    "--acc-adv-payload <type> (<payload>)\r\n"\
                    "                       Set adv payload type and data\r\n"\
                    "                       Allowed types:\r\n"\
                    "                       LE_GENERAL_DISCOVERABLE\r\n"\
                    "                       BREDR_NOT_SUPPORTED\r\n"\
                    "                       SERVICE_DATA\r\n"\
                    "--set-appearance <app> Set Appearance\r\n"\
                    "                       Allowed values:\r\n"\
                    "                       GENERIC_PHONE"\
                    "--scan-params <interval>,<win>,<?>,<active scan>\r\n"\
                    "                       set scan-parameters\r\n"\
                    "--set-preferred-conn-params <a>,<b>,<c>,<d>\r\n"\
                    "                       Set Preferred Connection Parameters\r\n"\
                    "                       <a>: Minimum Connection Interval in 1.25 ms units"\
                    "                       <b>: Maximum Connection Interval in 1.25 ms units"\
                    "                       <c>: Slave Latency in number of connection events"\
                    "                       <d>: Connection Supervision Timeout in 10 ms units"\
                    "\r\n"\
                    "                       Without [options] just print current config\r\n"
                    
                    
#define MAN_TEST    "test <id> <node> ([options]) Execute individual test\r\n"\
                    "<id>                   TC ID. Allowed values:  'A', 'HRM', 'BlockTransfer' ..\r\n"\
                    "<node>                 Node ID. Allowed values:  '1','2'\r\n"\
                    "<options>              e.g. subtest for A:1\r\n"\
                    "                       shutdownTest\r\n"\
                    "                       setTimeoutTest\r\n"\
                    "                       changePayloadTest\r\n"\
                    "                       changeIntervalTest\r\n"\
                    "                       setAddrTest\r\n"\
                    "                       setupIBeaconTest\r\n"
                    
void initialize_app_commands(void)
{
    cmd_add("ifconfig", cmd_ifconfig, "Configure Network", MAN_IFCONFIG);
    cmd_add("test", cmd_test, "Do device-side tests", MAN_TEST);
}

int cmd_test(int argc, char* argv[])
{
    if( cmd_parameter_index(argc, argv, "A") == 1 ) {
        if( cmd_parameter_index(argc, argv, "1") == 2 ) {
            return cmd_test_A_node1(argc, argv, &ble);
        } else 
        if( cmd_parameter_index(argc, argv, "2") == 2 ) {
            return cmd_test_A_node2(argc, argv, &ble);
        } else { 
            return CMDLINE_RETCODE_INVALID_PARAMETERS; 
        }
    } else if( cmd_parameter_index(argc, argv, "HRM") == 1 ) {
        if( cmd_parameter_index(argc, argv, "1") == 2 ) {
            return cmd_test_HRM_node1(argc, argv, &ble);
        } else 
        if( cmd_parameter_index(argc, argv, "2") == 2 ) {
            return cmd_test_HRM_node2(argc, argv, &ble);
        } else { 
            return CMDLINE_RETCODE_INVALID_PARAMETERS; 
        }
    }
    return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
}

char address_tmp[16];
char* cmd_print_address(Gap::Address_t addr)
{
    if(snprintf(address_tmp, 16, "%d:%d:%d:%d:%d:%d", 
       addr[0], addr[1], addr[2], addr[3], addr[4], addr[5])>0) {
        return address_tmp;
    }
    return "";
}
int cmd_ifconfig_print(int argc, char* argv[])
{
    tr_debug("called cmd_ifconfig_print()");
    // print all current interface configurations
    
    cmd_printf(" Interface ble0\r\n");
    
    /* Read in the MAC address of this peripheral. The corresponding central will be
     * commanded to co-ordinate with this address. */
    Gap::Address_t     address;
    Gap::AddressType_t addressType;
    ble.gap().getAddress(&addressType, address);
    if( addressType == Gap::ADDR_TYPE_PUBLIC ) {
        cmd_printf("Public-Address: %s\r\n", cmd_print_address(address));
    }
    
    uint16_t interval = ble.gap().getAdvertisingParams().getInterval();
    cmd_printf("Advertising interval: %d\r\n", interval);
    
    /* Check that the state is one of the valid values. */
    Gap::GapState_t state = ble.gap().getState();
    if (state.connected == 1) cmd_printf("Connected\r\n");
    if (state.advertising == 1) cmd_printf("Advertising\r\n");
    
    static const size_t MAX_DEVICE_NAME_LEN = 50;
    uint8_t  deviceName[MAX_DEVICE_NAME_LEN];
    unsigned length = MAX_DEVICE_NAME_LEN;
    ASSERT_NO_FAILURE(ble.gap().getDeviceName(deviceName, &length));
    cmd_printf("Device Name:  %*s\r\n", length, deviceName);
    
    GapAdvertisingData::Appearance appearance;
    ASSERT_NO_FAILURE(ble.gap().getAppearance(&appearance));
    cmd_printf("Appearance:  ");
    switch( appearance ){
        case(GapAdvertisingData::GENERIC_PHONE):
            cmd_printf("GENERIC_PHONE\r\n");
            break;
        default:
            cmd_printf("UNKNOWN\r\n");
            break;
    }
    
    Gap::ConnectionParams_t params;
    ble.gap().getPreferredConnectionParams(&params);
    cmd_printf(" minConnectionInterval:        %d\r\n", params.minConnectionInterval);
    cmd_printf(" maxConnectionInterval:        %d\r\n", params.maxConnectionInterval);
    cmd_printf(" slaveLatency:                 %d\r\n", params.slaveLatency);
    cmd_printf(" connectionSupervisionTimeout: %d\r\n", params.connectionSupervisionTimeout);
    //...
       
    return CMDLINE_RETCODE_SUCCESS;
}
void cmd_ifconfig_ble_reset(void)
{
    ble.gap().stopAdvertising();
    ble.gap().clearAdvertisingPayload();
    ble.gap().clearScanResponse();
    ble.gap().setAdvertisingTimeout(0);
    ble.gap().setAdvertisingInterval(1000);

    const static uint8_t trivialAdvPayload[] = {0, 0, 0, 0, 0};
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, trivialAdvPayload, sizeof(trivialAdvPayload));
}
int cmd_ifconfig_ble(int argc, char* argv[])
{
    tr_debug("called cmd_ifconfig_ble()");
    int32_t ival;
    char *val;
    int ret = -1;
    
    if( argc == 1 ) {
        return cmd_ifconfig_print(argc, argv);
    }
    if( cmd_parameter_index(argc, argv, "reset" ) == 1 ) {
        cmd_ifconfig_ble_reset();
    } else if( cmd_parameter_index(argc, argv, "ifup" ) == 1 ) {
        ret = ble.init();
    } else if( cmd_parameter_index(argc, argv, "ifdown" ) == 1 ) {
        ble.shutdown();
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, "--set-dev-name", &val) ) {
        ble.gap().setDeviceName((uint8_t*)val);
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, "--addr", &val) ){
        Gap::Address_t address; 
        // @todo convert string to address..
        // ...
        ret = ble.gap().setAddress(Gap::ADDR_TYPE_PUBLIC, address);
    } else if( cmd_parameter_int(argc, argv, "--adv-timeout", &ival)) {
        ble.gap().setAdvertisingTimeout(ival); /* in milliseconds. */
        ret = 0;
    } else if( cmd_parameter_int(argc, argv, "--adv-interval", &ival)) {
        ble.gap().setAdvertisingInterval(ival); /* in milliseconds. */
        ret = 0;
    } else if( cmd_parameter_index(argc, argv, "start-adv")>0) {
        ASSERT_NO_FAILURE(ble.gap().startAdvertising());
        ret = 0;
    } else if( cmd_parameter_index(argc, argv, "clear-adv-payload")>0) {
        ble.gap().clearAdvertisingPayload();
        ret = 0;
    } else  if( cmd_parameter_int(argc, argv, "--acc-adv-tx-pwr", &ival)) {
        ble.gap().accumulateAdvertisingPayloadTxPower(ival);
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, "--acc-adv-payload", &val) ){
        uint8_t mask = 0;
        if( strstr(val, "LE_GENERAL_DISCOVERABLE")>0) {
            mask |= GapAdvertisingData::LE_GENERAL_DISCOVERABLE;
        } 
        if( strstr(val, "BREDR_NOT_SUPPORTED") > 0) {
            mask |= GapAdvertisingData::BREDR_NOT_SUPPORTED;
        }
        if( strstr(val, "SERVICE_DATA")>0) {
            mask |= GapAdvertisingData::SERVICE_DATA;
            if( cmd_parameter_val(argc, argv, "--payload", &val) ){
                //const static uint8_t trivialAdvPayload[] = {123, 123, 123, 123, 123};
                //@todo convert val to array and overwrite above array
                //ASSERT_NO_FAILURE(ble.gap().accumulateAdvertisingPayload(mask, trivialAdvPayload, sizeof(trivialAdvPayload)));
                //return CMDLINE_RETCODE_SUCCESS;
                return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
            }
        }
        ble.gap().accumulateAdvertisingPayload( mask );
    } else if( cmd_parameter_val(argc, argv, "--scan-params", &val) ) {
        char *ptr = val;
        int scan_window = 200;
        bool active_scan = true;
        int timeout = 0;
        
        int scan_interval = strtol(ptr, &ptr, 10);
        if( scan_interval == 0 ) scan_interval = 500;
        if( ptr && *ptr && ptr[1] != 0 ) {
            scan_window = strtol((ptr+1), &ptr, 10);
        }
        if( ptr && *ptr && ptr[1] != 0 ) {
            timeout = strtol((ptr+1), &ptr, 10);
        }
        if( ptr && *ptr && ptr[1] != 0 ) {
            active_scan = strcmp(ptr, "true")==0;
        }
        ble.gap().setScanParams( scan_interval, scan_window, timeout, active_scan );
    } else if( cmd_parameter_val(argc, argv, "--set-appearance", &val) ) {
        GapAdvertisingData::Appearance appearance;
        if( strcmp(val, "GENERIC_PHONE") == 0 ){
            appearance = GapAdvertisingData::GENERIC_PHONE;
        } else {
            return CMDLINE_RETCODE_INVALID_PARAMETERS;
        }
        ASSERT_NO_FAILURE(ble.gap().setAppearance(appearance));
    }  else if( cmd_parameter_val(argc, argv, "--set-preferred-conn-params", &val) ) {
        char *ptr = val;
        Gap::ConnectionParams_t params =  {50, 500, 0, 500};
        for(int i=0;i<4; i++) {
            if( *ptr && ptr[1] != 0 ) {
                uint16_t value = strtoul((ptr+1), &ptr, 10);
                switch(i) {
                    case(0): params.maxConnectionInterval = value; break;
                    case(1): params.minConnectionInterval = value; break;
                    case(2): params.slaveLatency = value; break;
                    case(3): params.connectionSupervisionTimeout = value; break;
                    default: break;
                }
            }
        }
        ASSERT_NO_FAILURE(ble.gap().setPreferredConnectionParams(&params));
        ret = 0;
    }
    else {
        return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
    }
    return ret==0?CMDLINE_RETCODE_SUCCESS:CMDLINE_RETCODE_FAIL;
}

int cmd_ifconfig(int argc, char* argv[])
{
    tr_debug("called cmd_ifconfig()");
    //default ble0 -interface
    return cmd_ifconfig_ble(argc, argv);
}

