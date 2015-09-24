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

BLE ble;

// prototypes
int cmd_ifconfig(int argc, char* argv[]);
int cmd_ifconfig_ble(int argc, char* argv[]);
int cmd_ifconfig_print(int argc, char* argv[]);

// help function for cmd_print
char* cmd_print_address(Gap::Address_t addr);


#define MAN_IFCONFIG    "ifconfig (<iface>) ([options])\r\n"\
                    "<iface>                Interface, default: ble0\r\n"\
                    "init                   Initializa\r\n"\
                    "                       Without options just print current config\r\n"

void initialize_app_commands(void)
{
    cmd_add("ifconfig", cmd_ifconfig, "Configure Network", MAN_IFCONFIG);
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
int cmd_ifconfig_ble(int argc, char* argv[])
{
    tr_debug("called cmd_ifconfig_ble()");
    int ival;
    char *val;
    int ret = -1;
    
    if( argc == 1 ) {
        return cmd_ifconfig_print(argc, argv);
    }
    if( cmd_parameter_index(argc, argv, "init" ) == 1 ) {
        ret = ble.init();
    } else if( cmd_parameter_val(argc, argv, "--addr", &val) ){
        Gap::Address_t address; // @todo convert string to address..
        ret = ble.gap().setAddress(Gap::ADDR_TYPE_PUBLIC, address);
    } else if( cmd_parameter_int(argc, argv, "--adv-timeout", &ival)) {
        ble.gap().setAdvertisingInterval(ival); /* in milliseconds. */
        ret = 0;
    } else {
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

