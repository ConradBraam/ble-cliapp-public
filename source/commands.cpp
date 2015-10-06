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

char* ble_error_to_text(ble_error_t erno);
char* ble_appearance_to_text(GapAdvertisingData::Appearance appearance);


// help function for cmd_print
char* cmd_print_address(Gap::Address_t addr);

#define MAN_IFCONFIG    "ifconfig (<iface>) ([options])\r\n"\
                    "<iface>                Interface, default: ble0\r\n"\
                    "up                     Initializa\r\n"\
                    "down                   Interface down\r\n"\
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

char* cmd_print_address(Gap::Address_t addr)
{
    static char address_tmp[20];
    if( snprintf(address_tmp, sizeof(address_tmp) / sizeof(char), "%2X:%2X:%2X:%2X:%2X:%2X",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]) > 0 ) {
        return address_tmp;
    }
    return "";
}

char* ble_error_to_text(ble_error_t erno)
{
    switch(erno) {
        case(BLE_ERROR_NONE):
            return "No error";
        case(BLE_ERROR_BUFFER_OVERFLOW):
            return "The requested action would cause a buffer overflow and has been aborted";
        case(BLE_ERROR_NOT_IMPLEMENTED):
            return "Requested a feature that isn't yet implement or isn't supported by the target HW";
        case(BLE_ERROR_PARAM_OUT_OF_RANGE):
            return "One of the supplied parameters is outside the valid range";
        case(BLE_ERROR_INVALID_PARAM):
            return "One of the supplied parameters is invalid";
        case(BLE_STACK_BUSY):
            return "The stack is busy";
        case(BLE_ERROR_INVALID_STATE):
            return "Invalid state";
        case(BLE_ERROR_NO_MEM):
            return "Out of Memory";
        case(BLE_ERROR_OPERATION_NOT_PERMITTED ):
            return "Operation not permitted";
        case(BLE_ERROR_UNSPECIFIED):
            return "Unspecified";
        default:
            tr_debug("Unknown ble_error_t: %d", erno);
            return "Unknown ble_error_t error number";

    }
}
char* ble_appearance_to_text(GapAdvertisingData::Appearance appearance)
{
    /// @todo separate table, which can be use like this, but also
    ///       to convert text to GapAdvertisingData::Appearance
    ///       for cmd: "ifconfig --set-appearance <appearance>"
    switch(appearance) {
        case(GapAdvertisingData::UNKNOWN):
            return "Unknown of unspecified appearance type";
        case(GapAdvertisingData::GENERIC_PHONE):
            return "Generic Phone";
        case(GapAdvertisingData::GENERIC_COMPUTER): return "Generic Computer";
        case(GapAdvertisingData::GENERIC_WATCH): return "Generic Watch";
        case(GapAdvertisingData::WATCH_SPORTS_WATCH): return "Sports Watch";
        case(GapAdvertisingData::GENERIC_CLOCK): return "Generic Clock";
        case(GapAdvertisingData::GENERIC_DISPLAY): return "Generic Display";
        case(GapAdvertisingData::GENERIC_REMOTE_CONTROL): return "Generic Remote Control";
        case(GapAdvertisingData::GENERIC_EYE_GLASSES): return "Generic Eye Glasses";
        case(GapAdvertisingData::GENERIC_TAG): return "Generic Tag";
        case(GapAdvertisingData::GENERIC_KEYRING): return "Generic Keyring";
        case(GapAdvertisingData::GENERIC_MEDIA_PLAYER): return "Generic Media Player";
        case(GapAdvertisingData::GENERIC_BARCODE_SCANNER): return "Generic Barcode Scanner";
        case(GapAdvertisingData::GENERIC_THERMOMETER): return "Generic Thermometer";
        case(GapAdvertisingData::THERMOMETER_EAR): return "Ear Thermometer";
        case(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR): return "Generic Heart Rate Sensor";
        case(GapAdvertisingData::HEART_RATE_SENSOR_HEART_RATE_BELT): return "Belt Heart Rate Sensor";
        case(GapAdvertisingData::GENERIC_BLOOD_PRESSURE): return "Generic Blood Pressure";
        case(GapAdvertisingData::BLOOD_PRESSURE_ARM): return "Arm Blood Pressure";
        case(GapAdvertisingData::BLOOD_PRESSURE_WRIST): return "Wrist Blood Pressure";
        case(GapAdvertisingData::HUMAN_INTERFACE_DEVICE_HID): return "Human Interface Device (HID)";
        case(GapAdvertisingData::KEYBOARD): return "Keyboard";
        case(GapAdvertisingData::MOUSE): return "Mouse";
        case(GapAdvertisingData::JOYSTICK): return "Joystick";
        case(GapAdvertisingData::GAMEPAD): return "Gamepad";
        case(GapAdvertisingData::DIGITIZER_TABLET): return "Digitizer Tablet";
        case(GapAdvertisingData::CARD_READER): return "Card Read";
        case(GapAdvertisingData::DIGITAL_PEN): return "Digital Pen";
        case(GapAdvertisingData::BARCODE_SCANNER): return "Barcode Scanner";
        case(GapAdvertisingData::GENERIC_GLUCOSE_METER): return "Generic Glucose Meter";
        case(GapAdvertisingData::GENERIC_RUNNING_WALKING_SENSOR): return "Generic Running/Walking Sensor";
        case(GapAdvertisingData::RUNNING_WALKING_SENSOR_IN_SHOE): return "In Shoe Running/Walking Sensor";
        case(GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_SHOE): return "On Shoe Running/Walking Sensor";
        case(GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_HIP): return "On Hip Running/Walking Sensor";
        case(GapAdvertisingData::GENERIC_CYCLING): return "Generic Cycling";
        case(GapAdvertisingData::CYCLING_CYCLING_COMPUTER): return "Cycling Computer";
        case(GapAdvertisingData::CYCLING_SPEED_SENSOR): return "Cycling Speed Senspr";
        case(GapAdvertisingData::CYCLING_CADENCE_SENSOR): return "Cycling Cadence Sensor";
        case(GapAdvertisingData::CYCLING_POWER_SENSOR): return "Cycling Power Sensor";
        case(GapAdvertisingData::CYCLING_SPEED_AND_CADENCE_SENSOR):
            return "Cycling Speed and Cadence Sensor";
        case(GapAdvertisingData::PULSE_OXIMETER_GENERIC):
            return "Generic Pulse Oximeter";
        case(GapAdvertisingData::PULSE_OXIMETER_FINGERTIP):
            return "Fingertip Pulse Oximeter";
        case(GapAdvertisingData::PULSE_OXIMETER_WRIST_WORN):
            return "Wrist Worn Pulse Oximeter";
        case(GapAdvertisingData::OUTDOOR_GENERIC):
            return "Generic Outdoor";
        case(GapAdvertisingData::OUTDOOR_LOCATION_DISPLAY_DEVICE):
            return "Outdoor Location Display Device";
        case(GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE):
            return "Outdoor Location and Navigation Display Device";
        case(GapAdvertisingData::OUTDOOR_LOCATION_POD):
            return "Outdoor Location Pod";
        case(GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_POD):
            return "Location And Navigarion POD";
        default:
            tr_warn("Unknown appearance: %d", appearance);
            return "Unknown appearance";
    }
}

int cmd_ifconfig_print(int argc, char* argv[])
{
    ble_error_t erno;
    tr_debug("called cmd_ifconfig_print()");
    // print all current interface configurations

    cmd_printf("Interface ble0\r\n");

    static const size_t MAX_DEVICE_NAME_LEN = 50;
    uint8_t  deviceName[MAX_DEVICE_NAME_LEN];
    unsigned length = MAX_DEVICE_NAME_LEN;
    if( ble.gap().getDeviceName(deviceName, &length) == 0 ) {
        cmd_printf("  Device Name:  %*s\r\n", length, deviceName);
    } else {
        cmd_printf("  Device Name:  Unknown\r\n");
    }

    GapAdvertisingData::Appearance appearance;
    if( ble.gap().getAppearance(&appearance) == 0 ) {
        cmd_printf("  Appearance: %s", ble_appearance_to_text(appearance) );
        cmd_printf(" (%d)\r\n", appearance );
    } else {
        cmd_printf("  Appearance: UNKNOWN\r\n");
    }

    Gap::GapState_t state = ble.gap().getState();
    cmd_printf("  Connected:   %s\r\n", state.connected?"true":"false");
    cmd_printf("  Advertising: %s\r\n", state.advertising?"true":"false");
    uint16_t interval = ble.gap().getAdvertisingParams().getInterval();
    cmd_printf("  -interval: %d\r\n", interval);


    Gap::ConnectionParams_t params;
    if( (erno=ble.gap().getPreferredConnectionParams(&params)) == 0 ) {
        cmd_printf("  Preferred Connection Parameters:\r\n");
        cmd_printf("  -minConnectionInterval:        %d\r\n", params.minConnectionInterval);
        cmd_printf("  -maxConnectionInterval:        %d\r\n", params.maxConnectionInterval);
        cmd_printf("  -slaveLatency:                 %d\r\n", params.slaveLatency);
        cmd_printf("  -connectionSupervisionTimeout: %d\r\n", params.connectionSupervisionTimeout);
    }

    /* Read in the MAC address of this peripheral. The corresponding central will be
     * commanded to co-ordinate with this address. */
    cmd_printf("  Radio if address\r\n");
    Gap::Address_t     address;
    Gap::AddressType_t addressType;
    ble.gap().getAddress(&addressType, address);
    switch( addressType  ) {
        case(Gap::ADDR_TYPE_PUBLIC):
            cmd_printf("   Public-Address: %s\r\n", cmd_print_address(address));
            break;
        case(Gap::ADDR_TYPE_RANDOM_STATIC):
            cmd_printf("   Random-Static-Address: %s\r\n", cmd_print_address(address));
            break;
        case(Gap::ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE):
            cmd_printf("   Random-Private-Resolved-Address: %s\r\n", cmd_print_address(address));
            break;
        case(Gap::ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE):
            cmd_printf("   Random-Private-Non-Resolvable-Address: %s\r\n", cmd_print_address(address));
            break;
        default:
            cmd_printf("   Unknown-Type (%d) -Address: %s\r\n", addressType, cmd_print_address(address));
            break;
    }
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
    } else if( cmd_parameter_index(argc, argv, "up" ) == 1 ) {
        ret = ble.init();
    } else if( cmd_parameter_index(argc, argv, "down" ) == 1 ) {
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

