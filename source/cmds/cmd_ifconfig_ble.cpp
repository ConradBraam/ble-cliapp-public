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
#include "macros.h"


// BLE object
BLE ble;

// prototypes
int cmd_ifconfig_ble(int argc, char* argv[]);
int cmd_ifconfig_print(int argc, char* argv[]);

const char* ble_error_to_text(ble_error_t erno);

GapAdvertisingData::Appearance get_appearance_id( char* appearance );
const char* get_appearance_text(GapAdvertisingData::Appearance appearance);

// help function for cmd_print
char* cmd_print_address(Gap::Address_t addr);

void init_ifconfig_ble(void)
{
    
}
BLE* get_ble_instance(void)
{
    return &ble;
}

char* cmd_print_address(Gap::Address_t addr)
{
    static char address_tmp[20];
    if( snprintf(address_tmp, sizeof(address_tmp) / sizeof(char), "%2X:%2X:%2X:%2X:%2X:%2X",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]) > 0 ) {
        return address_tmp;
    }
    return (char*)"";
}

const char* ble_error_to_text(ble_error_t erno)
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
typedef struct appearance_map_t {
    GapAdvertisingData::Appearance id;
    const char* key;
    const char* text;
} apperance_map_s;

const appearance_map_t APPEARANCES[] = {
    {  .id = GapAdvertisingData::UNKNOWN,
       .key = "UNKNOWN",
       .text = "Unknown of unspecified appearance type"
    },{.id = GapAdvertisingData::GENERIC_PHONE,
       .key = "GENERIC_PHONE",
       .text = "Generic Phone"
    },{.id = GapAdvertisingData::GENERIC_COMPUTER,
       .key = "GENERIC_COMPUTER",
       .text = "Generic Computer"
    },{.id = GapAdvertisingData::GENERIC_WATCH,
       .key = "GENERIC_WATCH",
       .text = "Generic Watch"
    },{.id = GapAdvertisingData::WATCH_SPORTS_WATCH,
       .key = "WATCH_SPORTS_WATCH",
        .text = "Sports Watch"
    }, {.id = GapAdvertisingData::GENERIC_CLOCK,
       .key = "GENERIC_CLOCK",
        .text = "Generic Clock"
    }, {.id = GapAdvertisingData::GENERIC_DISPLAY,
       .key = "GENERIC_DISPLAY",
        .text = "Generic Display"
    }, {.id = GapAdvertisingData::GENERIC_REMOTE_CONTROL,
       .key = "GENERIC_REMOTE_CONTROL",
        .text = "Generic Remote Control"
    }, {.id = GapAdvertisingData::GENERIC_EYE_GLASSES,
       .key = "GENERIC_EYE_GLASSES",
        .text = "Generic Eye Glasses"
    }, {.id = GapAdvertisingData::GENERIC_TAG,
       .key = "GENERIC_TAG",
        .text = "Generic Tag"
    }, {.id = GapAdvertisingData::GENERIC_KEYRING,
       .key = "GENERIC_KEYRING",
        .text = "Generic Keyring"
    }, {.id = GapAdvertisingData::GENERIC_MEDIA_PLAYER,
        .key = "GENERIC_MEDIA_PLAYER",
        .text = "Generic Media Player"
    }, {.id = GapAdvertisingData::GENERIC_BARCODE_SCANNER,
        .key = "GENERIC_BARCODE_SCANNER",
        .text = "Generic Barcode Scanner"
    }, {.id = GapAdvertisingData::GENERIC_THERMOMETER,
        .key = "GENERIC_THERMOMETER",
        .text = "Generic Thermometer"
    }, {.id = GapAdvertisingData::THERMOMETER_EAR,
        .key = "THERMOMETER_EAR",
        .text = "Ear Thermometer"
    }, {.id = GapAdvertisingData::GENERIC_HEART_RATE_SENSOR,
        .key = "GENERIC_HEART_RATE_SENSOR",
        .text = "Generic Heart Rate Sensor"
    }, {.id = GapAdvertisingData::HEART_RATE_SENSOR_HEART_RATE_BELT,
       .key = "HEART_RATE_SENSOR_HEART_RATE_BELT",
        .text = "Belt Heart Rate Sensor"
    }, {.id = GapAdvertisingData::GENERIC_BLOOD_PRESSURE,
        .key = "GENERIC_BLOOD_PRESSURE",
        .text = "Generic Blood Pressure"
    }, {.id = GapAdvertisingData::BLOOD_PRESSURE_ARM,
        .key = "BLOOD_PRESSURE_ARM",
        .text = "Arm Blood Pressure"
    }, {.id = GapAdvertisingData::BLOOD_PRESSURE_WRIST,
        .key = "BLOOD_PRESSURE_WRIST",
        .text = "Wrist Blood Pressure"
    }, {.id = GapAdvertisingData::HUMAN_INTERFACE_DEVICE_HID,
        .key = "HUMAN_INTERFACE_DEVICE_HID",
        .text = "Human Interface Device (HID)"
    }, {.id = GapAdvertisingData::KEYBOARD,
        .key = "KEYBOARD",
        .text = "Keyboard"
    }, {.id = GapAdvertisingData::MOUSE,
        .key = "MOUSE",
        .text = "Mouse"
    }, {.id = GapAdvertisingData::JOYSTICK,
        .key = "JOYSTICK",
        .text = "Joystick"
    }, {.id = GapAdvertisingData::GAMEPAD,
        .key = "GAMEPAD",
        .text = "Gamepad"
    }, {.id = GapAdvertisingData::DIGITIZER_TABLET,
        .key = "DIGITIZER_TABLET",
        .text = "Digitizer Tablet"
    }, {.id = GapAdvertisingData::CARD_READER,
        .key = "CARD_READER",
        .text = "Card Read"
    }, {.id = GapAdvertisingData::DIGITAL_PEN,
        .key = "DIGITAL_PEN",
        .text = "Digital Pen"
    }, {.id = GapAdvertisingData::BARCODE_SCANNER,
        .key = "BARCODE_SCANNER",
        .text = "Barcode Scanner"
    }, {.id = GapAdvertisingData::GENERIC_GLUCOSE_METER,
        .key = "GENERIC_GLUCOSE_METER",
        .text = "Generic Glucose Meter"
    }, {.id = GapAdvertisingData::GENERIC_RUNNING_WALKING_SENSOR,
        .key = "GENERIC_RUNNING_WALKING_SENSOR",
        .text = "Generic Running/Walking Sensor"
    }, {.id = GapAdvertisingData::RUNNING_WALKING_SENSOR_IN_SHOE,
        .key = "RUNNING_WALKING_SENSOR_IN_SHOE",
        .text = "In Shoe Running/Walking Sensor"
    }, {.id = GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_SHOE,
        .key = "RUNNING_WALKING_SENSOR_ON_SHOE",
        .text = "On Shoe Running/Walking Sensor"
    }, {.id = GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_HIP,
        .key = "RUNNING_WALKING_SENSOR_ON_HIP",
        .text = "On Hip Running/Walking Sensor"
    }, {.id = GapAdvertisingData::GENERIC_CYCLING,
        .key = "GENERIC_CYCLING",
        .text = "Generic Cycling"
    }, {.id = GapAdvertisingData::CYCLING_CYCLING_COMPUTER,
        .key = "CYCLING_CYCLING_COMPUTER",
        .text = "Cycling Computer"
    }, {.id = GapAdvertisingData::CYCLING_SPEED_SENSOR,
        .key = "CYCLING_SPEED_SENSOR",
        .text = "Cycling Speed Senspr"
    }, {.id = GapAdvertisingData::CYCLING_CADENCE_SENSOR,
        .key = "CYCLING_CADENCE_SENSOR",
        .text = "Cycling Cadence Sensor"
    }, {.id = GapAdvertisingData::CYCLING_POWER_SENSOR,
        .key = "CYCLING_POWER_SENSOR",
        .text = "Cycling Power Sensor"
    }, {.id = GapAdvertisingData::CYCLING_SPEED_AND_CADENCE_SENSOR,
        .key = "CYCLING_SPEED_AND_CADENCE_SENSOR",
        .text = "Cycling Speed and Cadence Sensor"
    }, {.id = GapAdvertisingData::PULSE_OXIMETER_GENERIC,
        .key = "PULSE_OXIMETER_GENERIC",
        .text = "Generic Pulse Oximeter"
    }, {.id = GapAdvertisingData::PULSE_OXIMETER_FINGERTIP,
        .key = "PULSE_OXIMETER_FINGERTIP",
        .text = "Fingertip Pulse Oximeter"
    }, {.id = GapAdvertisingData::PULSE_OXIMETER_WRIST_WORN,
        .key = "PULSE_OXIMETER_WRIST_WORN",
        .text = "Wrist Worn Pulse Oximeter"
    }, {.id = GapAdvertisingData::OUTDOOR_GENERIC,
        .key = "OUTDOOR_GENERIC",
        .text = "Generic Outdoor"
    }, {.id = GapAdvertisingData::OUTDOOR_LOCATION_DISPLAY_DEVICE,
        .key = "OUTDOOR_LOCATION_DISPLAY_DEVICE",
        .text = "Outdoor Location Display Device"
    }, {.id = GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE,
        .key = "OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE",
        .text = "Outdoor Location and Navigation Display Device"
    }, {.id = GapAdvertisingData::OUTDOOR_LOCATION_POD,
        .key = "OUTDOOR_LOCATION_POD",
        .text = "Outdoor Location Pod"
    }, {.id = GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_POD,
        .key = "OUTDOOR_LOCATION_AND_NAVIGATION_POD",
        .text = "Location And Navigarion POD"
    },
    { .id = 0, .key = 0, .text = 0 }
};

const char* get_appearance_text(GapAdvertisingData::Appearance appearance)
{
    apperance_map_s* ptr=APPEARANCES;
    while( ptr->key ) {
        if( ptr->id == appearance ) {
            return ptr->text;
        }
        ptr++;
    }
    return "UNKNOWN";
}

GapAdvertisingData::Appearance get_appearance_id( char* appearance )
{
    apperance_map_s* ptr=APPEARANCES;
    while( ptr->text ) {
        if( strcmp(ptr->text, appearance ) == 0 ||
            strcmp(ptr->key, appearance ) == 0 ) {
            return ptr->id;
        }
        ptr++;
    }
    return GapAdvertisingData::UNKNOWN;
}

int cmd_ifconfig_print(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
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
        cmd_printf("  Appearance: %s", get_appearance_text(appearance) );
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
    if( cmd_parameter_index(argc, argv, (char*)"reset" ) == 1 ) {
        cmd_ifconfig_ble_reset();
    } else if( cmd_parameter_index(argc, argv, (char*)"up" ) == 1 ) {
        ret = ble.init();
    } else if( cmd_parameter_index(argc, argv, (char*)"down" ) == 1 ) {
        ble.shutdown();
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, (char*)"--set-dev-name", &val) ) {
        ble.gap().setDeviceName((uint8_t*)val);
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, (char*)"--addr", &val) ){
        Gap::Address_t address;
        // @todo convert string to address..
        // ...
        ret = ble.gap().setAddress(Gap::ADDR_TYPE_PUBLIC, address);
    } else if( cmd_parameter_int(argc, argv, (char*)"--adv-timeout", &ival)) {
        ble.gap().setAdvertisingTimeout(ival); /* in milliseconds. */
        ret = 0;
    } else if( cmd_parameter_int(argc, argv, (char*)"--adv-interval", &ival)) {
        ble.gap().setAdvertisingInterval(ival); /* in milliseconds. */
        ret = 0;
    } else if( cmd_parameter_index(argc, argv, (char*)"start-adv")>0) {
        ASSERT_NO_FAILURE(ble.gap().startAdvertising());
        ret = 0;
    } else if( cmd_parameter_index(argc, argv, (char*)"clear-adv-payload")>0) {
        ble.gap().clearAdvertisingPayload();
        ret = 0;
    } else  if( cmd_parameter_int(argc, argv, (char*)"--acc-adv-tx-pwr", &ival)) {
        ble.gap().accumulateAdvertisingPayloadTxPower(ival);
        ret = 0;
    } else if( cmd_parameter_val(argc, argv, (char*)"--acc-adv-payload", &val) ){
        uint8_t mask = 0;
        if( strstr(val, (char*)"LE_GENERAL_DISCOVERABLE")>NULL) {
            mask |= GapAdvertisingData::LE_GENERAL_DISCOVERABLE;
        }
        if( strstr(val, (char*)"BREDR_NOT_SUPPORTED") > NULL) {
            mask |= GapAdvertisingData::BREDR_NOT_SUPPORTED;
        }
        if( strstr(val, (char*)"SERVICE_DATA")>NULL) {
            mask |= GapAdvertisingData::SERVICE_DATA;
            if( cmd_parameter_val(argc, argv, (char*)"--payload", &val) ){
                //const static uint8_t trivialAdvPayload[] = {123, 123, 123, 123, 123};
                //@todo convert val to array and overwrite above array
                //ASSERT_NO_FAILURE(ble.gap().accumulateAdvertisingPayload(mask, trivialAdvPayload, sizeof(trivialAdvPayload)));
                //return CMDLINE_RETCODE_SUCCESS;
                return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
            }
        }
        ble.gap().accumulateAdvertisingPayload( mask );
    } else if( cmd_parameter_val(argc, argv, (char*)"--scan-params", &val) ) {
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
    } else if( cmd_parameter_val(argc, argv, (char*)"--set-appearance", &val) ) {
        GapAdvertisingData::Appearance appearance;
        appearance = get_appearance_id(val);
        tr_debug("Set appearance: %s", get_appearance_text(appearance));
        ASSERT_NO_FAILURE(ble.gap().setAppearance(appearance));
    }  else if( cmd_parameter_val(argc, argv, (char*)"--set-preferred-conn-params", &val) ) {
        char *ptr = val;
        Gap::ConnectionParams_t params =  {50, 500, 0, 500};
        for(int i=0;i<4; i++) {
            if( *ptr && ptr[1] != 0 ) {
                if (*ptr == ',') {
                    ++ptr;
                }
                unsigned value = strtoul(ptr, &ptr, 10);
                switch(i) {
                    case(0): params.minConnectionInterval = value; break;
                    case(1): params.maxConnectionInterval = value; break;
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

