// mbed
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"

// libraries
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#define TRACE_GROUP "appl"

// application
#include "commands.h"
#include "test_A/test_A.h"

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
                    "--addr <addr>          Set Address\r\n"\
                    "--adv-timeout <t>      Set Advertising timeout\r\n"\
                    "--adv-interval <t>     Set Advertising interval\r\n"\
                    "start-adv              StartAdvertising\r\n"\
                    "clear-adv-payload      Clear Adv payload\r\n"\
                    "--acc-adv-tx-pwr <dBm>   Set Acc adv tx power\r\n"\
                    "--acc-adv-payload <type> (<payload>)\r\n"\
                    "                       Set adv payload type and data\r\n"\
                    "                       Allowed types:\r\n"\
                    "                       LE_GENERAL_DISCOVERABLE\r\n"\
                    "                       BREDR_NOT_SUPPORTED\r\n"\
                    "                       SERVICE_DATA\r\n"\
                    "\r\n"\
                    "                       Without options just print current config\r\n"

#define MAN_TEST    "test <g> <id> ([options]) Execute individual test\r\n"\
                    "<g>                    Group, allowed values: 'A', 'B', 'AHRM', 'BHRM', 'BlockTransfer' ..\r\n"\
                    "<id>                   TC ID:\r\n"\
                    "                       shutdownTest\r\n"\
                    "                       setTimeoutTest\r\n"\
                    "                       changePayloadTest\r\n"\
                    "                       changeIntervalTest\r\n"\
                    "                       setAddrTest\r\n"\
                    "                       setupIBeaconTest\r\n"
                    
void initialize_app_commands(void)
{
    cmd_add("ifconfig", cmd_ifconfig, "Configure Network", MAN_IFCONFIG);
    cmd_add("test", cmd_test, "Do device-side tests", NULL);
}

int cmd_test(int argc, char* argv[])
{
    if( cmd_parameter_index(argc, argv, "A") == 1 ) {
        return cmd_test_A(argc, argv, &ble);
    }/* else if( cmd_parameter_index(argc, argv, "B") == 1 ) {
        return cmd_test_B(argc, argv, &ble);
    } else if( cmd_parameter_index(argc, argv, "AHRM") == 1 ) {
        return cmd_test_AHRM(argc, argv, &ble);
    }  else if( cmd_parameter_index(argc, argv, "BHRM") == 1 ) {
        return cmd_test_BHRM(argc, argv, &ble);
    }*/
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
    int ival;
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
                const static uint8_t trivialAdvPayload[] = {123, 123, 123, 123, 123};
                //@todo convert val to array
                //ASSERT_NO_FAILURE(ble.gap().accumulateAdvertisingPayload(mask, trivialAdvPayload, sizeof(trivialAdvPayload)));
                ret = 0;
            }
        }
        ble.gap().accumulateAdvertisingPayload( mask );
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

