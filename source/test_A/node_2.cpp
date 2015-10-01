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

/* This is the Central side of the beacon test. */

#include "mbed.h"
#include "ble/BLE.h"

#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "../commands.h"

/**
 * static objects.
 */
static BLE *ble_ptr;
static Gap::Address_t peerAddress;

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params);

/* */
int cmd_test_A_node2(int argc, char* argv[], BLE *ble)
{
    ble_ptr = ble;
    char* str;
    if( cmd_parameter_val(argc, argv, "--peer-addr", &str) > 0 )
    {
        //get peer-address
        char *ptr = str;
        for (unsigned i = 0; i < Gap::ADDR_LEN; i++) {
            int x;
            x = strtoul(ptr, &ptr, 10);
            peerAddress[i] = (uint8_t)x;
            if( !ptr || !*ptr || !ptr[1] ) {
                return CMDLINE_RETCODE_INVALID_PARAMETERS;
            }
        }
    } else {
        cmd_printf("require --peer-addr <addr> option!\r\n");
        return CMDLINE_RETCODE_INVALID_PARAMETERS;
    }
    
    ASSERT_NO_FAILURE(ble_ptr->gap().setScanParams(500/* scan interval*/, 200/* scan window*/,0,true/*active scanning*/));
    ASSERT_NO_FAILURE(ble_ptr->gap().startScan(advertisementCallback));
    return CMDLINE_RETCODE_SUCCESS;
}

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
    for (size_t i = 0; i < Gap::ADDR_LEN; i++) {
        if (peerAddress[i] != params->peerAddr[i]) {
            return; /* Filter away adverts all adverts except from the targeted peer. */
        }
    }

    /* emit the last byte of the advert payload. */
    cmd_printf("ScanResp: %u, Data: ", params->isScanResponse, *(params->advertisingData + params->advertisingDataLen - 1));
    for (unsigned index = 0; index < params->advertisingDataLen; index++) {
        cmd_printf("%u ", params->advertisingData[index]);
    }
    cmd_printf("\r\n");
}
