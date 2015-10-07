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
#include "ble/DiscoveredCharacteristic.h"
#include "ble/DiscoveredService.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "../commands.h"

/**
 * Global static objects.
 */
static BLE *ble_ptr;
static Gap::Address_t address;

static int init_HRM_test();
static int connectTest(Gap::Address_t address);
static int disconnectTest();
static int readTest();
static int writeTest();
static int notificationTest();

static DiscoveredCharacteristic* HRMCharacteristic;
static DiscoveredCharacteristic* LEDCharacteristic;
static DiscoveredCharacteristic* BTNCharacteristic;
static bool HRMFound = false;
static bool LEDFound = false;
static bool BTNFound = false;
static Gap::Handle_t deviceAHandle;

int cmd_test_HRM_node2(int argc, char* argv[], BLE *ble)
{
    ble_ptr = ble;
    char *val;
    init_HRM_test();

    if( cmd_parameter_index(argc, argv, "connect") > 0 )
    {
        if( cmd_parameter_val(argc, argv, "connect", &val) ) {
            Gap::Address_t address;
            char *ptr = val;
            Gap::ConnectionParams_t params =  {0, 0, 0, 0};
            for(int i=0;i<6; i++) {
                if( *ptr && ptr[1] != 0 ) {
                    if (*ptr == ',' || *ptr == ':') {
                        ++ptr;
                    }
                    address[i] = strtoul(ptr, &ptr, 16);
                }
            }
            return connectTest(address);
        }
    } else if( cmd_parameter_index(argc, argv, "disconnect") > 0 )
    {
        return disconnectTest();
    } else if( cmd_parameter_index(argc, argv, "read") > 0 )
    {
        return readTest();
    } else if( cmd_parameter_index(argc, argv, "write") > 0 )
    {
        return writeTest();
    } else if( cmd_parameter_index(argc, argv, "notification") > 0 )
    {
        return notificationTest();
    }
    return CMDLINE_RETCODE_INVALID_PARAMETERS;
}

static void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    cmd_printf("Disconnected\r\n");
}

/*
 * Call back when a service is discovered
 */
static void serviceDiscoveryCallback(const DiscoveredService *service)
{
    if (service->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
        cmd_printf("S UUID-%x attrs[%u %u]\r\n", service->getUUID().getShortUUID(), service->getStartHandle(), service->getEndHandle());
    } else {
        cmd_printf("S UUID-");
        const uint8_t *longUUIDBytes = service->getUUID().getBaseUUID();
        for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
            cmd_printf("%02x", longUUIDBytes[i]);
        }
        cmd_printf("attrs[%u %u]\r\n", service->getStartHandle(), service->getEndHandle());
    }
}

/*
 * Call back when a characteristic is discovered
 */
static void characteristicDiscoveryCallback(const DiscoveredCharacteristic *characteristicP)
{
    if (characteristicP->getUUID().getShortUUID() == 0x2a37) { /* Searches for HRM Characteristic*/
        HRMCharacteristic = new DiscoveredCharacteristic(*characteristicP);
        HRMFound          = true;
    }
    if (characteristicP->getUUID().getShortUUID() == 0xA001) { /* Searches for LED Characteristic*/
        LEDCharacteristic = new DiscoveredCharacteristic(*characteristicP);
        LEDFound          = true;
    }
    if (characteristicP->getUUID().getShortUUID() == 0xA003) {
        BTNCharacteristic = new DiscoveredCharacteristic(*characteristicP);
        BTNFound          = true;
    }
}

/*
 * Call back when device is connected
 */
static void connectionCallback(const Gap::ConnectionCallbackParams_t *params)
{
    cmd_printf("Connected to: %2X:%2X:%2X:%2X:%2X:%2X\r\n",
           params->peerAddr[0], params->peerAddr[1], params->peerAddr[2], params->peerAddr[3], params->peerAddr[4], params->peerAddr[5]);
    if (params->role == Gap::CENTRAL) {
        deviceAHandle = params->handle; /* Handle for device A so it is it possible to disconnect*/
        ASSERT_NO_FAILURE_VOID(ble_ptr->gattClient().launchServiceDiscovery(params->handle, serviceDiscoveryCallback, characteristicDiscoveryCallback));
    }
}

/*
 * The callback for reading a characteristic, print depends on what characteristic is read
 */
static void readCharacteristic(const GattReadCallbackParams *response)
{
    if (response->handle == HRMCharacteristic->getValueHandle()) {
        cmd_printf("HRMCounter: %d\n",  response->data[1]);
    }
    if (response->handle == LEDCharacteristic->getValueHandle()) {
        cmd_printf("LED: %d\n", response->data[0]);
    }
    if (response->handle == BTNCharacteristic->getValueHandle()) {
        cmd_printf("BTN: %d\n", response->data[0]);
    }
}

/*
 * Tests connecting devices. Devices must be disconnected for this test
 */
static int connectTest(Gap::Address_t address)
{
    if (!(ble_ptr->gap().getState().connected)) {
        ASSERT_NO_FAILURE(ble_ptr->gap().connect(address, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL));
    } else {
        cmd_printf("Devices already connected\n");
    }
    return 0;
}

/*
 * Tests reading from to the heart rate characteristic. Devices need to be connected for this test.
 */
static int readTest(){
    if (!(ble_ptr->gap().getState().connected)) {
        cmd_printf("Devices must be connected before this test can be run\n");
        return 100;
    }
    if (HRMFound) {
        ASSERT_NO_FAILURE(HRMCharacteristic->read());
    } else {
        cmd_printf("Characteristic not found\r\n");
    }
    return 0;
}

/**
 * Tests writing to the LED Characteristic. Then reads from the callback to verify that the write is correct.
 * Devices need to be connected for this test.
 */
static int writeTest()
{
    if (!(ble_ptr->gap().getState().connected)) {
        cmd_printf("Devices must be connected before this test can be run\n");
        return 100;
    }
    if (LEDFound) {
        uint8_t write_value = 1;
        ASSERT_NO_FAILURE(LEDCharacteristic->write(sizeof(write_value), &write_value)); /* When write finishes, writeCallback is called */
    } else {
        cmd_printf("Characeristic not found\r\n");
    }
    return 0;
}

/**
 * Tests disconnecting devices. If it is already connected it prints a message
 */
static int disconnectTest()
{
    if ((ble_ptr->gap().getState().connected)) {
        ASSERT_NO_FAILURE(ble_ptr->gap().disconnect(deviceAHandle, Gap::REMOTE_USER_TERMINATED_CONNECTION));
    } else {
        cmd_printf("Devices not connected\n");
    }
    return 0;
}

static int notificationTest()
{
    if (!ble_ptr->gap().getState().connected) {
        cmd_printf("Devices must be connected before this test can be run\n");
        return 100;
    }
    if (BTNFound) {
        uint16_t value = BLE_HVX_NOTIFICATION;
        ASSERT_NO_FAILURE(ble_ptr->gattClient().write(GattClient::GATT_OP_WRITE_REQ,
                                   deviceAHandle,
                                   BTNCharacteristic->getValueHandle() + 1, /* HACK Alert. We're assuming that CCCD descriptor immediately follows the value attribute. */
                                   sizeof(uint16_t),                        /* HACK Alert! size should be made into a BLE_API constant. */
                                   reinterpret_cast<const uint8_t *>(&value)));
    } else {
        cmd_printf("Characteristic not found\r\n");
    }
    return 0;
}

/**
 * Call back for writing to LED characteristic.
 */
static void writeCallback(const GattWriteCallbackParams *params)
{
    if (params->handle == LEDCharacteristic->getValueHandle()) {
        ASSERT_NO_FAILURE_VOID(LEDCharacteristic->read());
    } else if (params->handle == BTNCharacteristic->getValueHandle() + 1) {
        cmd_printf("Sync\r\n");
    }
}

static void hvxCallback(const GattHVXCallbackParams *params) {
    cmd_printf("Button: ");
    for (unsigned index = 0; index < params->len; index++) {
        cmd_printf("%02x", params->data[index]);
    }
    cmd_printf("\r\n");
}

static int init_HRM_test()
{
    /*
    unsigned x;
    for (unsigned i = 0; i < Gap::ADDR_LEN; i++) {
        scanf("%d", &x);
        address[i] = (uint8_t)x;
    }
    */
    ASSERT_NO_FAILURE(ble_ptr->init());
    ASSERT_NO_FAILURE(ble_ptr->gap().setScanParams(500 /* scan interval */, 200 /* scan window */));

    ble_ptr->gap().onConnection(connectionCallback);
    ble_ptr->gap().onDisconnection(disconnectionCallback);
    ble_ptr->gattClient().onDataRead(readCharacteristic);
    ble_ptr->gattClient().onDataWrite(writeCallback);
    ble_ptr->gattClient().onHVX(hvxCallback);
    return 0;
}
