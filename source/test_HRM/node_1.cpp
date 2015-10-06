/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /*
 * This test-group presents a command interpreter over the serial console to
 * allow various sub-tests to be executed. It is meant to be driven by a script,
 * but a user can also directly interact with it. It relies upon a useful
 * implementation for RawSerial on the host platform; more specifically, it
 * assumes that it is possible to attach handlers for input characters.
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "ble/services/DeviceInformationService.h"
#include "LEDService.h"
#include "ButtonService.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "../commands.h"

/**
 * Global static objects.
 */
static BLE *ble_ptr;
static ButtonService *btnServicePtr;

static int init_HRM_test(void);
static int setDeviceNameTest();
static int appearanceTest();
static int connParamTest();
static int notificationTest();

static const char     DEVICE_NAME[] = "HRMTEST";
static const uint16_t uuid16_list[] = {GattService::UUID_HEART_RATE_SERVICE,
                                       GattService::UUID_DEVICE_INFORMATION_SERVICE,
                                       LEDService::LED_SERVICE_UUID};

/**
 */
int cmd_test_HRM_node1(int argc, char* argv[], BLE *ble)
{
    ble_ptr = ble;
    
    //ASSERT_NO_FAILURE(  );
    init_HRM_test();
    
    if( cmd_parameter_index(argc, argv, "setDeviceName") > 0 )
    {
        return setDeviceNameTest();
    } else if( cmd_parameter_index(argc, argv, "appearance") > 0 )
    {
        return appearanceTest();
    }  else if( cmd_parameter_index(argc, argv, "connParam") > 0 )
    {
        return connParamTest();
    } else if( cmd_parameter_index(argc, argv, "notification") > 0 )
    {
        return notificationTest();
    }
    return CMDLINE_RETCODE_INVALID_PARAMETERS;
}
                                       
                                       
/**
 * Restarts advertising
 */
static void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    cmd_printf("Disconnected\r\n");
    ble_ptr->gap().startAdvertising(); // restart advertising
}

/**
 * When connected prints the bluetooth MAC address of the device connected to
 */
static void connectionCallback(const Gap::ConnectionCallbackParams_t *params){
    cmd_printf("Connected to: %d:%d:%d:%d:%d:%d\n",
           params->peerAddr[0], params->peerAddr[1], params->peerAddr[2], params->peerAddr[3], params->peerAddr[4], params->peerAddr[5]);
}

/**
 * Tests the set and get Device Name functions
 */
static int setDeviceNameTest()
{
    if (ble_ptr->gap().getState().connected) {
        cmd_printf("Device must be disconnected\n");
        return 100;
    }

    uint8_t deviceNameIn[] = "Josh-test";
    ASSERT_NO_FAILURE(ble_ptr->gap().setDeviceName(deviceNameIn));
    wait(0.5);  /* TODO: remove this. */

    static const size_t MAX_DEVICE_NAME_LEN = 50;
    uint8_t  deviceName[MAX_DEVICE_NAME_LEN];
    unsigned length = MAX_DEVICE_NAME_LEN;
    ASSERT_NO_FAILURE(ble_ptr->gap().getDeviceName(deviceName, &length));

    cmd_printf("ASSERTIONS DONE\r\n");

    for (unsigned i = 0; i < length; i++) {
        cmd_printf("%c", deviceName[i]);
    }
    cmd_printf("\r\n");
    for (unsigned i = 0; i < strlen((char *)deviceNameIn); i++) {
        cmd_printf("%c", deviceNameIn[i]);
    }
    cmd_printf("\r\n");
    return 0;
}

/**
 * Tests the set and get Apeparance functions
 */
static int appearanceTest()
{
    if ((ble_ptr->gap().getState().connected)) {
        cmd_printf("Device must be disconnected\n");
        return 100;
    }

    ASSERT_NO_FAILURE(ble_ptr->gap().setAppearance(GapAdvertisingData::GENERIC_PHONE));
    GapAdvertisingData::Appearance appearance;
    ASSERT_NO_FAILURE(ble_ptr->gap().getAppearance(&appearance));
    cmd_printf("ASSERTIONS DONE\r\n");

    cmd_printf("%d\r\n", appearance);
    return 0;
}

/**
 * Tests the get and set Preferred Connection Params functions
 */
static int connParamTest()
{
    if ((ble_ptr->gap().getState().connected)) {
        cmd_printf("Device must be disconnected\n");
        return 100;
    }

    Gap::ConnectionParams_t originalParams;
    ASSERT_NO_FAILURE(ble_ptr->gap().getPreferredConnectionParams(&originalParams));

    Gap::ConnectionParams_t params;
    Gap::ConnectionParams_t paramsOut = {50, 500, 0, 500};
    ASSERT_NO_FAILURE(ble_ptr->gap().setPreferredConnectionParams(&paramsOut));

    cmd_printf("ASSERTIONS DONE\r\n");

    ble_ptr->gap().getPreferredConnectionParams(&params);

    cmd_printf("%d\n", params.minConnectionInterval);
    cmd_printf("%d\n", params.maxConnectionInterval);
    cmd_printf("%d\n", params.slaveLatency);
    cmd_printf("%d\n", params.connectionSupervisionTimeout);

    ble_ptr->gap().setPreferredConnectionParams(&originalParams);
    return 0;
}

/**
 * Changes button characteristic to be detected the B device for the notification test
 */
static int notificationTest(void) {
    btnServicePtr->updateButtonState(true);
    return 0;
}

/**
 * @return 0 if basic assumptions are validated. Non-zero returns are used to
 *     terminate the second-level python script early.
 */
static int verifyBasicAssumptions()
{
    ble_ptr->gap().onDisconnection(disconnectionCallback);
    ble_ptr->gap().onConnection(connectionCallback);

    /* Setup advertising. */
    if (ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE)) {
        return 1;
    }
    if (ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list))) {
        return 1;
    }
    if (ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR)) {
        return 1;
    }
    if (ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME))) {
        return 1;
    }
    ble_ptr->gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble_ptr->gap().setAdvertisingInterval(1000); /* 1000ms */

    if (ble_ptr->gap().startAdvertising()) {
        return 1;
    }

    const char *version = ble_ptr->getVersion();
    cmd_printf("%s\r\n", version);
    if (!strcmp(version, "")) return 1;
    return 0;
}

static int init_HRM_test(void)
{

    int errorCode = ble_ptr->init();
    if (errorCode == 0) {
        uint8_t                   hrmCounter = 100; // init HRM to 100bps
        HeartRateService         *hrService  = new HeartRateService(*ble_ptr, hrmCounter, HeartRateService::LOCATION_FINGER);

        bool                      initialValueForLEDCharacteristic = false;
        LEDService               *ledService                       = new LEDService(*ble_ptr, initialValueForLEDCharacteristic);

        btnServicePtr = new ButtonService(*ble_ptr, false);
    }
    errorCode |= verifyBasicAssumptions();
    return errorCode;
}
