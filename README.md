# mbed BLE command line application

This application is a shell for mbed BLE: via a terminal, a user can call most of the functions available in mbed BLE, the device execute it and send back the result.
Even if the tool was initially created to run system tests of mbed BLE, it can also be used to debug an application or a port of mbed BLE. 

## Getting started 

This application can be compiled for mbed OS 2, 3 and 5. 

* mbed OS 2, 5: Use [mbed-cli](https://github.com/ARMmbed/mbed-cli) to compile the application. 
* mbed OS 3: Use [yotta](http://yottadocs.mbed.com/#installing) to compile the application. 

Once the application is compiled, load it onto your board and open a terminal to your board. The baudrate should be set to 115.200 baud. 

A good way to start with the application is to create a simple beacon from the shell commands: 

* `ble init`: This will initialize BLE API. In c++ this would translate to: 

```c++
BLE& ble = BLE::Instance()
ble.init();
```

* `gap accumulateAdvertisingPayload COMPLETE_LOCAL_NAME FOO_BAR_BAZ`: add a complete local name field in the advertisement payload. It is equivalent to:

```c++
const char device_name[] = "FOO_BAR_BAZ";
Gap& gap = ble.gap();
gap.accumulateAdvertisingPayload(
  GapAdvertisingData::COMPLETE_LOCAL_NAME,
  device_name, sizeof(device_name)
);
```
 
* `gap startAdvertising`: Ask the device to start advertisements. 

```c++ 
gap.startAdvertising()
```

At this point, the device should advertise itself with the name *FOO_BAR_BAZ*.


# Commands

Invoking a command is simple: 

`<module> <command> <arguments>`

Like mbed BLE, the shell is divided in modules which contain a set of commands, the following modules are accessible: 

* `ble`: Model the class `BLE`
* `gap`: Model the class `Gap`
* `gattServer`: Model the class `GattServer`
* `gattClient`: Model the class `GattClient`
* `securityManager`: Model the class `SecurityManager`

When the command as completed, the result is printed on the terminal, in a `json` dictionary. This dictionary contains the following keys: 

* `name`: name of the command invoked
* `args`: array of arguments passed to the function
* `status`: return status, if the result is equal to 0 then the command has been correctly executed. A negative value means that the command has failed. 
* `error`: This key will be present only if the command execution has failed. The value associated is a `json` object (generally a string).
* `result`: This key is present if the command has succeeded and there is a meaningful result to report to the user. The value associated is a `json` object, its format depend on the command invoked.

## ble module

The `ble` module expose the following functions from the class `BLE`:

### init
* invocation: `ble init`
* arguments: None 
* result: None
* modeled after: `BLE::init`

### shutdown
* invocation: `ble shutdown`
* modeled after: `BLE::shutdown`
* arguments: None 
* result: None

### reset
Reset the ble stack. If the stack is initialized, it will shut it down prior to initializing it again.

* invocation: `ble reset`
* arguments: None 
* result: None

### getVersion
* invocation: `ble getVersion`
* modeled after: `BLE::getVersion`
* arguments: None 
* result: The version of the underlying stack as a string.


## gap module

The `gap` module expose the following functions from the class `Gap`:

### setAddress
* invocation: `gap setAddress <address_type> <MAC address>`
* arguments: 
  - `[AddressType](#addresstype)` address_type: The type of the address to set.
  - `[MacAddress](#macaddress)` MAC address: The mac address to set.
* result: None
* modeled after: `Gap::setAddress`

### getAddress
* invocation: `gap getAddress`
* arguments: None 
* result: `json` dictionary with the following attributes:
  - `[AddressType](#addresstype)` address_type: The type of the address set.
  - `[MacAddress](#macaddress)` MAC address: The mac address set.
* modeled after: `Gap::getAddress`

### getMinAdvertisingInterval
* invocation: `gap getMinAdvertisingInterval`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the minimum advertising interval
* modeled after: `Gap::getMinAdvertisingInterval`

### getMinNonConnectableAdvertisingInterval
* invocation: `gap getMinNonConnectableAdvertisingInterval`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the minimum non connectable advertising interval
* modeled after: `Gap::getMinNonConnectableAdvertisingInterval`

### getMaxAdvertisingIntervalCommand
* invocation: `gap getMaxAdvertisingIntervalCommand`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the maximum advertising interval
* modeled after: `Gap::GetMaxAdvertisingIntervalCommand`

### stopAdvertising
* invocation: `gap stopAdvertising`
* arguments: None 
* result: None
* modeled after: `Gap::stopAdvertising`

### stopScan
* invocation: `gap stopScan`
* arguments: None 
* result: None
* modeled after: `Gap::stopScan`

### connect
* invocation: `gap connect <address_type> <address> <min_connection_interval> <max_connection_interval> <slave_latency> <connection_supervision_timeout> <interval> <window> <scan_timeout> <active scanning> <command_timeout>`
* arguments:
  - [`AddressType`](#addresstype) address_type: The address type of the peer device.
  - [`MacAddress`](#macaddress) address: The mac address of the peer.
  - connection parameters: The next four arguments are the connections parameters:
    + [`uint16_t`](#uint16_t) min_connection_interval: Minimum Connection Interval in 1.25 ms units
    + [`uint16_t`](#uint16_t) max_connection_interval: Maximum Connection Interval in 1.25 ms units
    + [`uint16_t`](#uint16_t) slave_latency: Slave Latency in number of connection events
    + [`uint16_t`](#uint16_t) connection_supervision_timeout: Connection Supervision Timeout in 10 ms units
  - scan parameters: The next four arguments used as scan parameter during peer search.
    + [`uint16_t`](#uint16_t) interval: The scan interval, it should be a number between 3 and 10420ms.
    + [`uint16_t`](#uint16_t) window: The scan window, it should be a number between 3 and 10420ms.
    + [`uint16_t`](#uint16_t) scan timeout: The scan timeout, it should be a number between 0 and 65534
    + [`bool`](#bool) activeScanning: A boolean value indicating if the device send scan request or not
  - [`uint16_t`](#uint16_t) timeout: Maximum time allowed for this procedure; in milliseconds.
* result: A `json` dictionary containing the following attributes: 
  - [`uint16_t`](#uint16_t) handle: The connection handle
  - [`GapRole`](#gaprole) role: Role of the device in the connection (here, it should be central)
  - [`AddressType`](#addresstype) peerAddrType: The addressType of the peer
  - [`MacAddress`](#macaddress) peerAddr: The address of the peer
  - [`AddressType`](#addresstype) ownAddrType: The address type of this device
  - [`MacAddress`](#macaddress) ownAddr: The address of this device
  - `JSONDictionary` connectionParams: Contain the parameters of the connection:
    + [`uint16_t`](#uint16_t) minConnectionInterval: minimum connection interval for this connection
    + [`uint16_t`](#uint16_t) maxConnectionInterval: maximum connection interval for this connection
    + [`uint16_t`](#uint16_t) slaveLatency: slave latency of the connection
    + [`uint16_t`](#uint16_t) connectionSupervisionTimeout: supervision timeout for this connection
* modeled after: `Gap::connect` and `Gap::onConnection`.


### waitForConnection
This function instruct the device to wait for a connection and report the connection parameters once it is connected.

* invocation: `gap waitForConnection <command_timeout>`
* argument: [`uint16_t`](#uint16_t) timeout: Maximum time allowed for this command; in milliseconds.
* result: A `json` dictionary identical to the one returned by [connect](#connect). It contains the following attributes : 
  - [`uint16_t`](#uint16_t) handle: The connection handle
  - [`GapRole`](#gaprole) role: Role of the device in the connection (here, it should be central)
  - [`AddressType`](#addresstype) peerAddrType: The addressType of the peer
  - [`MacAddress`](#macaddress) peerAddr: The address of the peer
  - [`AddressType`](#addresstype) ownAddrType: The address type of this device
  - [`MacAddress`](#macaddress) ownAddr: The address of this device
  - `JSONDictionary` connectionParams: Contain the parameters of the connection:
    + [`uint16_t`](#uint16_t) minConnectionInterval: minimum connection interval for this connection
    + [`uint16_t`](#uint16_t) maxConnectionInterval: maximum connection interval for this connection
    + [`uint16_t`](#uint16_t) slaveLatency: slave latency of the connection
    + [`uint16_t`](#uint16_t) connectionSupervisionTimeout: supervision timeout for this connection
* modeled after: `Gap::onConnection` and `Gap::onConnection`.


### disconnect

* invocation: `gap disconnect <connection_handle> <reason>`
* arguments: 
  - [`uint16_t`](#uint16_t) handle: The id of the connection to terminate.
  - [`DisconnectionReason`](#DisconnectionReason) reason: The reason of the termination.
* result: A `json` dictionary containing the following attributes: 
  - [`uint16_t`](#uint16_t) handle: The id of the connection terminated.
  - [`DisconnectionReason`](#DisconnectionReason) reason: The reason of the termination.
* modeled after: `Gap::disconnect` and `Gap::onDisconnection`.


### getPreferredConnectionParams

* invocation: `gap getPreferredConnectionParams`
* arguments: None
* result: A `json` dictionary containing the following attributes: 
  - [`uint16_t`](#uint16_t) minConnectionInterval: The preferred minimum connection interval
  - [`uint16_t`](#uint16_t) maxConnectionInterval: The preferred maximum connection interval
  - [`uint16_t`](#uint16_t) slaveLatency: The preferred slave latency 
  - [`uint16_t`](#uint16_t) connectionSupervisionTimeout: The preferred supervision timeout
* modeled after: `Gap::getPreferredConnectionParams`


### setPreferredConnectionParams

* invocation: `gap setPreferredConnectionParams <minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>`
* arguments: All arguments are comma separated without space between them.
  - [`uint16_t`](#uint16_t) minConnectionInterval: The preferred minimum connection interval
  - [`uint16_t`](#uint16_t) maxConnectionInterval: The preferred maximum connection interval
  - [`uint16_t`](#uint16_t) slaveLatency: The preferred slave latency 
  - [`uint16_t`](#uint16_t) connectionSupervisionTimeout: The preferred supervision timeout
* result: None
* modeled after: `Gap::setPreferredConnectionParams`


### setDeviceName

* invocation: `gap setDeviceName <name>`
* arguments: The name as a string (without quote). spaces are not supported.
* result: None
* modeled after: `Gap::setDeviceName`


### getDeviceName

* invocation: `gap getDeviceName`
* arguments: None
* result: The name of the device as a string.
* modeled after: `Gap::getDeviceName`


### setAppearance

* invocation: `gap setAppearance <appearance>`
* arguments: 
  - [`Appearance`](#appearance) appearance: The appearance of the device.
* result: None
* modeled after: `Gap::setAppearance`


### getAppearance

* invocation: `gap getAppearance`
* arguments: None.
* result: The appearance as a string. See [`Appearance`](#appearance) for possible values.
* modeled after: `Gap::getAppearance`


### setTxPower

* invocation: `gap setTxPower <power>`
* arguments: 
  - [`int8_t`](#int8_t) power: The transmission power to set.
* result: None
* modeled after: `Gap::setTxPower`


### getPermittedTxPowerValues

* invocation: `gap getPermittedTxPowerValues`
* arguments: None
* result: Array of the permitted Tx power values. Each element of the array is an [`int8_t`](#int8_t).
* modeled after: `Gap::getPermittedTxPowerValues`


### getState

* invocation: `gap getState`
* arguments: None
* result: The gap state of the device in a json dictionary: 
  - [`bool`](#bool) advertising: true if the device is advertising and false otherwise.
  - [`bool`](#bool) connected: true if the device is connected and false otherwise.
* modeled after: `Gap::getState`


### setAdvertisingType

* invocation: `gap setAdvertisingType <advertising_type>`
* arguments: 
  - [`AdvertisingType`](#advertisingtype) advertising_type: The type of advertising to set.
* result: None
* modeled after: `Gap::setAdvertisingType`


### setAdvertisingInterval

* invocation: `gap setAdvertisingInterval <advertising_interval>`
* arguments: 
  - [`uint16_t`](#uint16_t) advertising_interval: The interval between two advertisements.
* result: None
* modeled after: `Gap::setAdvertisingInterval`


### setAdvertisingTimeout

* invocation: `gap setAdvertisingTimeout <advertising_timeout>`
* arguments: 
  - [`uint16_t`](#uint16_t) advertising_timeout: The duration of advertisements in seconds. The value should be in the range [0: 0x3FFF]. A value of 0 disable the timeout.
* result: None
* modeled after: `Gap::setAdvertisingTimeout`


### startAdvertising

* invocation: `gap startAdvertising`
* arguments: None.
* result: None.
* modeled after: `Gap::startAdvertising`


### clearAdvertisingPayload

* invocation: `gap clearAdvertisingPayload`
* arguments: None.
* result: None.
* modeled after: `Gap::clearAdvertisingPayload`


### accumulateAdvertisingPayload

* invocation: `gap accumulateAdvertisingPayload <field_type> <field_argument>`
* arguments: 
  - [`AdvertisingDataType`](#advertisingdatatype) field_type: The type of field to add to the advertising payload.
  - field_argument: The type of the field argument depend on the type of field to add: 
    + `FLAGS` -> \[[`GapAdvertisingDataFlags`](#gapadvertisingdataflags)\]
    + `INCOMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `COMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `INCOMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `COMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `INCOMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `COMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `SHORTENED_LOCAL_NAME` -> [`String`](#string)
    + `COMPLETE_LOCAL_NAME` -> [`String`](#string)
    + `TX_POWER_LEVEL` -> [`int8_t`](#int8_t)
    + `DEVICE_ID` -> not implemented
    + `SLAVE_CONNECTION_INTERVAL_RANGE`: <minimum_connection_interval> <maximum_connection_interval>
      * [`uint16_t`](#uint16_t) minimum_connection_interval: The minimum connection interval
      * [`uint16_t`](#uint16_t) maximum_connection_interval: The maximum connection interval
    + `SERVICE_DATA` -> not yet implemented
    + `APPEARANCE` -> [`Appearance`](#appearance)
    + `ADVERTISING_INTERVAL` -> [`uint16_t`](#uint16_t)
    + `MANUFACTURER_SPECIFIC_DATA` -> [`HexString`](#hex_string)
* result: None.
* modeled after: `Gap::accumulateAdvertisingPayload`



### updateAdvertisingPayload

* invocation: `gap updateAdvertisingPayload <field_type> <field_argument>`
* arguments: 
  - [`AdvertisingDataType`](#advertisingdatatype) field_type: The type of field to update in the advertising payload.
  - field_argument: The type of the field argument depend on the type of field to add: 
    + `FLAGS` -> \[[`GapAdvertisingDataFlags`](#gapadvertisingdataflags)\]
    + `INCOMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `COMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `INCOMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `COMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `INCOMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `COMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `SHORTENED_LOCAL_NAME` -> [`String`](#string)
    + `COMPLETE_LOCAL_NAME` -> [`String`](#string)
    + `TX_POWER_LEVEL` -> [`int8_t`](#int8_t)
    + `DEVICE_ID` -> not implemented
    + `SLAVE_CONNECTION_INTERVAL_RANGE`: <minimum_connection_interval> <maximum_connection_interval>
      * [`uint16_t`](#uint16_t) minimum_connection_interval: The minimum connection interval
      * [`uint16_t`](#uint16_t) maximum_connection_interval: The maximum connection interval
    + `SERVICE_DATA` -> not yet implemented
    + `APPEARANCE` -> [`Appearance`](#appearance)
    + `ADVERTISING_INTERVAL` -> [`uint16_t`](#uint16_t)
    + `MANUFACTURER_SPECIFIC_DATA` -> [`HexString`](#hex_string)
* result: None.
* modeled after: `Gap::updateAdvertisingPayload`


### getAdvertisingPayload

* invocation: `gap getAdvertisingPayload`
* arguments: None.
* result: [`AdvertisingPayload`](#advertisingpayload).
* modeled after: `Gap::getAdvertisingPayload`


### accumulateScanResponse

* invocation: `gap accumulateScanResponse <field_type> <field_argument>`
* arguments: 
  - [`AdvertisingDataType`](#advertisingdatatype) field_type: The type of field to add to the scan response  payload.
  - field_argument: The type of the field argument depend on the type of field to add: 
    + `FLAGS` -> \[[`GapAdvertisingDataFlags`](#gapadvertisingdataflags)\]
    + `INCOMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `COMPLETE_LIST_16BIT_SERVICE_IDS` -> \[[`uint16_t`](#uint16_t)\]
    + `INCOMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `COMPLETE_LIST_32BIT_SERVICE_IDS` -> \[[`uint32_t`](#uint32_t)\]
    + `INCOMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `COMPLETE_LIST_128BIT_SERVICE_IDS` -> \[[`UUID`](#uuid)\]
    + `SHORTENED_LOCAL_NAME` -> [`String`](#string)
    + `COMPLETE_LOCAL_NAME` -> [`String`](#string)
    + `TX_POWER_LEVEL` -> [`int8_t`](#int8_t)
    + `DEVICE_ID` -> not implemented
    + `SLAVE_CONNECTION_INTERVAL_RANGE`: <minimum_connection_interval> <maximum_connection_interval>
      * [`uint16_t`](#uint16_t) minimum_connection_interval: The minimum connection interval
      * [`uint16_t`](#uint16_t) maximum_connection_interval: The maximum connection interval
    + `SERVICE_DATA` -> not yet implemented
    + `APPEARANCE` -> [`Appearance`](#appearance)
    + `ADVERTISING_INTERVAL` -> [`uint16_t`](#uint16_t)
    + `MANUFACTURER_SPECIFIC_DATA` -> [`HexString`](#hex_string)
* result: None.
* modeled after: `Gap::accumulateScanResponse`


### clearScanResponse

* invocation: `gap clearScanResponse`
* arguments: None.
* result: None.
* modeled after: `Gap::clearScanResponse`


### setScanParam

* invocation: `gap setScanParam <interval> <window> <timeout> <activeScanning>`
* arguments: 
  - [`uint16_t`](#uint16_t) interval: The scan interval, it should be a number between 3 and 10420ms.
  - [`uint16_t`](#uint16_t) window: The scan window, it should be a number between 3 and 10420ms.
  - [`uint16_t`](#uint16_t) timeout: The scan timeout, it should be a number between 0 and 65534.
  - [`bool`](#bool) activeScanning: Enable or disable scan requests.
* result: None.
* modeled after: `Gap::setScanParam`


### setScanInterval

* invocation: `gap setScanInterval <interval> <window> <timeout> <activeScanning>`
* arguments: 
  - [`uint16_t`](#uint16_t) interval: The scan interval, it should be a number between 3 and 10420ms.
* result: None.
* modeled after: `Gap::setScanInterval`


### setScanWindow

* invocation: `gap setScanWindow <window>`
* arguments: 
  - [`uint16_t`](#uint16_t) window: The scan window, it should be a number between 3 and 10420ms.
* result: None.
* modeled after: `Gap::setScanWindow`


### setScanTimeout

* invocation: `gap setScanTimeout <timeout>`
* arguments: 
  - [`uint16_t`](#uint16_t) timeout: The scan timeout, it should be a number between 0 and 65534.
* result: None.
* modeled after: `Gap::setScanTimeout`


### setActiveScanning

* invocation: `gap setActiveScanning <activeScanning>`
* arguments: 
  - [`bool`](#bool) activeScanning: Enable or disable scan requests.
* result: None.
* modeled after: `Gap::setActiveScanning`


### startScan

* invocation: `gap startScan <duration> <target_address>`
* arguments: 
  - [`uint16_t`](#uint16_t) duration: Duration of the scan procedure.
  - [`MacAddress`](#macaddress) target_address: Address of the peer to scan for.
* result: A `JSON` dictionary array of scan records. Each records is a dictionary which contains the following fields: 
  - [`MacAddress`](#macaddress) peerAddr: The address of the peer.
  - [`int8_t`](#int8_t) rssi: The RSSI for this record.
  - [`bool`](#bool) isScanResponse: true if the record is a response to a scan request and false otherwise.
  - [`AdvertisingType`](#advertisingtype) type: The type of advertising.
  - [`AdvertisingPayload`](#advertisingpayload) data: The content of the advertising payload received.
  - [`int32_t`](#int32_t) time: The time at which the record was captured. The time start when the procedure is started.
* modeled after: `Gap::startScan` and `Gap::stopScan`.


### getAdvertisingParams

* invocation: `gap getAdvertisingParams`
* arguments: None.
* result: A `JSON` dictionary with the following fields: 
  - [`uint16_t`](#uint16_t) interval: Advertising interval.
  - [`uint16_t`](#uint16_t) timeout: Advertisement timeout.
  - [`AdvertisingType`](#advertisingtype) type: type of advertising.
* modeled after: `Gap::getAdvertisingParams`


### setAdvertisingParams

* invocation: `gap setAdvertisingParams <advertising_type> <interval> <timeout>`
* arguments: 
  - [`AdvertisingType`](#advertisingtype) advertising_type: type of advertising.
  - [`uint16_t`](#uint16_t) interval: Advertising interval, it should be a number between 0 and 65534".
  - [`uint16_t`](#uint16_t) timeout: Advertisement timeout, it should be a number between 0 and 65534".
* result: None
* modeled after: `Gap::setAdvertisingParams`


### getMaxWhitelistSize

* invocation: `gap getMaxWhitelistSize`
* arguments: None
* result: [`uint8_t`](#uint8_t) The maximum size of the whitelist.
* modeled after: `Gap::getMaxWhitelistSize`


### getWhitelist

* invocation: `gap getWhitelist`
* arguments: None
* result: A JSON array containing the list of devices in the whitelist. Each devices is a JSON dictionary with the following fields: 
  - [`AddressType`](#AddressType) address_type: The type of the address of the device.
  - [`MacAddress`](#MacAddress) address: The mac address of the device.
* modeled after: `Gap::getWhitelist`


### setWhitelist

* invocation: `gap setWhitelist <address_type> <address>`
* arguments: list of pair of address type and address: 
  - `address_type`: -> [`AddressType`](#AddressType)
  - `address`: -> [`MacAddress`](#MacAddress)
* result: None
* modeled after: `Gap::setWhitelist`


### setAdvertisingPolicyMode

* invocation: `gap setAdvertisingPolicyMode <advertising_policy_mode>`
* arguments: 
  - `advertising_policy_mode`: -> [`AdvertisingPolicyMode`](#AdvertisingPolicyMode)
* result: None
* modeled after: `Gap::setAdvertisingPolicyMode`


### setScanningPolicyMode

* invocation: `gap setScanningPolicyMode <scanning_policy_mode>`
* arguments: 
  - `scanning_policy_mode`: -> [`ScanningPolicyMode`](#ScanningPolicyMode)
* result: None
* modeled after: `Gap::setScanningPolicyMode`


### setInitiatorPolicyMode

* invocation: `gap setInitiatorPolicyMode <initiator_policy_mode>`
* arguments: 
  - `initiator_policy_mode`: -> [`InitiatorPolicyMode`](#InitiatorPolicyMode)
* result: None
* modeled after: `Gap::setInitiatorPolicyMode`



### getAdvertisingPolicyMode

* invocation: `gap getAdvertisingPolicyMode`
* arguments: None
* result: [`AdvertisingPolicyMode`](#AdvertisingPolicyMode) as a string
* modeled after: `Gap::getAdvertisingPolicyMode`


### getScanningPolicyMode

* invocation: `gap getScanningPolicyMode <scanning_policy_mode>`
* arguments: None
* result: [`ScanningPolicyMode`](#ScanningPolicyMode) as a string
* modeled after: `Gap::getScanningPolicyMode`


### getInitiatorPolicyMode

* invocation: `gap getInitiatorPolicyMode <initiator_policy_mode>`
* arguments: None
* result: [`InitiatorPolicyMode`](#InitiatorPolicyMode) as a string
* modeled after: `Gap::getInitiatorPolicyMode`



## gattClient module

The `gattClient` module expose the following functions from the class `GattClient`:


### discoverAllServicesAndCharacteristics

* invocation: `gattClient discoverAllServicesAndCharacteristics <connection_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
* result: A JSON array of the discovered service. Each service is a JSON object which contains the following fields:
  - [`UUID`](#UUID) "UUID": The UUID of the service.
  - [`uint16_t`](#uint16_t) "start_handle": The first attribute handle of the service.
  - [`uint16_t`](#uint16_t) "end_handle": The last attribute handle of the service.
  - "characteristics": an array of JSON objects describing the characteristics of the service. Each characteristics expose the following attributes:
    + [`UUID`](#UUID) "UUID": The UUID of the characteristic.
    + "properties": JSON array of the properties of the characteristics, each property is modeled as a string:
      * "broadcast"
      * "read"
      * "writeWoResp"
      * "write"
      * "notify"
      * "indicate"
      * "authSignedWrite"
    + [`uint16_t`](#uint16_t) "start_handle": The first attribute handle of the characteristic.
    + [`uint16_t`](#uint16_t) "value_handle": The handle used to retrieve the value of the characteristic.
    + [`uint16_t`](#uint16_t) "end_handle": The last attribute handle of the characteristic.
* modeled after: `GattClient::launchServiceDiscovery`, `GattClient::onServiceDiscoveryTermination`.


### discoverAllServices

* invocation: `gattClient discoverAllServices <connection_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
* result: A JSON array of the discovered service. Each service is a JSON object which contains the following fields:
  - [`UUID`](#UUID) "UUID": The UUID of the service.
  - [`uint16_t`](#uint16_t) "start_handle": The first attribute handle of the service.
  - [`uint16_t`](#uint16_t) "end_handle": The last attribute handle of the service.
* modeled after: `GattClient::launchServiceDiscovery`, `GattClient::onServiceDiscoveryTermination`.



### discoverPrimaryServicesByUUID

* invocation: `gattClient discoverPrimaryServicesByUUID <connection_handle> <service_UUID>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`UUID`](#UUID) service_UUID: The UUID of the service to discover.
* result: A JSON array of the discovered service. Each service is a JSON object which contains the following fields:
  - [`UUID`](#UUID) "UUID": The UUID of the service.
  - [`uint16_t`](#uint16_t) "start_handle": The first attribute handle of the service.
  - [`uint16_t`](#uint16_t) "end_handle": The last attribute handle of the service.
* modeled after: `GattClient::launchServiceDiscovery`, `GattClient::onServiceDiscoveryTermination`.



### discoverAllCharacteristicsDescriptors

* invocation: `gattClient discoverAllCharacteristicsDescriptors <connection_handle> <char_start> <char_end>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) "char_start": The first attribute handle of the characteristic targeted by the operation.
   - [`uint16_t`](#uint16_t) "char_end": The last attribute handle of the characteristic targeted by the operation.
* result: A JSON array of the discovered descriptors. Each discovered descriptor is a JSON object which contains the following fields:
  - [`uint16_t`](#uint16_t) "handle": Attribute handle of the descriptor.
  - [`UUID`](#UUID) "UUID": The UUID of the characteristic descriptor.
* modeled after: `GattClient::discoverCharacteristicDescriptors`



### readCharacteristicValue

* invocation: `gattClient readCharacteristicValue <connection_handle> <char_value_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) "char_value_handle": The attribute handle of the value to read.
* result: The value of the characteristic, as an [`HexString`](#hexstring).
* modeled after: `GattClient::read` and `GattClient::onDataRead`


### writeWithoutResponse

* invocation: `gattClient writeWithoutResponse <connection_handle> <char_value_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) char_value_handle: The attribute handle of the value to read.
   - [`HexString`](#hexstring) value: The value to write in the characteristic.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`



### write

* invocation: `gattClient write <connection_handle> <char_value_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) char_value_handle: The attribute handle of the value to write.
   - [`HexString`](#hexstring) value: The value to write in the characteristic.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`


### readCharacteristicDescriptor

* invocation: `gattClient readCharacteristicDescriptor <connection_handle> <descriptor_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) "descriptor_handle": The attribute handle of the descriptor to read.
* result: The value of the descriptor, as an [`HexString`](#hexstring).
* modeled after: `GattClient::read` and `GattClient::onDataRead`


### write

* invocation: `gattClient writeCharacteristicDescriptor <connection_handle> <descriptor_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) connection_handle: The connection handle used by the procedure.
   - [`uint16_t`](#uint16_t) descriptor_handle: The attribute handle of the descriptor to write.
   - [`HexString`](#hexstring) value: The value to write in the descriptor.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`


## gattServer module

The `gattServer` module allows a user to access the capabilities of the GattServer class but building a service is a bit different with this module than it is with the original class. Rather than registering a service all at once, user can split the declaration in multiple sub operations then commit the result in the GattServer when it is ready.



### declareService

* invocation: `gattServer declareService <service_uuid>`
* description: Start the declaration of a service, after this call, user can call:
  - `declareCharacteristic` to declare a characteristic inside the service, 
  - `commitService` to commit the service or 
  - `cancelServiceDeclaration` to cancel the service declaration.
  - 
* arguments: 
   - [`UUID`](#uuid) service_uuid: The UUID of the service declared.
* result: None


### declareCharacteristic

* invocation: `gattServer declareCharacteristic <characteristic_uuid>`
* description: Start the declaration of a characteristic, after this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`UUID`](#uuid) characteristic_uuid: The UUID of the characteristic declared.
* result: None


### setCharacteristicValue

* invocation: `gattServer setCharacteristicValue <value>`
* description: Set the value of the characteristic being declared. After this call, user can call:
  - `setCharacteristicValue` to set the value of this to another value.
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`HexString`](#hexstring) value: The value of the characteristic declared.
* result: None



### setCharacteristicProperties

* invocation: `gattServer setCharacteristicProperties [properties]`
* description: Set the properties of the characteristic being declared. After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic to another value.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: A list of string mapping to the properties to set: 
  - "broadcast"
  - "read"
  - "writeWoResp"
  - "write"
  - "notify"
  - "indicate"
  - "authSignedWrite" 
* result: None


### setCharacteristicVariableLength

* invocation: `gattServer setCharacteristicVariableLength <enable>`
* description: Set a boolean value which indicate if the characteristic has a variable length. If the characteristic has a variable length then `setCharacteristicMaxLength`  could be call to bound the length to a maximum.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`bool`](#bool) enable: True to enable variable length and false to enable fixed length.
* result: None


### setCharacteristicMaxLength

* invocation: `gattServer setCharacteristicMaxLength <max_length>`
* description: Set the maximum length of the value of the characteristic being declared.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`uint16_t`](#uint16_t) max_length: The max length that the characteristic value can occupy.
* result: None


### declareDescriptor

* invocation: `gattServer declareDescriptor <descriptor_uuid>`
* description: Start the declaration of a descriptor which will be attached to the characteristic being declared.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setDescriptorVariableLength`  enable or disable variable length value of this descriptor.
  - `setDescriptorMaxLength` set the maximum length that is allowed for the value of this descriptor.
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to start the declaration of another descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`UUID`](#uuid) max_length: The max length that the characteristic value can occupy.
* result: None


### setDescriptorValue

* invocation: `gattServer setDescriptorValue <value>`
* description: Set the value of the descriptor being declared. After this call, user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`HexString`](#hexstring) value: The value of the descriptor declared.
* result: None


### setDescriptorVariableLength

* invocation: `gattServer setDescriptorVariableLength <enable>`
* description: Set a boolean value which indicate if the descriptor has a variable length. If the descriptor has a variable length then `setDescriptorMaxLength`  could be call to bound the length to a maximum.
After this call, user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`bool`](#bool) enable: True to enable variable length and false to enable fixed length.
* result: None


### setDescriptorMaxLength

* invocation: `gattServer setDescriptorMaxLength <max_length>`
* description: Set the maximum length of the value of the descriptor being declared.
After this call, user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`uint16_t`](#uint16_t) max_length: The max length that the descriptor value can occupy.
* result: None


### commitService

* invocation: `gattServer commitService`
* description: Commit in the GattServer the service declared. After this call, the ongoing service declaration is reset.
* arguments: None
* result: A JSON object describing the service in the ATT DB. It contains the following fields:
  - [`UUID`](#uuid) "UUID": The uuid of the service
  - [`uint16_t`](#uint16_t) "handle": The attribute handle of the service. 
  - "characteristics": A JSON array of the characteristics presents in the service. Each characteristic is a JSON object containing the following fields:
    + [`UUID`](#uuid) "UUID": The uuid of the characteristic.
    + [`uint16_t`](#uint16_t) "value_handle": The attribute handle of the value of the characteristic. 
    + "properties": A JSON array of the properties of the characteristic. The possible values are:
      * "broadcast"
      * "read"
      * "writeWoResp"
      * "write"
      * "notify"
      * "indicate"
      * "authSignedWrite" 
    + [`uint16_t`](#uint16_t) "length": The length of the value of the characteristic.
    + [`uint16_t`](#uint16_t) "max_length": The maximum length that the value of the characteristic can take.
    + [`bool``](#bool`) "has_variable_length": Flag indicating if the length of the characteristic is fixed or variable.
    + [`HexString`](#hexstring) "value": The value of the characteristic.
    + "descriptors": A JSON array containing the descriptors of the characteristic:
      * [`UUID`](#uuid) "UUID": The uuid of the descriptor.
      * [`uint16_t`](#uint16_t) "handle": The attribute handle of the descriptor. 
      * [`uint16_t`](#uint16_t) "length": The length of the value of the descriptor.
      * [`uint16_t`](#uint16_t) "max_length": The maximum length that the value of the descriptor can take.
      * [`bool``](#bool`) "has_variable_length": Flag indicating if the length of the descriptor is fixed or variable
      * [`HexString`](#hexstring) "value": The value of the descriptor.
* modeled after: `GattServer::addService`


### cancelServiceDeclaration

* invocation: `gattServer cancelServiceDeclaration`
* description: Cancel the current service declaration.
* arguments: None
* result: None



### read

* invocation: `gattServer read <attribute_handle> <connection_handle>`
* description: Read the value an attribute handle of the gatt server. The connection_handle parameter is optional.
* arguments: 
  - [`uint16_t`](#uint16_t) "attribute_handle": The attribute handle to read. 
  - [`uint16_t`](#uint16_t) "connection_handle": Optional parameter, useful to read the value of CCCD for a given connection.
* result: 
  - [`HexString`](#hexstring) The value of the attribute.
* modeled after: `GattServer::read`


### write

* invocation: `gattServer write <attribute_handle> <value> <connection_handle>`
* description: Write the value an attribute handle of the gatt server. The connection_handle parameter is optional.
* arguments: 
  - [`uint16_t`](#uint16_t) "attribute_handle": The attribute handle to write. 
  - [`HexString`](#hexstring) "value": The value to write to the attribute.
  - [`uint16_t`](#uint16_t) "connection_handle": Optional parameter, useful to write the value of CCCD for a given connection.
* result: None
* modeled after: `GattServer::write`


### waitForDataWritten

* invocation: `gattServer waitForDataWritten <connection_handle> <attribute_handle> <timeout>`
* description: Wait for data of an attribute to be written.
* arguments: 
  - [`uint16_t`](#uint16_t) "connection_handle": Handle of the connection issuing the write request.
  - [`uint16_t`](#uint16_t) "attribute_handle": The attribute handle to monitor. 
  - [`uint16_t`](#uint16_t) "timeout": Maximum time allowed to this procedure; in ms. 
* result: None
* modeled after: `GattServer::onDataWritten`



## securityManager module

The `securityManager` module allows a user to access capabilities of the SecurityManager class.


### init

* invocation: `securityManager init <enable_bonding> <require_mitm> <io_capabilities> <passkey>`
* description: Initialize and configure the security manager.
* arguments: 
  - [`bool`](#bool) "enable_bonding": Allow or disallow bonding to the device.
  - [`bool`](#bool) "require_mitm": Allow or disallow MITM protection.
  - [`SecurityIOCapabilities`](#SecurityIOCapabilities) "io_capabilities": I/O capabilities of the device.
  - [`Passkey`](#passkey) "passkey": The static passkey used by the security manager.
* result: None
* modeled after: `SecurityManager::init`


### getAddressesFromBondTable

* invocation: `securityManager getAddressesFromBondTable <addresses_count>`
* description: Get the addresses from the bond table.
* arguments: 
  - [`uint8_t`](#uint8_t) "addresses_count": The maximum number of addresses to get. 
* result: A JSON array containing the addresses in the bond table. Each of this record is a JSON dictionary containing the following fields:
  - [`AddressType`](#AddressType) "address_type": The type of the address.
  - [`MacAddress`](#MacAddress) "address": The mac address.
* modeled after: `SecurityManager::getAddressesFromBondTable`


### purgeAllBondingState

* invocation: `securityManager purgeAllBondingState`
* description: Delete all peer device context and all related bonding information from the database within the security manager.
* arguments: None
* result: None
* modeled after: `SecurityManager::purgeAllBondingState`



## Data types format: 

Every parameter used by a CLI commands is typed. This is the list of type recognized by ble-cliapp.


### AddressType: 

* model: `BLEProtocol::AddressType_t`

The enum value map to a string:

| C++                                                     | ble cliapp                              |
|---------------------------------------------------------|-----------------------------------------|
| BLEProtocol::AddressType::PUBLIC                        | PUBLIC                                  |
| BLEProtocol::AddressType::PUBLIC                        | ADDR_TYPE_PUBLIC                        |
| BLEProtocol::AddressType::RANDOM_STATIC                 | STATIC                                  |
| BLEProtocol::AddressType::RANDOM_STATIC                 | ADDR_TYPE_RANDOM_STATIC                 |
| BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE     | ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE     |
| BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE     | ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE     |
| BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE | ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE |
| BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE | ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE |

### MacAddress

This type model a mac address, it is always represented a string: `XX:XX:XX:XX:XX:XX` where `X` is an hexadecimal character.

* model `Gap::Address_t`

### uint16_t

unsigned integer on 16 bits. The application accept decimal and hexadecimal (0x...) representations.

* model `uint16_t`

### bool

Boolean type, ble cliapp accept "true" or "false" as valid values.

* model `bool`

### GapRole

* model Gap::`Role_t`.

| C++             | ble-cliapp   |
|-----------------|--------------|
| Gap::PERIPHERAL | "PERIPHERAL" |
| Gap::CENTRAL    | "CENTRAL"    |


### DisconnectionReason

* model `Gap::DisconnectionReason_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| Gap::CONNECTION_TIMEOUT                          | "CONNECTION_TIMEOUT"                          |
| Gap::REMOTE_USER_TERMINATED_CONNECTION           | "REMOTE_USER_TERMINATED_CONNECTION"           |
| Gap::REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES | "REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES" |
| Gap::REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     | "REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF"     |
| Gap::LOCAL_HOST_TERMINATED_CONNECTION            | "LOCAL_HOST_TERMINATED_CONNECTION"            |
| Gap::CONN_INTERVAL_UNACCEPTABLE                  | "CONN_INTERVAL_UNACCEPTABLE"                  |


### Appearance

* model `GapAdvertisingData::Appearance_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingData::UNKNOWN | "UNKNOWN"  |
| GapAdvertisingData::GENERIC_PHONE | "GENERIC_PHONE"  |
| GapAdvertisingData::GENERIC_COMPUTER | "GENERIC_COMPUTER"  |
| GapAdvertisingData::GENERIC_WATCH | "GENERIC_WATCH"  |
| GapAdvertisingData::WATCH_SPORTS_WATCH | "WATCH_SPORTS_WATCH"  |
| GapAdvertisingData::GENERIC_CLOCK | "GENERIC_CLOCK"  |
| GapAdvertisingData::GENERIC_DISPLAY | "GENERIC_DISPLAY"  |
| GapAdvertisingData::GENERIC_REMOTE_CONTROL | "GENERIC_REMOTE_CONTROL"  |
| GapAdvertisingData::GENERIC_EYE_GLASSES | "GENERIC_EYE_GLASSES"  |
| GapAdvertisingData::GENERIC_TAG | "GENERIC_TAG"  |
| GapAdvertisingData::GENERIC_KEYRING | "GENERIC_KEYRING"  |
| GapAdvertisingData::GENERIC_MEDIA_PLAYER | "GENERIC_MEDIA_PLAYER"  |
| GapAdvertisingData::GENERIC_BARCODE_SCANNER | "GENERIC_BARCODE_SCANNER"  |
| GapAdvertisingData::GENERIC_THERMOMETER | "GENERIC_THERMOMETER"  |
| GapAdvertisingData::THERMOMETER_EAR | "THERMOMETER_EAR"  |
| GapAdvertisingData::GENERIC_HEART_RATE_SENSOR | "GENERIC_HEART_RATE_SENSOR"  |
| GapAdvertisingData::HEART_RATE_SENSOR_HEART_RATE_BELT | "HEART_RATE_SENSOR_HEART_RATE_BELT"  |
| GapAdvertisingData::GENERIC_BLOOD_PRESSURE | "GENERIC_BLOOD_PRESSURE"  |
| GapAdvertisingData::BLOOD_PRESSURE_ARM | "BLOOD_PRESSURE_ARM"  |
| GapAdvertisingData::BLOOD_PRESSURE_WRIST | "BLOOD_PRESSURE_WRIST"  |
| GapAdvertisingData::HUMAN_INTERFACE_DEVICE_HID | "HUMAN_INTERFACE_DEVICE_HID"  |
| GapAdvertisingData::KEYBOARD | "KEYBOARD"  |
| GapAdvertisingData::MOUSE | "MOUSE"  |
| GapAdvertisingData::JOYSTICK | "JOYSTICK"  |
| GapAdvertisingData::GAMEPAD | "GAMEPAD"  |
| GapAdvertisingData::DIGITIZER_TABLET | "DIGITIZER_TABLET"  |
| GapAdvertisingData::CARD_READER | "CARD_READER"  |
| GapAdvertisingData::DIGITAL_PEN | "DIGITAL_PEN"  |
| GapAdvertisingData::BARCODE_SCANNER | "BARCODE_SCANNER"  |
| GapAdvertisingData::GENERIC_GLUCOSE_METER | "GENERIC_GLUCOSE_METER"  |
| GapAdvertisingData::GENERIC_RUNNING_WALKING_SENSOR | "GENERIC_RUNNING_WALKING_SENSOR"  |
| GapAdvertisingData::RUNNING_WALKING_SENSOR_IN_SHOE | "RUNNING_WALKING_SENSOR_IN_SHOE"  |
| GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_SHOE | "RUNNING_WALKING_SENSOR_ON_SHOE"  |
| GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_HIP | "RUNNING_WALKING_SENSOR_ON_HIP"  |
| GapAdvertisingData::GENERIC_CYCLING | "GENERIC_CYCLING"  |
| GapAdvertisingData::CYCLING_CYCLING_COMPUTER | "CYCLING_CYCLING_COMPUTER"  |
| GapAdvertisingData::CYCLING_SPEED_SENSOR | "CYCLING_SPEED_SENSOR"  |
| GapAdvertisingData::CYCLING_CADENCE_SENSOR | "CYCLING_CADENCE_SENSOR"  |
| GapAdvertisingData::CYCLING_POWER_SENSOR | "CYCLING_POWER_SENSOR"  |
| GapAdvertisingData::CYCLING_SPEED_AND_CADENCE_SENSOR | "CYCLING_SPEED_AND_CADENCE_SENSOR"  |
| GapAdvertisingData::PULSE_OXIMETER_GENERIC | "PULSE_OXIMETER_GENERIC"  |
| GapAdvertisingData::PULSE_OXIMETER_FINGERTIP | "PULSE_OXIMETER_FINGERTIP"  |
| GapAdvertisingData::PULSE_OXIMETER_WRIST_WORN | "PULSE_OXIMETER_WRIST_WORN"  |
| GapAdvertisingData::OUTDOOR_GENERIC | "OUTDOOR_GENERIC"  |
| GapAdvertisingData::OUTDOOR_LOCATION_DISPLAY_DEVICE | "OUTDOOR_LOCATION_DISPLAY_DEVICE"  |
| GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE | "OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE"  |
| GapAdvertisingData::OUTDOOR_LOCATION_POD | "OUTDOOR_LOCATION_POD"  |
| GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_POD | "OUTDOOR_LOCATION_AND_NAVIGATION_POD" |

### int8_t

Signed integer on 8 bits. The application accept decimal and hexadecimal (0x...) representations.

* model `int8_t`

### AdvertisingType

* model `GapAdvertisingParams::AdvertisingType_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED | "ADV_CONNECTABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED | "ADV_CONNECTABLE_DIRECTED" |
| GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED | "ADV_SCANNABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED | "ADV_NON_CONNECTABLE_UNDIRECTED" |

### AdvertisingDataType

* model `GapAdvertisingData::DataType_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingData::FLAGS | "FLAGS" |
| GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS | "INCOMPLETE_LIST_16BIT_SERVICE_IDS" |
| GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS | "COMPLETE_LIST_16BIT_SERVICE_IDS" |
| GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS | "INCOMPLETE_LIST_32BIT_SERVICE_IDS" |
| GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS | "COMPLETE_LIST_32BIT_SERVICE_IDS" |
| GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS | "INCOMPLETE_LIST_128BIT_SERVICE_IDS" |
| GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS | "COMPLETE_LIST_128BIT_SERVICE_IDS" |
| GapAdvertisingData::LIST_128BIT_SOLICITATION_IDS | "LIST_128BIT_SOLICITATION_IDS" |
| GapAdvertisingData::SHORTENED_LOCAL_NAME | "SHORTENED_LOCAL_NAME" |
| GapAdvertisingData::COMPLETE_LOCAL_NAME | "COMPLETE_LOCAL_NAME" |
| GapAdvertisingData::TX_POWER_LEVEL | "TX_POWER_LEVEL" |
| GapAdvertisingData::DEVICE_ID | "DEVICE_ID" |
| GapAdvertisingData::SLAVE_CONNECTION_INTERVAL_RANGE | "SLAVE_CONNECTION_INTERVAL_RANGE" |
| GapAdvertisingData::SERVICE_DATA | "SERVICE_DATA" |
| GapAdvertisingData::APPEARANCE | "APPEARANCE" |
| GapAdvertisingData::ADVERTISING_INTERVAL | "ADVERTISING_INTERVAL" |
| GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA | "MANUFACTURER_SPECIFIC_DATA" |


### uint32_t

unsigned integer on 32 bits. The application accept decimal and hexadecimal (0x...) representations.

* model `uint32_t`

### String

Model an ASCII string, spaces are not accepted at the moment.

### HexString

Hexadecimal representation for raw data. Each byte is represented as two hexadecimal characters.

### AdvertisingType

* model `GapAdvertisingParams::AdvertisingType_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED | "ADV_CONNECTABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED | "ADV_CONNECTABLE_DIRECTED" |
| GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED | "ADV_SCANNABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED | "ADV_NON_CONNECTABLE_UNDIRECTED" |


### AdvertisingPayload

A `json` dictionary of the advertising payload: 
* `FLAGS` -> JSON array of \[[`GapAdvertisingDataFlags`](#gapadvertisingdataflags)\].
* `INCOMPLETE_LIST_16BIT_SERVICE_IDS` -> JSON array of \[[`uint16_t`](#uint16_t)\] in the hex form (0xXXXX).
* `COMPLETE_LIST_16BIT_SERVICE_IDS` -> JSON array of  \[[`uint16_t`](#uint16_t)\] in the hex form (0xXXXX).
* `INCOMPLETE_LIST_32BIT_SERVICE_IDS` -> JSON array of \[[`uint32_t`](#uint32_t)\] in the hex form (0xXXXXXXXX).
* `COMPLETE_LIST_32BIT_SERVICE_IDS` -> JSON array of \[[`uint32_t`](#uint32_t)\] in the hex form (0xXXXXXXXX).
* `SHORTENED_LOCAL_NAME` -> [`String`](#string)
* `COMPLETE_LOCAL_NAME` -> [`String`](#string)
* `MANUFACTURER_SPECIFIC_DATA` -> [`HexString`](#hex_string)
* `raw`: The complete advertising payload in an [`HexString`](#hex_string).


### uint8_t

Unsigned 8 bits integer. The application accept decimal and hexadecimal (0x...) representations.

### AdvertisingPolicyMode

* model: `Gap::AdvertisingPolicyMode_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| Gap::ADV_POLICY_IGNORE_WHITELIST | "ADV_POLICY_IGNORE_WHITELIST" |
| Gap::ADV_POLICY_FILTER_SCAN_REQS | "ADV_POLICY_FILTER_SCAN_REQS" |
| Gap::ADV_POLICY_FILTER_CONN_REQS | "ADV_POLICY_FILTER_CONN_REQS" |
| Gap::ADV_POLICY_FILTER_ALL_REQS | "ADV_POLICY_FILTER_ALL_REQS" |


### ScanningPolicyMode

* model `Gap::ScanningPolicyMode_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| Gap::SCAN_POLICY_IGNORE_WHITELIST | "SCAN_POLICY_IGNORE_WHITELIST"  |
| Gap::SCAN_POLICY_FILTER_ALL_ADV | "SCAN_POLICY_FILTER_ALL_ADV" |



### InitiatorPolicyMode

* model `Gap::InitiatorPolicyMode_t`

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| Gap::INIT_POLICY_IGNORE_WHITELIST | "INIT_POLICY_IGNORE_WHITELIST"  |
| Gap::INIT_POLICY_FILTER_ALL_ADV | "INIT_POLICY_FILTER_ALL_ADV" |



### AdvertisingPolicyMode

* model `Gap::AdvertisingPolicyMode_t`

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| Gap::ADV_POLICY_IGNORE_WHITELIST | "ADV_POLICY_IGNORE_WHITELIST"  |
| Gap::ADV_POLICY_FILTER_SCAN_REQS | "ADV_POLICY_FILTER_SCAN_REQS"  |
| Gap::ADV_POLICY_FILTER_CONN_REQS | "ADV_POLICY_FILTER_CONN_REQS"  |
| Gap::ADV_POLICY_FILTER_ALL_REQS | "ADV_POLICY_FILTER_ALL_REQS" |




# Extending the application

It is easy to add new commands modules and extends existing modules with new commands, the software is flexible enough to add both in few lines of code. 

## Sources organization:

The sources of the applications are contained in the `source` folder. This folder contains the following files and folders:

* `CLICommand`: C++ framework built on top of `mbed-cli-command`, it contains all the necessary primitives to create  commands and group them into commands module. 
* `Commands`: Contains the commands and commands module of the application.
* `EventQueue`: cross platform event queue used by the application. 
* `Serialization`: Contains a collection of serializers/deserializers which convert value from the command line to C++ value and vice versa. 
* `util`: Few utility classes.
* `main.cpp`: Glue everything together.

## Module creation 

A module is a structure declaring four static functions. Lets start by creating a simple module: 

```c++
struct MyCommandModule {
    static const char* name() {
        return "my_module";
    }

    static const char* info() {
        return "Module created for learning purposes";
    }

    static const char* man() {
        return "Manual of this module ...";
    }

    static ConstArray<const Command*> commands() { 
      static const Command* const commandHandlers[] = {
          // Pointer to the commands hosted by the module.
      };
      return ConstArray<const Command*>(commandHandlers);
    }
};
```

* `const char* name()`: return the name used to access to the module. On the CLI, users access commands by typing the module name then the command name then the arguments of the command. 

``` 
<my_module> <command_name> <args...>
```

* `const char* info()`: Short readable description of the module.
* `const char* man()`: Manual of the module.
* `ConstArray<const Command*> commands()`: return the list of commands hosted by the module. The class ConstArray turn a C or C++ array into an object.


## Command creation 












































