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
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "../commands.h"


/**
 * Global static objects.
 */
BLE *ble_ptr;
int shutdownTest(void);
int setTimeoutTest(void);
int responseTest(void);
int changePayloadTest(void);
int changeIntervalTest(void);
int setAddrTest(void);
int setupIBeaconTest(void);


/**
 * Returns a pointer to the test function wanting to run. Sets up a table which maps strings to functions.
 */
int cmd_test_A(int argc, char* argv[], BLE *ble)
{
    ble_ptr = ble;
    if( cmd_parameter_index(argc, argv, "setAddr") > 0 )
    {
        return setAddrTest();
    } else if( cmd_parameter_index(argc, argv, "changeIntervalTest") > 0 )
    {
        return changeIntervalTest();
    } else if( cmd_parameter_index(argc, argv, "changePayloadTest") > 0 )
    {
        return changePayloadTest();
    } else if( cmd_parameter_index(argc, argv, "setTimeoutTest") > 0 )
    {
        return setTimeoutTest();
    } else if( cmd_parameter_index(argc, argv, "responseTest") > 0 )
    {
        return responseTest();
    } else if( cmd_parameter_index(argc, argv, "shutdownTest") > 0 )
    {
        return shutdownTest();
    } else if( cmd_parameter_index(argc, argv, "setupIBeaconTest") > 0 )
    {
        return setupIBeaconTest();
    } else {
        return CMDLINE_RETCODE_INVALID_PARAMETERS;
    }
}

/**
 * Test for advertising using an iBeacon
 */
int setupIBeaconTest(void)
{
    /* setup the iBeacon */
    const static uint8_t uuid[] = {0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                                   0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
    uint16_t majorNumber = 1122;
    uint16_t minorNumber = 3344;
    uint16_t txPower     = 0xC8;
    iBeacon ibeacon(*ble_ptr, uuid, majorNumber, minorNumber, txPower);

    uint16_t interval_value = 1000;
    ble_ptr->gap().setAdvertisingInterval(interval_value); /* 1000ms. */
    CHECK_EQUALS(ble_ptr->gap().getAdvertisingParams().getInterval(), interval_value);

    ble_ptr->gap().setAdvertisingTimeout(0);
    CHECK_EQUALS(ble_ptr->gap().getAdvertisingParams().getTimeout(), 0);

    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS; 
}

/**
 * Test for setting and getting MAC address
 */
int setAddrTest(void)
{
    Gap::AddressType_t addressType;
    Gap::Address_t     origAddress;
    ble_ptr->gap().getAddress(&addressType, origAddress);

    Gap::Address_t newAddress = {110, 100, 100, 100, 100, 100}; /* A randomly chosen address for assigning to the peripheral. */
    ASSERT_NO_FAILURE(ble_ptr->gap().setAddress(Gap::ADDR_TYPE_PUBLIC, newAddress));

    Gap::Address_t fetchedAddress;
    ASSERT_NO_FAILURE(ble_ptr->gap().getAddress(&addressType, fetchedAddress));

    cmd_printf("ASSERTIONS DONE\r\n");
    cmd_printf("%s\r\n", cmd_print_address(newAddress));
    cmd_printf("%s\r\n", cmd_print_address(fetchedAddress));

    ble_ptr->gap().setAddress(Gap::ADDR_TYPE_PUBLIC, origAddress);
    return CMDLINE_RETCODE_SUCCESS;
}

/**
 * Test to change advertisement interval
 */
int changeIntervalTest(void)
{
    ble_ptr->gap().setAdvertisingTimeout(0);
    ble_ptr->gap().setAdvertisingInterval(500); /* in milliseconds. */
    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS;
}

/**
 * Test to change advertisement payload
 */
int changePayloadTest(void)
{
    ble_ptr->gap().clearAdvertisingPayload();
    ble_ptr->gap().setAdvertisingTimeout(0);

    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::LE_GENERAL_DISCOVERABLE | GapAdvertisingData::BREDR_NOT_SUPPORTED));
    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::OUTDOOR_GENERIC));
    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateAdvertisingPayloadTxPower(10)); /* in dbm. */

    const static uint8_t trivialAdvPayload[] = {123, 123, 123, 123, 123};
    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, trivialAdvPayload, sizeof(trivialAdvPayload)));

    ble_ptr->gap().setAdvertisingInterval(500); /* in milliseconds. */
    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS;
}

/**
 * Test to change add a scan response
 */
int responseTest(void)
{
    ble_ptr->gap().clearAdvertisingPayload();
    ble_ptr->gap().clearScanResponse();
    ble_ptr->gap().setAdvertisingTimeout(0);
    ble_ptr->setAdvertisingType(GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED);

    const static uint8_t trivialAdvPayload[] = {50, 50, 50, 50, 50};
    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, trivialAdvPayload, sizeof(trivialAdvPayload)));

    const static uint8_t trivialScanResponse[] = {50, 50, 50, 50, 50};
    ASSERT_NO_FAILURE(ble_ptr->gap().accumulateScanResponse(GapAdvertisingData::SERVICE_DATA, trivialScanResponse, sizeof(trivialScanResponse)));

    ble_ptr->gap().setAdvertisingInterval(500); /* in  units of milliseconds. */
    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS;
}

/**
 * Test to change advertisement timeout.
 */
int setTimeoutTest(void)
{
    ble_ptr->gap().clearAdvertisingPayload();
    ble_ptr->gap().clearScanResponse();

    ble_ptr->gap().setAdvertisingTimeout(5); /* 5 seconds */
    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS;
}

/**
 * Test of the ble shutdown function. Reinitializes and makes sure it
 */
int shutdownTest(void)
{
    ASSERT_NO_FAILURE(ble_ptr->shutdown());
    ASSERT_NO_FAILURE(ble_ptr->init());
    ASSERT_NO_FAILURE(ble_ptr->gap().startAdvertising());
    cmd_printf("ASSERTIONS DONE\r\n");
    return CMDLINE_RETCODE_SUCCESS;
}
