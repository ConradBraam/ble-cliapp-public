This application is a shell for mbed BLE. Via a terminal, a user can call most 
of the functions available in mbed BLE, the device execute it and send back the 
result.
Even if the tool was initially created to run system tests of mbed BLE it can be 
use to debug a port of mbed BLE or an application using mbed BLE. 


# Summary

<!-- TOC -->

- [Summary](#summary)
- [Getting started](#getting-started)
    - [Compilation](#compilation)
        - [mbed OS 2 and 5](#mbed-os-2-and-5)
        - [mbed OS 3](#mbed-os-3)
    - [First run](#first-run)
        - [Initializing BLE](#initializing-ble)
        - [Setting up an advertising payload](#setting-up-an-advertising-payload)
        - [Start advertising](#start-advertising)
    - [Exploration](#exploration)
        - [Modules available](#modules-available)
        - [List commands of a module](#list-commands-of-a-module)
        - [Help of command](#help-of-command)
- [Commands](#commands)
    - [ble module](#ble-module)
        - [init](#init)
        - [shutdown](#shutdown)
        - [reset](#reset)
        - [getVersion](#getversion)
    - [gap module](#gap-module)
        - [setAddress](#setaddress)
        - [getAddress](#getaddress)
        - [getMinAdvertisingInterval](#getminadvertisinginterval)
        - [getMinNonConnectableAdvertisingInterval](#getminnonconnectableadvertisinginterval)
        - [getMaxAdvertisingIntervalCommand](#getmaxadvertisingintervalcommand)
        - [stopAdvertising](#stopadvertising)
        - [stopScan](#stopscan)
        - [connect](#connect)
        - [waitForConnection](#waitforconnection)
        - [disconnect](#disconnect)
        - [getPreferredConnectionParams](#getpreferredconnectionparams)
        - [setPreferredConnectionParams](#setpreferredconnectionparams)
        - [setDeviceName](#setdevicename)
        - [getDeviceName](#getdevicename)
        - [setAppearance](#setappearance)
        - [getAppearance](#getappearance)
        - [setTxPower](#settxpower)
        - [getPermittedTxPowerValues](#getpermittedtxpowervalues)
        - [getState](#getstate)
        - [setAdvertisingType](#setadvertisingtype)
        - [setAdvertisingInterval](#setadvertisinginterval)
        - [setAdvertisingTimeout](#setadvertisingtimeout)
        - [startAdvertising](#startadvertising)
        - [clearAdvertisingPayload](#clearadvertisingpayload)
        - [accumulateAdvertisingPayload](#accumulateadvertisingpayload)
        - [updateAdvertisingPayload](#updateadvertisingpayload)
        - [getAdvertisingPayload](#getadvertisingpayload)
        - [accumulateScanResponse](#accumulatescanresponse)
        - [clearScanResponse](#clearscanresponse)
        - [setScanParam](#setscanparam)
        - [setScanInterval](#setscaninterval)
        - [setScanWindow](#setscanwindow)
        - [setScanTimeout](#setscantimeout)
        - [setActiveScanning](#setactivescanning)
        - [startScan](#startscan)
        - [getAdvertisingParams](#getadvertisingparams)
        - [setAdvertisingParams](#setadvertisingparams)
        - [getMaxWhitelistSize](#getmaxwhitelistsize)
        - [getWhitelist](#getwhitelist)
        - [setWhitelist](#setwhitelist)
        - [setAdvertisingPolicyMode](#setadvertisingpolicymode)
        - [setScanningPolicyMode](#setscanningpolicymode)
        - [setInitiatorPolicyMode](#setinitiatorpolicymode)
        - [getAdvertisingPolicyMode](#getadvertisingpolicymode)
        - [getScanningPolicyMode](#getscanningpolicymode)
        - [getInitiatorPolicyMode](#getinitiatorpolicymode)
    - [gattClient module](#gattclient-module)
        - [discoverAllServicesAndCharacteristics](#discoverallservicesandcharacteristics)
        - [discoverAllServices](#discoverallservices)
        - [discoverPrimaryServicesByUUID](#discoverprimaryservicesbyuuid)
        - [discoverAllCharacteristicsDescriptors](#discoverallcharacteristicsdescriptors)
        - [readCharacteristicValue](#readcharacteristicvalue)
        - [writeWithoutResponse](#writewithoutresponse)
        - [write](#write)
        - [readCharacteristicDescriptor](#readcharacteristicdescriptor)
        - [write](#write-1)
        - [listenHVX](#listenhvx)
    - [gattServer module](#gattserver-module)
        - [declareService](#declareservice)
        - [declareCharacteristic](#declarecharacteristic)
        - [setCharacteristicValue](#setcharacteristicvalue)
        - [setCharacteristicProperties](#setcharacteristicproperties)
        - [setCharacteristicVariableLength](#setcharacteristicvariablelength)
        - [setCharacteristicMaxLength](#setcharacteristicmaxlength)
        - [declareDescriptor](#declaredescriptor)
        - [setDescriptorValue](#setdescriptorvalue)
        - [setDescriptorVariableLength](#setdescriptorvariablelength)
        - [setDescriptorMaxLength](#setdescriptormaxlength)
        - [commitService](#commitservice)
        - [cancelServiceDeclaration](#cancelservicedeclaration)
        - [read](#read)
        - [write](#write-2)
        - [waitForDataWritten](#waitfordatawritten)
    - [securityManager module](#securitymanager-module)
        - [init](#init-1)
        - [getAddressesFromBondTable](#getaddressesfrombondtable)
        - [purgeAllBondingState](#purgeallbondingstate)
- [Data types format:](#data-types-format)
    - [Primitive types](#primitive-types)
        - [bool](#bool)
        - [int8_t](#int8_t)
        - [uint8_t](#uint8_t)
        - [uint16_t](#uint16_t)
        - [uint32_t](#uint32_t)
        - [String](#string)
        - [HexString](#hexstring)
    - [BLE](#ble)
        - [AddressType:](#addresstype)
        - [MacAddress](#macaddress)
        - [UUID](#uuid)
    - [Gap](#gap)
        - [GapRole](#gaprole)
        - [DisconnectionReason](#disconnectionreason)
        - [Appearance](#appearance)
        - [AdvertisingType](#advertisingtype)
        - [AdvertisingDataType](#advertisingdatatype)
        - [AdvertisingDataFlags](#advertisingdataflags)
        - [AdvertisingType](#advertisingtype-1)
        - [AdvertisingPolicyMode](#advertisingpolicymode)
        - [ScanningPolicyMode](#scanningpolicymode)
        - [InitiatorPolicyMode](#initiatorpolicymode)
        - [AdvertisingPolicyMode](#advertisingpolicymode-1)
        - [AdvertisingPayload](#advertisingpayload)
    - [SecurityManager](#securitymanager)
        - [Passkey](#passkey)
        - [IO Capabilities](#io-capabilities)
    - [GattClient](#gattclient)
        - [HVXType_t](#hvxtype_t)
- [Extending the application](#extending-the-application)
    - [Organization](#organization)
        - [Reusable code](#reusable-code)
        - [Application code:](#application-code)
    - [Command creation](#command-creation)
        - [Command concepts](#command-concepts)
        - [Command declaration](#command-declaration)
            - [Begining a command declaration](#begining-a-command-declaration)
            - [Adding the command name](#adding-the-command-name)
            - [Adding help of the command](#adding-help-of-the-command)
            - [Describing command parameters](#describing-command-parameters)
            - [Describing command results](#describing-command-results)
            - [Command handler](#command-handler)
            - [Command instantiation](#command-instantiation)
            - [Complete example](#complete-example)
            - [Few words about the Command design](#few-words-about-the-command-design)
            - [Asynchronous command](#asynchronous-command)
    - [Module creation](#module-creation)
    - [Register command suite](#register-command-suite)
    - [Serialization](#serialization)
        - [String deserialization](#string-deserialization)
        - [Enum deserialization/serialization](#enum-deserializationserialization)
        - [JSON serialization](#json-serialization)
            - [Number serialization](#number-serialization)
            - [String serialization](#string-serialization)
            - [Boolean serialization](#boolean-serialization)
            - [Null serialization](#null-serialization)
            - [Object declaration](#object-declaration)
            - [Array declaration](#array-declaration)

<!-- /TOC -->


# Getting started 

This application is designed to work on any version of mbed OS: 2, 3 or 5. 

## Compilation

### mbed OS 2 and 5 

Use [mbed-cli](https://github.com/ARMmbed/mbed-cli) to compile the application: 

```shell
mbed compile -t <compiler> -m <target> --profile toolchains_profile.json
```

> **Note:** It is important to apply the profile located in 
`toolchains_profile.json` to reduce the size of the binary.

### mbed OS 3

Use [yotta](http://yottadocs.mbed.com/#installing) to compile the application: 

```shell
yotta target <target>
yotta build
```

## First run

Once the application is compiled, load it onto your board and open a serial port 
terminal targeting the COM port of your board. It is important to set the 
baudrate to 115.200 baud. 

A good way to start with the application is to create a simple beacon.

### Initializing BLE

First, ble has to be initialised. In C++, this would be achieve by the following 
lines of code: 

```c++
BLE& ble = BLE::Instance()
ble.init();
```

With ble-cliapp enter the following command to achieve the same result: 

```
ble init
```

> Invoking a command follow the format:  `<command module> <command name> [arguments]`.
> A module is a cohesive group of command.

In this case, `ble` is the ble module and `init` is the initialization command 
of the ble module. No arguments are supplied.

The board should respond with the following JSON document: 

```json
{
        "status": 0
}
```

The `status` field indicate if the command has succeed or not. A value of 0 
indicate that the command execution was a success. The list of status code can 
be found in the enumeration 
[CommandResponse::StatusCode_t](source/CLICommand/CommandResponse.h).


### Setting up an advertising payload 

The advertising payload of the beacon for this example will just be a 
complete local name with `FOO_BAR_BAZ` as the value.

The C++ code for this operation would be: 

```c++
const char device_name[] = "FOO_BAR_BAZ";
Gap& gap = ble.gap();
gap.accumulateAdvertisingPayload(
  GapAdvertisingData::COMPLETE_LOCAL_NAME,
  device_name, sizeof(device_name)
);
```

While with ble-cliapp the boilerplate is eliminated and can be reduce to the 
command: 

```
gap accumulateAdvertisingPayload COMPLETE_LOCAL_NAME FOO_BAR_BAZ
```

The command `accumulateAdvertisingPayload` of the `gap` module is invoked with 
`COMPLETE_LOCAL_NAME` as its first argument and `FOO_BAR_BAZ`
as its second argument.

The device should response with a status code 0.

It is possible to verify the content of the advertising payload by typing the 
command `gap getAdvertisingPayload`. The device should respond with the 
following JSON document: 

```json
{
        "status": 0,
        "result": {
                "COMPLETE_LOCAL_NAME": "FOO_BAR_BAZ",
                "raw": "0C09464F4F5F4241525F42415A"
        }
}
```

A new field, `result`, is present in the response. It is used by commands to 
transmit the result of the command if any. For the command `getAdvertisingPayload`
it is the content of the payload. If the command does not succeed, the response 
can also carry details about the faillure in an `error` field instead of a 
`result` field.


### Start advertising 

In C++ this would be achieved by: 

```c++
gap.startAdvertising();
```

while with ble-cliapp this can be done with the command 

```
gap startAdvertising
```

At this point, the device should be visible with a BLE scanner and advertise the 
complete local name `FOO_BAR_BAZ`.


## Exploration 

It is possible to list commands available in every module and query help about 
each of them in ble-cliapp itself.

### Modules available 

The modules available in ble-cliapp are: 

* `ble`: Model the class `BLE`
* `gap`: Model the class `Gap`
* `gattServer`: Model the class `GattServer`
* `gattClient`: Model the class `GattClient`
* `securityManager`: Model the class `SecurityManager`

### List commands of a module 

Every module have a builtin command which list all the commands of the module, 
it is the `list` command which can be invoked with `<module name> <list>`.

For instance, to list the commands in the `gap` module, the command `gap list` 
should be entered, it will output the result: 

```json
{
        "status": 0,
        "result": [
                "help",
                "list",
                "setAddress",
                "getAddress",
                "getMinAdvertisingInterval",
                "getMinNonConnectableAdvertisingInterval",
                "getMaxAdvertisingInterval",
                "stopAdvertising",
                "stopScan",
                "connect",
                "waitForConnection",
                "disconnect",
                "getPreferredConnectionParams",
                "setPreferredConnectionParams",
                "updateConnectionParams",
                "setDeviceName",
                "getDeviceName",
                "setAppearance",
                "getAppearance",
                "setTxPower",
                "getPermittedTxPowerValues",
                "getState",
                "setAdvertisingType",
                "setAdvertisingInterval",
                "setAdvertisingTimeout",
                "startAdvertising",
                "clearAdvertisingPayload",
                "accumulateAdvertisingPayload",
                "updateAdvertisingPayload",
                "setAdvertisingPayload",
                "getAdvertisingPayload",
                "accumulateScanResponse",
                "clearScanResponse",
                "setScanParams",
                "setScanInterval",
                "setScanWindow",
                "setScanTimeout",
                "setActiveScanning",
                "startScan",
                "initRadioNotification",
                "getAdvertisingParams",
                "setAdvertisingParams",
                "getMaxWhitelistSize",
                "getWhitelist",
                "setWhitelist",
                "setAdvertisingPolicyMode",
                "setScanningPolicyMode",
                "setInitiatorPolicyMode",
                "getAdvertisingPolicyMode",
                "getScanningPolicyMode",
                "getInitiatorPolicyMode"
        ]
}
```

Every entry in the `result` array is a command belonging to the module. 

### Help of command 

To get details about a command, it is possible to use the builtin `help` command.
It syntax is: `<module name> <help> <command name>`

For example, to get more informations around the command connect of the gap 
module, just enter the command: `gap help connect`. The device should output:

```json
{
        "status": 0,
        "result": {
                "command": "connect",
                "help": "connect to a device, if this function succeed, a ConnectionCallbackParams_t is returned.",
                "arguments": [
                        "addressType: BLEProtocol::AddressType_t - The address type to of the peer device.",
                        "address: MacAddress_t - The address itself which is a string representation like 'XX:XX:XX:XX:XX:XX'",
                        "minConnectionInterval: uint16_t - Minimum Connection Interval in 1.25 ms units",
                        "maxConnectionInterval: uint16_t - Maximum Connection Interval in 1.25 ms units",
                        "slaveLatency: uint16_t - Slave Latency in number of connection events",
                        "connectionSupervisionTimeout: uint16_t - Connection Supervision Timeout in 10 ms units",
                        "interval: uint16_t - The scan interval, it should be a number between 3 and 10420ms.",
                        "window: uint16_t - The scan window, it should be a number between 3 and 10420ms.",
                        "scan timeout: uint16_t - The scan timeout, it should be a number between 0 and 65534",
                        "activeScanning: bool - A boolean value { true, false } indicating if the device send scan request or not",
                        "timeout: uint16_t - Maximum time allowed for this procedure"
                ],
                "results": [
                        "handle: uint16_t - The handle of the connection created",
                        "role: Gap::Role - Role of the device in the connection (here, it should be central)",
                        "peerAddrType: BLEProtocol::AddressType_t - The addressType of the peer",
                        "peerAddr: MacAddress_t - The address of the peer",
                        "ownAddrType: BLEProtocol::AddressType_t - The address type of this device",
                        "ownAddr: MacAddress_t - The address of this device",
                        "connectionParams: JSON object - Object which contain the parameters of the connection",
                        "connectionParams.minConnectionInterval: uint16_t - minimum connection interval for this connection",
                        "connectionParams.maxConnectionInterval: uint16_t - maximum connection interval for this connection",
                        "connectionParams.slaveLatency: uint16_t - slave latency of the connection",
                        "connectionParams.connectionSupervisionTimeout: uint16_t - supervision timeout for this connection"
                ]
        }
}
```

The result detail the command name, the expected parameters with their type and 
the results sent back in case of success.

# Commands

Invoking a command is simple: 

```
`<module> <command> [arguments]`
```

Like mbed BLE, the shell is divided in modules which contain a set of commands, 
the following modules are accessible: 

* `ble`: Model the class `BLE`
* `gap`: Model the class `Gap`
* `gattServer`: Model the class `GattServer`
* `gattClient`: Model the class `GattClient`
* `securityManager`: Model the class `SecurityManager`

When the command as completed, the result is printed on the terminal, in a `json` 
object. This object contains the following properties: 

* `status`: returned status, if the result is equal to 0 then the command has 
been correctly executed. A negative value means that the command execution has 
failed. 
* `error`: This key will be present only if the command execution has failed. 
The value associated is a `json` value (generally a string).
* `result`: This key is present if the command has succeeded and there is a 
meaningful result to report to the user. The value associated is a `json` value, 
its format depend on the command invoked.

## ble module

The `ble` module expose functions from the class `BLE`:

### init
Initialize the ble API and underlying BLE stack.

* invocation: `ble init`
* arguments: None 
* result: None
* modeled after: `BLE::init`

### shutdown
Shutdown the current BLE instance, calling ble related function after this call 
may lead to faillure.

* invocation: `ble shutdown`
* arguments: None 
* result: None
* modeled after: `BLE::shutdown`

### reset
Reset the ble stack. If the stack is initialized, it will shut it down prior to 
initializing it again.

* invocation: `ble reset`
* arguments: None 
* result: None

### getVersion
Return the version of the BLE API.

* invocation: `ble getVersion`
* modeled after: `BLE::getVersion`
* arguments: None 
* result: The version of the underlying stack as a string.




## gap module

The `gap` module expose functions from the class `Gap`:

### setAddress
Set the address of this device.

* invocation: `gap setAddress <address_type> <MAC address>`
* arguments: 
  - [`AddressType`](#addresstype) **address_type**: The type of the address to set.
  - [`MacAddress`](#macaddress) **MAC address**: The mac address to set.
* result: None
* modeled after: `Gap::setAddress`

### getAddress
Get the address and the type of address of this device.

* invocation: `gap getAddress`
* arguments: None 
* result: `json` object containing the properties:
  - [`AddressType`](#addresstype) **address_type**: The type of the address set.
  - [`MacAddress`](#macaddress) **MAC address**: The mac address set.
* modeled after: `Gap::getAddress`

### getMinAdvertisingInterval
Return the minimum advertising interval supported by the device.

* invocation: `gap getMinAdvertisingInterval`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the minimum advertising interval.
* modeled after: `Gap::getMinAdvertisingInterval`

### getMinNonConnectableAdvertisingInterval
* invocation: `gap getMinNonConnectableAdvertisingInterval`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the minimum non connectable 
advertising interval.
* modeled after: `Gap::getMinNonConnectableAdvertisingInterval`

### getMaxAdvertisingIntervalCommand
* invocation: `gap getMaxAdvertisingIntervalCommand`
* arguments: None 
* result: An [uint16_t](#uint16_t) representing the maximum advertising interval.
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
* invocation: `gap connect <address_type> <address> <min_connection_interval> 
<max_connection_interval> <slave_latency> <connection_supervision_timeout> 
<interval> <window> <scan_timeout> <active scanning> <command_timeout>`
* arguments:
  - [`AddressType`](#addresstype) **address_type**: The address type of the peer 
  device.
  - [`MacAddress`](#macaddress) **address**: The mac address of the peer.
  - connection parameters: The next four arguments are the connections parameters:
    + [`uint16_t`](#uint16_t) **min_connection_interval**: Minimum Connection 
    Interval in 1.25 ms units
    + [`uint16_t`](#uint16_t) **max_connection_interval**: Maximum Connection 
    Interval in 1.25 ms units
    + [`uint16_t`](#uint16_t) **slave_latency**: Slave Latency in number of 
    connection events
    + [`uint16_t`](#uint16_t) **connection_supervision_timeout**: Connection 
    Supervision Timeout in 10 ms units
  - scan parameters: The next four arguments used as scan parameter during peer 
  search.
    + [`uint16_t`](#uint16_t) **interval**: The scan interval, it should be a number 
    between 3 and 10420ms.
    + [`uint16_t`](#uint16_t) **window**: The scan window, it should be a number 
    between 3 and 10420ms.
    + [`uint16_t`](#uint16_t) **scan timeout**: The scan timeout, it should be a 
    number between 0 and 65534
    + [`bool`](#bool) **activeScanning**: A boolean value indicating if the device 
    send scan request or not
  - [`uint16_t`](#uint16_t) **timeout**: Maximum time allowed for this procedure; in 
  milliseconds.
* result: A `json` object containing the following attributes: 
  - [`uint16_t`](#uint16_t) **handle**: The connection handle
  - [`GapRole`](#gaprole) **role**: Role of the device in the connection (here, it 
  should be central)
  - [`AddressType`](#addresstype) **peerAddrType**: The addressType of the peer
  - [`MacAddress`](#macaddress) **peerAddr**: The address of the peer
  - [`AddressType`](#addresstype) **ownAddrType**: The address type of this device
  - [`MacAddress`](#macaddress) **ownAddr**: The address of this device
  - `JSONDictionary` **connectionParams**: Contain the parameters of the connection:
    + [`uint16_t`](#uint16_t) **minConnectionInterval**: minimum connection interval 
    for this connection
    + [`uint16_t`](#uint16_t) **maxConnectionInterval**: maximum connection interval 
    for this connection
    + [`uint16_t`](#uint16_t) **slaveLatency**: slave latency of the connection
    + [`uint16_t`](#uint16_t) **connectionSupervisionTimeout**: supervision timeout 
    for this connection
* modeled after: `Gap::connect` and `Gap::onConnection`.


### waitForConnection
This function instruct the device to wait for a connection and report the 
connection parameters once it is connected.

* invocation: `gap waitForConnection <command_timeout>`
* argument: [`uint16_t`](#uint16_t) **timeout**: Maximum time allowed for this 
command; in milliseconds.
* result: A `json` object identical to the one returned by 
[connect](#connect). It contains the following attributes : 
  - [`uint16_t`](#uint16_t) **handle**: The connection handle
  - [`GapRole`](#gaprole) **role**: Role of the device in the connection (here, it 
  should be central)
  - [`AddressType`](#addresstype) **peerAddrType**: The addressType of the peer
  - [`MacAddress`](#macaddress) **peerAddr**: The address of the peer
  - [`AddressType`](#addresstype) **ownAddrType**: The address type of this device
  - [`MacAddress`](#macaddress) **ownAddr**: The address of this device
  - `JSONDictionary` **connectionParams**: Contain the parameters of the connection:
    + [`uint16_t`](#uint16_t) **minConnectionInterval**: minimum connection interval 
    for this connection
    + [`uint16_t`](#uint16_t) **maxConnectionInterval**: maximum connection interval 
    for this connection
    + [`uint16_t`](#uint16_t) **slaveLatency**: slave latency of the connection
    + [`uint16_t`](#uint16_t) **connectionSupervisionTimeout**: supervision timeout 
    for this connection
* modeled after: `Gap::onConnection` and `Gap::onConnection`.


### disconnect

* invocation: `gap disconnect <connection_handle> <reason>`
* arguments: 
  - [`uint16_t`](#uint16_t) **handle**: The id of the connection to terminate.
  - [`DisconnectionReason`](#disconnectionreason) **reason**: The reason of the 
  termination.
* result: A `json` object containing the following attributes: 
  - [`uint16_t`](#uint16_t) **handle**: The id of the connection terminated.
  - [`DisconnectionReason`](#disconnectionreason) **reason**: The reason of the 
  termination.
* modeled after: `Gap::disconnect` and `Gap::onDisconnection`.


### getPreferredConnectionParams

* invocation: `gap getPreferredConnectionParams`
* arguments: None
* result: A `json` object containing the following attributes: 
  - [`uint16_t`](#uint16_t) **minConnectionInterval**: The preferred minimum 
  connection interval
  - [`uint16_t`](#uint16_t) **maxConnectionInterval**: The preferred maximum 
  connection interval
  - [`uint16_t`](#uint16_t) **slaveLatency**: The preferred slave latency 
  - [`uint16_t`](#uint16_t) **connectionSupervisionTimeout**: The preferred 
  supervision timeout
* modeled after: `Gap::getPreferredConnectionParams`


### setPreferredConnectionParams

* invocation: `gap setPreferredConnectionParams <minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>`
* arguments: All arguments are comma separated without space between them.
  - [`uint16_t`](#uint16_t) **minConnectionInterval**: The preferred minimum 
  connection interval
  - [`uint16_t`](#uint16_t) **maxConnectionInterval:** The preferred maximum 
  connection interval
  - [`uint16_t`](#uint16_t) **slaveLatency**: The preferred slave latency 
  - [`uint16_t`](#uint16_t) **connectionSupervisionTimeout**: The preferred 
  supervision timeout
* result: None
* modeled after: `Gap::setPreferredConnectionParams`


### setDeviceName

* invocation: `gap setDeviceName <name>`
* arguments: The name as a string (without quote). Spaces are not supported.
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
  - [`Appearance`](#appearance) **appearance**: The appearance of the device.
* result: None
* modeled after: `Gap::setAppearance`


### getAppearance

* invocation: `gap getAppearance`
* arguments: None.
* result: The appearance as a string. See [`Appearance`](#appearance) for 
possible values.
* modeled after: `Gap::getAppearance`


### setTxPower

* invocation: `gap setTxPower <power>`
* arguments: 
  - [`int8_t`](#int8_t) **power**: The transmission power to set.
* result: None
* modeled after: `Gap::setTxPower`


### getPermittedTxPowerValues

* invocation: `gap getPermittedTxPowerValues`
* arguments: None
* result: Array of the permitted Tx power values. Each element of the array is 
an [`int8_t`](#int8_t).
* modeled after: `Gap::getPermittedTxPowerValues`


### getState

* invocation: `gap getState`
* arguments: None
* result: The gap state of the device in a json object: 
  - [`bool`](#bool) **advertising**: true if the device is advertising and false 
  otherwise.
  - [`bool`](#bool) **connected**: true if the device is connected and false 
  otherwise.
* modeled after: `Gap::getState`


### setAdvertisingType

* invocation: `gap setAdvertisingType <advertising_type>`
* arguments: 
  - [`AdvertisingType`](#advertisingtype) **advertising_type**: The type of 
  advertising to set.
* result: None
* modeled after: `Gap::setAdvertisingType`


### setAdvertisingInterval

* invocation: `gap setAdvertisingInterval <advertising_interval>`
* arguments: 
  - [`uint16_t`](#uint16_t) **advertising_interval**: The interval between two 
  advertisements.
* result: None
* modeled after: `Gap::setAdvertisingInterval`


### setAdvertisingTimeout

* invocation: `gap setAdvertisingTimeout <advertising_timeout>`
* arguments: 
  - [`uint16_t`](#uint16_t) **advertising_timeout**: The duration of advertisements 
  in seconds. The value should be in the range [0: 0x3FFF]. A value of 0 disable 
  the timeout.
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
  - [`AdvertisingDataType`](#advertisingdatatype) **field_type**: The type of field 
  to add to the advertising payload.
  - **field_argument**: The type of the field argument depend on the type of field 
  to add: 

    | **Field type**                       | **Data type**                                                 |
    |--------------------------------------|---------------------------------------------------------------|
    | `FLAGS`                              | list of [`GapAdvertisingDataFlags`](#advertisingdataflags) |
    | `INCOMPLETE_LIST_16BIT_SERVICE_IDS`  | list of [`uint16_t`](#uint16_t)                               |
    | `COMPLETE_LIST_16BIT_SERVICE_IDS`    | list of [`uint16_t`](#uint16_t)                               |
    | `INCOMPLETE_LIST_32BIT_SERVICE_IDS`  | list of [`uint32_t`](#uint32_t)                               |
    | `COMPLETE_LIST_32BIT_SERVICE_IDS`    | list of [`uint32_t`](#uint32_t)                               |
    | `INCOMPLETE_LIST_128BIT_SERVICE_IDS` | list of [`UUID`](#uuid)                                       |
    | `COMPLETE_LIST_128BIT_SERVICE_IDS`   | list of [`UUID`](#uuid)                                       |
    | `SHORTENED_LOCAL_NAME`               | [`String`](#string)                                           |
    | `COMPLETE_LOCAL_NAME`                | [`String`](#string)                                           |
    | `TX_POWER_LEVEL`                     | [`int8_t`](#int8_t)                                           |
    | `DEVICE_ID`                          | not implemented                                               |
    | `SLAVE_CONNECTION_INTERVAL_RANGE`    | <interval min [`uint16_t`](#uint16_t)> <interval max [`uint16_t`](#uint16_t)>                                                             |
    | `SERVICE_DATA`                       | not implemented                                               |
    | `APPEARANCE`                         | [`Appearance`](#appearance)                                   |
    | `ADVERTISING_INTERVAL`               | [`uint16_t`](#uint16_t)                                       |
    | `MANUFACTURER_SPECIFIC_DATA`         | [`HexString`](#hexstring)                                    |
* result: None.
* modeled after: `Gap::accumulateAdvertisingPayload`



### updateAdvertisingPayload

* invocation: `gap updateAdvertisingPayload <field_type> <field_argument>`
* arguments: 
  - [`AdvertisingDataType`](#advertisingdatatype) **field_type**: The type of field 
  to update in the advertising payload.
  - **field_argument**: The type of the field argument depend on the type of field 
  to add: 

    | **Field type**                       | **Data type**                                                 |
    |--------------------------------------|---------------------------------------------------------------|
    | `FLAGS`                              | list of [`GapAdvertisingDataFlags`](#advertisingdataflags) |
    | `INCOMPLETE_LIST_16BIT_SERVICE_IDS`  | list of [`uint16_t`](#uint16_t)                               |
    | `COMPLETE_LIST_16BIT_SERVICE_IDS`    | list of [`uint16_t`](#uint16_t)                               |
    | `INCOMPLETE_LIST_32BIT_SERVICE_IDS`  | list of [`uint32_t`](#uint32_t)                               |
    | `COMPLETE_LIST_32BIT_SERVICE_IDS`    | list of [`uint32_t`](#uint32_t)                               |
    | `INCOMPLETE_LIST_128BIT_SERVICE_IDS` | list of [`UUID`](#uuid)                                       |
    | `COMPLETE_LIST_128BIT_SERVICE_IDS`   | list of [`UUID`](#uuid)                                       |
    | `SHORTENED_LOCAL_NAME`               | [`String`](#string)                                           |
    | `COMPLETE_LOCAL_NAME`                | [`String`](#string)                                           |
    | `TX_POWER_LEVEL`                     | [`int8_t`](#int8_t)                                           |
    | `DEVICE_ID`                          | not implemented                                               |
    | `SLAVE_CONNECTION_INTERVAL_RANGE`    | <interval min [`uint16_t`](#uint16_t)> <interval max [`uint16_t`](#uint16_t)>                                                             |
    | `SERVICE_DATA`                       | not implemented                                               |
    | `APPEARANCE`                         | [`Appearance`](#appearance)                                   |
    | `ADVERTISING_INTERVAL`               | [`uint16_t`](#uint16_t)                                       |
    | `MANUFACTURER_SPECIFIC_DATA`         | [`HexString`](#hexstring)                                    |
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
  - [`AdvertisingDataType`](#advertisingdatatype) **field_type**: The type of field 
  to add to the scan response  payload.
  - **field_argument**: The type of the field argument depend on the type of field 
  to add: 

    | **Field type**                       | **Data type**                                                 |
    |--------------------------------------|---------------------------------------------------------------|
    | `FLAGS`                              | list of [`GapAdvertisingDataFlags`](#advertisingdataflags) |
    | `INCOMPLETE_LIST_16BIT_SERVICE_IDS`  | list of [`uint16_t`](#uint16_t)                               |
    | `COMPLETE_LIST_16BIT_SERVICE_IDS`    | list of [`uint16_t`](#uint16_t)                               |
    | `INCOMPLETE_LIST_32BIT_SERVICE_IDS`  | list of [`uint32_t`](#uint32_t)                               |
    | `COMPLETE_LIST_32BIT_SERVICE_IDS`    | list of [`uint32_t`](#uint32_t)                               |
    | `INCOMPLETE_LIST_128BIT_SERVICE_IDS` | list of [`UUID`](#uuid)                                       |
    | `COMPLETE_LIST_128BIT_SERVICE_IDS`   | list of [`UUID`](#uuid)                                       |
    | `SHORTENED_LOCAL_NAME`               | [`String`](#string)                                           |
    | `COMPLETE_LOCAL_NAME`                | [`String`](#string)                                           |
    | `TX_POWER_LEVEL`                     | [`int8_t`](#int8_t)                                           |
    | `DEVICE_ID`                          | not implemented                                               |
    | `SLAVE_CONNECTION_INTERVAL_RANGE`    | <interval min [`uint16_t`](#uint16_t)> <interval max [`uint16_t`](#uint16_t)>                                                             |
    | `SERVICE_DATA`                       | not implemented                                               |
    | `APPEARANCE`                         | [`Appearance`](#appearance)                                   |
    | `ADVERTISING_INTERVAL`               | [`uint16_t`](#uint16_t)                                       |
    | `MANUFACTURER_SPECIFIC_DATA`         | [`HexString`](#hexstring)                                    |
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
  - [`uint16_t`](#uint16_t) **interval**: The scan interval, it should be a number 
  between 3 and 10420ms.
  - [`uint16_t`](#uint16_t) **window**: The scan window, it should be a number 
  between 3 and 10420ms.
  - [`uint16_t`](#uint16_t) **timeout**: The scan timeout, it should be a number 
  between 0 and 65534.
  - [`bool`](#bool) **activeScanning**: Enable or disable scan requests.
* result: None.
* modeled after: `Gap::setScanParam`


### setScanInterval

* invocation: `gap setScanInterval <interval> <window> <timeout> <activeScanning>`
* arguments: 
  - [`uint16_t`](#uint16_t) **interval**: The scan interval, it should be a number 
  between 3 and 10420ms.
* result: None.
* modeled after: `Gap::setScanInterval`


### setScanWindow

* invocation: `gap setScanWindow <window>`
* arguments: 
  - [`uint16_t`](#uint16_t) **window**: The scan window, it should be a number 
  between 3 and 10420ms.
* result: None.
* modeled after: `Gap::setScanWindow`


### setScanTimeout

* invocation: `gap setScanTimeout <timeout>`
* arguments: 
  - [`uint16_t`](#uint16_t) **timeout**: The scan timeout, it should be a number 
  between 0 and 65534.
* result: None.
* modeled after: `Gap::setScanTimeout`


### setActiveScanning

* invocation: `gap setActiveScanning <activeScanning>`
* arguments: 
  - [`bool`](#bool) **activeScanning**: Enable or disable scan requests.
* result: None.
* modeled after: `Gap::setActiveScanning`


### startScan

* invocation: `gap startScan <duration> <target_address>`
* arguments: 
  - [`uint16_t`](#uint16_t) **duration**: Duration of the scan procedure.
  - [`MacAddress`](#macaddress) **target_address**: Address of the peer to scan for.
* result: A `JSON` object array of scan records. Each records is a 
JSON object which contains the following attributes: 
  - [`MacAddress`](#macaddress) **peerAddr**: The address of the peer.
  - [`int8_t`](#int8_t) **rssi**: The RSSI for this record.
  - [`bool`](#bool) **isScanResponse**: true if the record is a response to a scan 
  request and false otherwise.
  - [`AdvertisingType`](#advertisingtype) **type**: The type of advertising.
  - [`AdvertisingPayload`](#advertisingpayload) **data**: The content of the 
  advertising payload received.
  - [`int32_t`](#int32_t) **time**: The time at which the record was captured. The 
  time start when the procedure is started.
* modeled after: `Gap::startScan` and `Gap::stopScan`.


### getAdvertisingParams

* invocation: `gap getAdvertisingParams`
* arguments: None.
* result: A `JSON` object with the following fields: 
  - [`uint16_t`](#uint16_t) **interval**: Advertising interval.
  - [`uint16_t`](#uint16_t) **timeout**: Advertisement timeout.
  - [`AdvertisingType`](#advertisingtype) **type**: type of advertising.
* modeled after: `Gap::getAdvertisingParams`


### setAdvertisingParams

* invocation: `gap setAdvertisingParams <advertising_type> <interval> <timeout>`
* arguments: 
  - [`AdvertisingType`](#advertisingtype) **advertising_type**: type of advertising.
  - [`uint16_t`](#uint16_t) **interval**: Advertising interval, it should be a 
  number between 0 and 65534".
  - [`uint16_t`](#uint16_t) **timeout**: Advertisement timeout, it should be a 
  number between 0 and 65534".
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
* result: A JSON array containing the list of devices in the whitelist. Each 
devices is a JSON object with the following attributes: 
  - [`AddressType`](#addresstype) **address_type**: The type of the address of the 
  device.
  - [`MacAddress`](#macaddress) **address**: The mac address of the device.
* modeled after: `Gap::getWhitelist`


### setWhitelist

* invocation: `gap setWhitelist [<address_type> <address>]`
* arguments: list of pair of address type and address: 
  - [`AddressType`](#addresstype) **address_type**: The type of the address 
  - [`MacAddress`](#macaddress) **address**: The MAC address associated. 
* result: None
* modeled after: `Gap::setWhitelist`


### setAdvertisingPolicyMode

* invocation: `gap setAdvertisingPolicyMode <advertising_policy_mode>`
* arguments: 
  - [`AdvertisingPolicyMode`](#advertisingpolicymode) 
  **advertising_policy_mode**: The policy mode to use during advertising. 
* result: None
* modeled after: `Gap::setAdvertisingPolicyMode`


### setScanningPolicyMode

* invocation: `gap setScanningPolicyMode <scanning_policy_mode>`
* arguments: 
  - [`ScanningPolicyMode`](#scanningpolicymode) **scanning_policy_mode**: The 
  policy to use during scanning. 
* result: None
* modeled after: `Gap::setScanningPolicyMode`


### setInitiatorPolicyMode

* invocation: `gap setInitiatorPolicyMode <initiator_policy_mode>`
* arguments: 
  - [`InitiatorPolicyMode`](#initiatorpolicymode) **initiator_policy_mode**: The 
  policy mode to use during connection initiation.
* result: None
* modeled after: `Gap::setInitiatorPolicyMode`



### getAdvertisingPolicyMode

* invocation: `gap getAdvertisingPolicyMode`
* arguments: None
* result: [`AdvertisingPolicyMode`](#advertisingpolicymode) 
* modeled after: `Gap::getAdvertisingPolicyMode`


### getScanningPolicyMode

* invocation: `gap getScanningPolicyMode <scanning_policy_mode>`
* arguments: None
* result: [`ScanningPolicyMode`](#scanningpolicymode) 
* modeled after: `Gap::getScanningPolicyMode`


### getInitiatorPolicyMode

* invocation: `gap getInitiatorPolicyMode <initiator_policy_mode>`
* arguments: None
* result: [`InitiatorPolicyMode`](#initiatorpolicymode)
* modeled after: `Gap::getInitiatorPolicyMode`




## gattClient module

The `gattClient` module expose the following functions from the class 
`GattClient`:


### discoverAllServicesAndCharacteristics

* invocation: `gattClient discoverAllServicesAndCharacteristics <connection_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
* result: A JSON array of the discovered service. Each service is a JSON object 
which contains the following fields:
  - [`UUID`](#uuid) **UUID**: The UUID of the service.
  - [`uint16_t`](#uint16_t) **start_handle**: The first attribute handle of the 
  service.
  - [`uint16_t`](#uint16_t) **end_handle**: The last attribute handle of the 
  service.
  - **characteristics**: an array of JSON objects describing the characteristics 
  of the service. Each characteristics expose the following attributes:
    + [`UUID`](#uuid) **UUID**: The UUID of the characteristic.
    + **properties**: JSON array of the properties of the characteristics, each 
    property is modeled as a string:
      * "broadcast"
      * "read"
      * "writeWoResp"
      * "write"
      * "notify"
      * "indicate"
      * "authSignedWrite"
    + [`uint16_t`](#uint16_t) **start_handle**: The first attribute handle of the 
    characteristic.
    + [`uint16_t`](#uint16_t) **value_handle**: The handle used to retrieve the 
    value of the characteristic.
    + [`uint16_t`](#uint16_t) **end_handle**: The last attribute handle of the 
    characteristic.
* modeled after: `GattClient::launchServiceDiscovery`, 
`GattClient::onServiceDiscoveryTermination`.


### discoverAllServices

* invocation: `gattClient discoverAllServices <connection_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used 
   by the procedure.
* result: A JSON array of the discovered service. Each service is a JSON object 
which contains the following fields:
  - [`UUID`](#uuid) **UUID**: The UUID of the service.
  - [`uint16_t`](#uint16_t) **start_handle**: The first attribute handle of the 
  service.
  - [`uint16_t`](#uint16_t) **end_handle**: The last attribute handle of the 
  service.
* modeled after: `GattClient::launchServiceDiscovery`, 
`GattClient::onServiceDiscoveryTermination`.



### discoverPrimaryServicesByUUID

* invocation: `gattClient discoverPrimaryServicesByUUID <connection_handle> 
<service_UUID>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used 
   by the procedure.
   - [`UUID`](#uuid) **service_UUID**: The UUID of the service to discover.
* result: A JSON array of the discovered service. Each service is a JSON object 
which contains the following fields:
  - [`UUID`](#uuid) **UUID**: The UUID of the service.
  - [`uint16_t`](#uint16_t) **start_handle**: The first attribute handle of the 
  service.
  - [`uint16_t`](#uint16_t) **end_handle**: The last attribute handle of the 
  service.
* modeled after: `GattClient::launchServiceDiscovery`, 
`GattClient::onServiceDiscoveryTermination`.



### discoverAllCharacteristicsDescriptors

* invocation: `gattClient discoverAllCharacteristicsDescriptors 
<connection_handle> <char_start> <char_end>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used 
   by the procedure.
   - [`uint16_t`](#uint16_t) **char_start**: The first attribute handle of the 
   characteristic targeted by the operation.
   - [`uint16_t`](#uint16_t) **char_end**: The last attribute handle of the 
   characteristic targeted by the operation.
* result: A JSON array of the discovered descriptors. Each discovered descriptor 
is a JSON object which contains the following fields:
  - [`uint16_t`](#uint16_t) **handle**: Attribute handle of the descriptor.
  - [`UUID`](#uuid) **UUID**: The UUID of the characteristic descriptor.
* modeled after: `GattClient::discoverCharacteristicDescriptors`



### readCharacteristicValue

* invocation: `gattClient readCharacteristicValue <connection_handle> <char_value_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
   - [`uint16_t`](#uint16_t) **char_value_handle**: The attribute handle of the 
   value to read.
* result: The value of the characteristic, as an [`HexString`](#hexstring).
* modeled after: `GattClient::read` and `GattClient::onDataRead`


### writeWithoutResponse

* invocation: `gattClient writeWithoutResponse <connection_handle> <char_value_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
   - [`uint16_t`](#uint16_t) **char_value_handle**: The attribute handle of the 
   value to read.
   - [`HexString`](#hexstring) **value**: The value to write in the characteristic.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`



### write

* invocation: `gattClient write <connection_handle> <char_value_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
   - [`uint16_t`](#uint16_t) **char_value_handle**: The attribute handle of the 
   value to write.
   - [`HexString`](#hexstring) **value**: The value to write in the characteristic.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`


### readCharacteristicDescriptor

* invocation: `gattClient readCharacteristicDescriptor <connection_handle> <descriptor_handle>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
   - [`uint16_t`](#uint16_t) **descriptor_handle**: The attribute handle of the 
   descriptor to read.
* result: The value of the descriptor, as an [`HexString`](#hexstring).
* modeled after: `GattClient::read` and `GattClient::onDataRead`


### write

* invocation: `gattClient writeCharacteristicDescriptor <connection_handle> <descriptor_handle> <value>`
* arguments: 
   - [`uint16_t`](#uint16_t) **connection_handle**: The connection handle used by 
   the procedure.
   - [`uint16_t`](#uint16_t) **descriptor_handle**: The attribute handle of the 
   descriptor to write.
   - [`HexString`](#hexstring) **value**: The value to write in the descriptor.
* result: None
* modeled after: `GattClient::write` and `GattClient::onDataWritten`


### listenHVX

* invocation: `gattClient listenHVX <timeout>`
* arguments: 
   - [`uint16_t`](#uint16_t) **timeout**: Time to listen to server notification 
   or indication.
* result: The list of notifications or indication received from the server. 
Each record contains the following attributes: 
  - [`uint16_t`](#uint16_t) **connHandle**: The reference of the connection to 
  the GATT server which has issued the event.
  - [`uint16_t`](#uint16_t) **handle**: The GATT attribute which has initiated 
  the event.
  - [`HVXType_t`](#hvxtype_t) **type**: Type of the event (notification or 
  indication).
  - [`HexString`](#hexstring) **data**: Payload of the event.
* modeled after: `GattClient::onHVX` 



## gattServer module

The `gattServer` module allows a user to access the capabilities of the 
GattServer class but building a service is a bit different with this module than 
it is with the original class. Rather than registering a service all at once, 
user can split the declaration in multiple sub operations then commit the result 
in the GattServer when it is ready.

### declareService

* invocation: `gattServer declareService <service_uuid>`
* description: Start the declaration of a service, after this call, user can call:
  - `declareCharacteristic` to declare a characteristic inside the service, 
  - `commitService` to commit the service or 
  - `cancelServiceDeclaration` to cancel the service declaration.
* arguments: 
   - [`UUID`](#uuid) **service_uuid**: The UUID of the service declared.
* result: None


### declareCharacteristic

* invocation: `gattServer declareCharacteristic <characteristic_uuid>`
* description: Start the declaration of a characteristic, after this call, user 
can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`UUID`](#uuid) **characteristic_uuid**: The UUID of the characteristic 
   declared.
* result: None


### setCharacteristicValue

* invocation: `gattServer setCharacteristicValue <value>`
* description: Set the value of the characteristic being declared. After this 
call, user can call:
  - `setCharacteristicValue` to set the value of this to another value.
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`HexString`](#hexstring) **value**: The value of the characteristic 
   declared.
* result: None



### setCharacteristicProperties

* invocation: `gattServer setCharacteristicProperties [properties]`
* description: Set the properties of the characteristic being declared. After 
this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic 
  to another value.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
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
* description: Set a boolean value which indicate if the characteristic has a 
variable length. If the characteristic has a variable length then 
`setCharacteristicMaxLength`  could be call to bound the length to a maximum.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`bool`](#bool) **enable**: True to enable variable length and false to 
   enable fixed length.
* result: None


### setCharacteristicMaxLength

* invocation: `gattServer setCharacteristicMaxLength <max_length>`
* description: Set the maximum length of the value of the characteristic being 
declared.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`uint16_t`](#uint16_t) **max_length**: The max length that the characteristic 
   value can occupy.
* result: None


### declareDescriptor

* invocation: `gattServer declareDescriptor <descriptor_uuid>`
* description: Start the declaration of a descriptor which will be attached to 
the characteristic being declared.
After this call, user can call:
  - `setCharacteristicValue` to set the value of this characteristic
  - `setDescriptorVariableLength`  enable or disable variable length value of 
  this descriptor.
  - `setDescriptorMaxLength` set the maximum length that is allowed for the 
  value of this descriptor.
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to start the declaration of another descriptor inside 
  this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`UUID`](#uuid) **max_length**: The max length that the characteristic 
   value can occupy.
* result: None


### setDescriptorValue

* invocation: `gattServer setDescriptorValue <value>`
* description: Set the value of the descriptor being declared. After this call, 
user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of 
  this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the 
  value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicProperties` to set the properties of this characteristic
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`HexString`](#hexstring) **value**: The value of the descriptor declared.
* result: None


### setDescriptorVariableLength

* invocation: `gattServer setDescriptorVariableLength <enable>`
* description: Set a boolean value which indicate if the descriptor has a 
variable length. If the descriptor has a variable length then 
`setDescriptorMaxLength` could be call to bound the length to a maximum.
After this call, user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of 
  this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the 
  value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`bool`](#bool) **enable**: True to enable variable length and false to enable 
   fixed length.
* result: None


### setDescriptorMaxLength

* invocation: `gattServer setDescriptorMaxLength <max_length>`
* description: Set the maximum length of the value of the descriptor being 
declared.
After this call, user can call:
  - `setDescriptorVariableLength`  enable or disable variable length value of 
  this descriptor. 
  - `setDescriptorMaxLength` set the maximum length that is allowed for the 
  value of this descriptor.
  - `setDescriptorValue` to set the value of this to another value.
  - `setCharacteristicValue` to set the value of this characteristic
  - `setCharacteristicProperties` to set the properties of this characteristic.
  - `setCharacteristicVariableLength` enable or disable variable length value of 
  this characteristic.
  - `setCharacteristicMaxLength` set the maximum length that is allowed for the 
  value of this characteristic.
  - `declareCharacteristic` to declare another characteristic inside the service
  - `declareDescriptor` to add a descriptor inside this characteristic
  - `commitService` to commit the service 
  - `cancelServiceDeclaration` to cancel the service declaration 
* arguments: 
   - [`uint16_t`](#uint16_t) **max_length**: The max length that the descriptor 
   value can occupy.
* result: None


### commitService

* invocation: `gattServer commitService`
* description: Commit in the GattServer the service declared. After this call, 
the ongoing service declaration is reset.
* arguments: None
* result: A JSON object describing the service in the ATT DB. It contains the 
following fields:
  - [`UUID`](#uuid) **UUID**: The uuid of the service
  - [`uint16_t`](#uint16_t) **handle**: The attribute handle of the service. 
  - "characteristics": A JSON array of the characteristics presents in the 
  service. Each characteristic is a JSON object containing the following fields:
    + [`UUID`](#uuid) **UUID**: The uuid of the characteristic.
    + [`uint16_t`](#uint16_t) **value_handle**: The attribute handle of the value 
    of the characteristic. 
    + "properties": A JSON array of the properties of the characteristic. The 
    possible values are:
      * "broadcast"
      * "read"
      * "writeWoResp"
      * "write"
      * "notify"
      * "indicate"
      * "authSignedWrite" 
    + [`uint16_t`](#uint16_t) **length**: The length of the value of the 
    characteristic.
    + [`uint16_t`](#uint16_t) **max_length**: The maximum length that the value 
    of the characteristic can take.
    + [`bool``](#bool`) **has_variable_length**: Flag indicating if the length of 
    the characteristic is fixed or variable.
    + [`HexString`](#hexstring) **value**: The value of the characteristic.
    + "descriptors": A JSON array containing the descriptors of the 
    characteristic:
      * [`UUID`](#uuid) **UUID**: The uuid of the descriptor.
      * [`uint16_t`](#uint16_t) **handle**: The attribute handle of the descriptor. 
      * [`uint16_t`](#uint16_t) **length**: The length of the value of the descriptor.
      * [`uint16_t`](#uint16_t) **max_length**: The maximum length that the value 
      of the descriptor can take.
      * [`bool``](#bool`) **has_variable_length**: Flag indicating if the length 
      of the descriptor is fixed or variable
      * [`HexString`](#hexstring) **value**: The value of the descriptor.
* modeled after: `GattServer::addService`


### cancelServiceDeclaration

* invocation: `gattServer cancelServiceDeclaration`
* description: Cancel the current service declaration.
* arguments: None
* result: None



### read

* invocation: `gattServer read <attribute_handle> <connection_handle>`
* description: Read the value an attribute handle of the gatt server. The 
connection_handle parameter is optional.
* arguments: 
  - [`uint16_t`](#uint16_t) **attribute_handle**: The attribute handle to read. 
  - [`uint16_t`](#uint16_t) **connection_handle**: Optional parameter, useful to 
  read the value of CCCD for a given connection.
* result: 
  - [`HexString`](#hexstring) The value of the attribute.
* modeled after: `GattServer::read`


### write

* invocation: `gattServer write <attribute_handle> <value> <connection_handle>`
* description: Write the value an attribute handle of the gatt server. The 
connection_handle parameter is optional.
* arguments: 
  - [`uint16_t`](#uint16_t) **attribute_handle**: The attribute handle to write. 
  - [`HexString`](#hexstring) **value**: The value to write to the attribute.
  - [`uint16_t`](#uint16_t) **connection_handle**: Optional parameter, useful to 
  write the value of CCCD for a given connection.
* result: None
* modeled after: `GattServer::write`


### waitForDataWritten

* invocation: `gattServer waitForDataWritten <connection_handle> <attribute_handle> <timeout>`
* description: Wait for data of an attribute to be written.
* arguments: 
  - [`uint16_t`](#uint16_t) **connection_handle**: Handle of the connection 
  issuing the write request.
  - [`uint16_t`](#uint16_t) **attribute_handle**: The attribute handle to monitor. 
  - [`uint16_t`](#uint16_t) **timeout**: Maximum time allowed to this procedure; 
  in ms. 
* result: None
* modeled after: `GattServer::onDataWritten`




## securityManager module

The `securityManager` module allows a user to access capabilities of the 
SecurityManager class.


### init

* invocation: `securityManager init <enable_bonding> <require_mitm> 
<io_capabilities> <passkey>`
* description: Initialize and configure the security manager.
* arguments: 
  - [`bool`](#bool) **enable_bonding**: Allow or disallow bonding to the device.
  - [`bool`](#bool) **require_mitm**: Allow or disallow MITM protection.
  - [`SecurityIOCapabilities`](#io-capabilities) **io_capabilities**: I/O 
  capabilities of the device.
  - [`Passkey`](#passkey) **passkey**: The static passkey used by the security 
  manager.
* result: None
* modeled after: `SecurityManager::init`


### getAddressesFromBondTable

* invocation: `securityManager getAddressesFromBondTable <addresses_count>`
* description: Get the addresses from the bond table.
* arguments: 
  - [`uint8_t`](#uint8_t) **addresses_count**: The maximum number of addresses to 
  get. 
* result: A JSON array containing the addresses in the bond table. Each of this 
record is a JSON object containing the following fields:
  - [`AddressType`](#addresstype) **address_type**: The type of the address.
  - [`MacAddress`](#macaddress) **address**: The mac address.
* modeled after: `SecurityManager::getAddressesFromBondTable`


### purgeAllBondingState

* invocation: `securityManager purgeAllBondingState`
* description: Delete all peer device context and all related bonding 
information from the database within the security manager.
* arguments: None
* result: None
* modeled after: `SecurityManager::purgeAllBondingState`





# Data types format: 

Every parameter used by a CLI commands is typed. This is the list of type 
recognized by ble-cliapp.


## Primitive types 

### bool

Boolean type, ble cliapp accept "true" or "false" as valid values.

* model `bool`

### int8_t

Signed integer on 8 bits. The application accept decimal and hexadecimal 
(0x...) representations.

* model `int8_t`

### uint8_t

Unsigned 8 bits integer. The application accept decimal and hexadecimal (0x...) 
representations.

* model `uint8_t`

### uint16_t

unsigned integer on 16 bits. The application accept decimal and hexadecimal 
(0x...) representations.

* model `uint16_t`

### uint32_t

unsigned integer on 32 bits. The application accept decimal and hexadecimal 
(0x...) representations.

* model `uint32_t`

### String

Model an ASCII string without quotes; spaces are not accepted at the moment.

### HexString

Hexadecimal representation for raw data. Each byte is represented as its two 
hexadecimal characters.

## BLE 

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

This type model a mac address, it is always represented a string: 
`XX:XX:XX:XX:XX:XX` where `X` is an hexadecimal character.

* model `Gap::Address_t`

### UUID 

* model the class `UUID`.

The following formats are supported: 

* `0xXXXX` for 16 bits UUID constructed from an hex value
* `XXXXX` for 16 bits UUID constructed from a 16 bit number.
* `XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX` for 128 bits UUID

## Gap

### GapRole

* model `Gap::Role_t`.

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


### AdvertisingDataFlags 

* model `GapAdvertisingData::Flags_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingData::LE_LIMITED_DISCOVERABLE | "LE_LIMITED_DISCOVERABLE" | 
| GapAdvertisingData::LE_GENERAL_DISCOVERABLE | "LE_GENERAL_DISCOVERABLE" | 
| GapAdvertisingData::BREDR_NOT_SUPPORTED | "BREDR_NOT_SUPPORTED" | 
| GapAdvertisingData::SIMULTANEOUS_LE_BREDR_C | "SIMULTANEOUS_LE_BREDR_C" | 
| GapAdvertisingData::SIMULTANEOUS_LE_BREDR_H | "SIMULTANEOUS_LE_BREDR_H" |  


### AdvertisingType

* model `GapAdvertisingParams::AdvertisingType_t`.

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED | "ADV_CONNECTABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED | "ADV_CONNECTABLE_DIRECTED" |
| GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED | "ADV_SCANNABLE_UNDIRECTED" |
| GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED | "ADV_NON_CONNECTABLE_UNDIRECTED" |


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



### AdvertisingPayload

A JSON object of the advertising payload: 
* **FLAGS**: JSON array of [`GapAdvertisingDataFlags`](#advertisingdataflags).
* **INCOMPLETE_LIST_16BIT_SERVICE_IDS** : JSON array of [`uint16_t`](#uint16_t) 
in the hex form (0xXXXX).
* **COMPLETE_LIST_16BIT_SERVICE_IDS** : JSON array of  [`uint16_t`](#uint16_t) 
in the hex form (0xXXXX).
* **INCOMPLETE_LIST_32BIT_SERVICE_IDS** : JSON array of [`uint32_t`](#uint32_t) 
in the hex form (0xXXXXXXXX).
* **COMPLETE_LIST_32BIT_SERVICE_IDS** : JSON array of [`uint32_t`](#uint32_t) in 
the hex form (0xXXXXXXXX).
* **SHORTENED_LOCAL_NAME** : [`String`](#string)
* **COMPLETE_LOCAL_NAME** : [`String`](#string)
* **MANUFACTURER_SPECIFIC_DATA** : [`HexString`](#hexstring)
* **raw**: The complete advertising payload in an [`HexString`](#hexstring).


## SecurityManager

### Passkey 

* model `SecurityManager::Passkey_t`. 

It is a string of 6 digits.

### IO Capabilities

* model `SecurityManager::SecurityIOCapabilities_t`. 

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| SecurityManager::IO_CAPS_DISPLAY_ONLY | "IO_CAPS_DISPLAY_ONLY" | 
| SecurityManager::IO_CAPS_DISPLAY_YESNO | "IO_CAPS_DISPLAY_YESNO" | 
| SecurityManager::IO_CAPS_KEYBOARD_ONLY | "IO_CAPS_KEYBOARD_ONLY" | 
| SecurityManager::IO_CAPS_NONE | "IO_CAPS_NONE" | 
| SecurityManager::IO_CAPS_KEYBOARD_DISPLAY | "IO_CAPS_KEYBOARD_DISPLAY"| 


## GattClient

### HVXType_t

* model `HVXType_t`

| C++                                              | ble-cliapp                                    |
|--------------------------------------------------|-----------------------------------------------|
| BLE_HVX_NOTIFICATION | "BLE_HVX_NOTIFICATION"  |
| BLE_HVX_INDICATION | "BLE_HVX_INDICATION"  |



# Extending the application

Extending the application means adding new command to existing modules and/or 
adding new command modules. Before diving in the extension of the application, 
it is important to understand how the application is structured and what it 
contains.

## Organization 

The application is divided in software modules which can be split into two 
categories: 

* Application specific code: This code is specific to ble-cliapp application.
* Reusable code: These component are not specific to ble-cliapp and it should be 
possible to reuse them in other projects. At some point they might move outside 
ble-cliapp in their own repository.

All these software modules are contained in the [`source`](source) folder.

### Reusable code

Four different modules can be considered as reusable: 

* `CLICommand`: C++ framework built on top of `mbed-cli-command`, it contains 
all the necessary primitives to create commands, group them into modules and 
register the modules in the system. 
* `EventQueue`: An implementation of a cross platform event queue. 
* `Serialization`: A very simple serialization framework which convert string 
values to C++ value and vice versa. It also contains a class which can format 
and stream JSON values. 
* `util`: Few utility classes.

### Application code:

The application specific code is located in the [`Commands`](source/Commands) 
folder. This module contains all the commands available in ble-cliapp. These 
commands are grouped into module where each of these module reflect a module of 
mbed BLE: 

- [`ble`](source/Commands/BLECommands.cpp): See [ble module](#ble-module).
- [`gap`](source/Commands/GapCommands.cpp): See [gap module](#gap-module).
- [`gattClient`](source/Commands/GattClientCommands.cpp): See 
[gattClient module](#gattclient-module).
- [`gattServer`](source/Commands/GattServerCommands.cpp): See 
[gattServer module](#gattserver-module).
- [`securityManager`](source/Commands/SecurityManagerCommands.cpp): See 
[securityManager module](#securitymanager-module).

Serialization of mbed BLE specific type is located in the 
[Serialization](source/Commands/Serialization) subfolder 

Finaly, the file [`main.cpp`](source/main.cpp) glue the different part of the 
system together.

All code extending the application should be added in the Commands folder or 
main.cpp for the glue logic.


## Command creation

### Command concepts

A command is just an instance of the [Command structure](source/CLICommand/Command.h). 
The structure is collection of six function pointers. One of these function 
pointer is the command handler itself while the other five of those functions 
describe the command with metadatas.
These metadatas are used by the system to retrieve the command, help the user or 
validate arguments on the command line. 

* `const char* name()`: Should return the name of the Command. 
It is used by the system to retrieve and invoke the right command handler from 
the command line. It should **not** contain spaces.
* `const char* help()`: It is the help of the command.
* `ConstArray<CommandArgDescription> argsDescription()`: Description of the 
arguments expected by the command. The list of argument will be displayed when 
user asked for the help of the command.
* `ConstArray<CommandArgDescription> resultDescription()`: Description of the 
result returned by the command.
* `std::size_t maximumArgsRequired()`: Return the maximum number of arguments 
that the command can handle. This parameter is used to prefilter invalid command 
requests and  reduce the number of parameters that has to be checked by the 
handler. If the command line in input contains more arguments than the maximum 
number of arguments expected by the command, the command is not invoked and the 
system returns INVALID_PARAMETERS instead.  
* `void handler(const CommandArgs& args, const CommandResponsePtr& res)`: The 
actual command handler. It accept two arguments: 
  + args: Arguments pushed by the handler on the command line. The module name 
  and the command name are not included.
  + res: A CommandResponse object that the handler has to fullfill to answer the 
  command.



### Command declaration 

It is not advised to create command instances by hand. The prefered way is to 
define them with the helpers provided by 
[CLICommand/CommandHelper.h](source/CLICommand/CommandHelper.h). 

Goals of these helpers are: 
* Do not split Command declaration from command implementation to reduce the 
maintainance.
* Ease command construction. 
* Ensure correctness of the constness of the data structures.
* Reduce the boiler plate needed for the verification of the arguments from the 
command line.


#### Begining a command declaration 

To start a command declaration, use the macro `DECLARE_CMD(NAME)`. This will 
start a structure named `NAME` inheriting publicly from the structure 
`BaseCommand`. 
All elements of the command should go inside this structure. 

```c++
DECLARE_CMD(MyCommand) { 
    // elements of the command 
};
```

This is equivalent to 


```c++
struct MyCommand : public BaseCommand { 


};
```

#### Adding the command name 

It is easy to add the command name, either use the macro `CMD_NAME(QUOTED_NAME)` 
or create the static member function `const char* name()` inside the command 
being declared. The name of the command should **not** contains space.

```c++ 
DECLARE_CMD(MyCommand) { 
    CMD_NAME("my_command");
    // other parts of the command 
};
```

or 

```c++
struct MyCommand : public BaseCommand { 
    static const char* name() { 
        return "my_command"
    }
    // other parts of the command     
};
```

#### Adding help of the command 

It is not mandatory to add this function. If its not present, the system will 
use the implementation provided by the class `BaseCommand`. Once again the user 
have the choice to use a macros or declare the static member function by hand.


```c++ 
DECLARE_CMD(MyCommand) { 
    CMD_HELP("This is the help of my_command ....");
    // other parts of the command 
};
```

or 

```c++
struct MyCommand : public BaseCommand { 
    static const char* help() { 
        return "This is the help of my_command ...."
    }
    // other parts of the command     
};
```

#### Describing command parameters 

The declaration of command parameters is used for two things: 
* Complement the help of the command 
* Allow the system to automatically check that the number of arguments in input 
match the number of parameters expected by the command before invoking the 
actual command handler. 

Each parameters is described with three properties: 
* The type of the parametrer 
* The name of the parameter
* The description of the parameter 

Again it can be declared as a static function or using helper macros.

```c++ 
DECLARE_CMD(MyCommand) { 

    CMD_ARGS(
        CMD_ARG("<parameter type>", "<parameter name>", "<parameter description>"), 
        CMD_ARG("<parameter type>", "<parameter name>", "<parameter description>"),
        // other arguments  
    )

    // other parts of the command 
};
```

or 

```c++
struct MyCommand : public BaseCommand { 
    static ConstArray<CommandArgDescription> argsDescription() { 
        static const CommandArgDescription argsDescription[] = { 
            { "<parameter type>", "<parameter name>", "<parameter description>" },
            { "<parameter type>", "<parameter name>", "<parameter description>" },
            // other parameters ...
        }; 
        return ConstArray<CommandArgDescription>(argsDescription); 
    }
    // other parts of the command     
};
```

#### Describing command results 

Like the command parameters, this function provide a better help for the end 
user but unlike the command parameter the content of the command result does not 
have any effect at runtime on the command handler execution.


```c++ 
DECLARE_CMD(MyCommand) { 

    CMD_RESULTS(
        CMD_RESULT("<result type>", "<result name>", "<result description>"), 
        CMD_RESULT("<result type>", "<result name>", "<result description>"),
        // other results
    )

    // other parts of the command 
};
```

or 

```c++
struct MyCommand : public BaseCommand { 
    static ConstArray<CommandArgDescription> resultDescription() { 
        static const CommandArgDescription resultDescription[] = { 
            { "<result type>", "<result name>", "<result description>" },
            { "<result type>", "<result name>", "<result description>" },
            // other parameters ...
        }; 
        return ConstArray<CommandArgDescription>(resultDescription); 
    }
    // other parts of the command     
};
```

#### Command handler 

The command handler itself is achieved via the handler function:  

```c++
`void handler(const CommandArgs& args, const CommandResponsePtr& res)`
```

By default it accepts two parameters: 
* **args** which is just an array of strings matching containing the arguments 
of the command. 
* **res** the response of the command which is shared pointer to a 
`CommandResponse` object. The command should fullfill the content of the 
response by setting indicating if it succeed or fail and can send a JSON object 
as a result. 

The handler can be declared using an helper or directly as a static member 
function: 

```c++ 
DECLARE_CMD(MyCommand) { 

     CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) { 
        // check arguments 
        // do something with them
        // fullfill the response 
     } 

    // other parts of the command 
};
```

or

```c++ 
DECLARE_CMD(MyCommand) { 

     static void handler(const CommandArgs& args, CommandResponsePtr& response) { 
        // check arguments 
        // do something with them
        // fullfill the response 
     } 

    // other parts of the command 
};
```

> **Note:** It is not required to check the number of arguments in input of the 
command, the runtime take care of it before invoking the handler.


It is also possible to let the runtime handle the deserialization of the 
arguments of the Command handler automatically. It is required to change the 
signature of the Command handler by putting the expected parameters before the 
command response parameter. The deserialization function for the arguments type 
should also be available.

```c++
void handler(A0 arg0, A1 arg1, ..., CommandResponsePtr& response);
```

> **Note:** Up to 10 arguments are supported.

> **Note:** References and values are supported in input.


#### Command instantiation 

As stated earlier commands are instances of the Command class. Declaring a 
comamnd the way as described above does **not** create an instance of the 
Command structure. To get access to the Command instance of the command class 
created, it is either possible to use an helper or generate it using the 
template class `CommandGenerator`. 

```c++
static const Command* my_command = CMD_INSTANCE(MyCommand);
```

or 

```c++
static const Command* my_command = &CommandGenerator<MyCommand>::command;
```


#### Complete example 

As a complete example consider an addition command which add two `int32_t` 
numbers: 

```C++
#include <CLICommand/CommandHelper.h>

DECLARE_CMD(AdditionCommand) {
    CMD_NAME("add")
    CMD_HELP("Add two number togethers and returns the result.")
    CMD_ARGS(
      CMD_ARG("int32_t", "lhs", "The left hand side of the operation"),
      CMD_ARG("int32_t", "rhs", "The right hand side of the operation")
    )
    CMD_RESULTS(
      CMD_RESULT("int64_t", "", "The result of lhs + rhs.")
    )

    CMD_HANDLER(int32_t lhs, int32_t rhs, CommandResponsePtr& response) { 
      int64_t result = lhs + rhs;
      response->success(result);
    }
};
```

Once attached to a command module and the command module registered in the 
system, the command line user will be able to list this command from its module,
get a detailed help about it and of course invoke the command: 

```
<module name> add <lhs> <rhs>
```

#### Few words about the Command design 

It would have been simpler to use an abstract class as the base class for the  
command rather than static polymorphism. Unfortunatelly using an abstract base 
class for Commands force the usage of virtual functions. As soon as a class 
defines virtual functions it cannot be instantiated at compilation time in read 
only memory (FLASH) because it is not a POD. Consequently objects referencing 
instances of a non POD class will also be created at runtime, in RAM, after the 
instantiation of the objects it reference. 

Worse, if these objects are global one (like a command or a list of commands) 
they will be created before main by the C++ runtime. To destroy these globals at 
exit, the C++ runtime has to book keep in RAM what has been instantiated and 
how to destroy it. Depending on the compiler used, this might involve dynamic 
memory allocation.

At the end, a global empty command will consume between 28 and 40 bytes of 
RAM. For an application like ble-cliapp and its 77 commands it means that 
between 2156 and 3080 bytes of RAM just for storing commands. This was an issue 
on the most constrained targets like the NRF51.

Another technical issue encountered by the use of an abstract Command class 
design was the increase of the code size. With abstract class design, a pair 
of destructor will be generated and added to the vtable of each command, 
additional destruction function will also be generated to cleanup global command 
objects. It was the cause of flash exhaustion on the NRF51 target.

Using C++11 (initialy it was the case!) instead of C++98 would have solve the 
all the memory issues induced by the use of abstract base class as well as 
allowing us to provide a clearer command declaration. 

Unfortunatelly until IAR supports C++11, the application will remains stuck with 
C++98.


#### Asynchronous command 

CLICommand provide the base class 
[AsyncProcedure](source/CLICommand/util/AsyncProcedure.h) to help the developer 
to write commands requiring asynchronous operations. 

The class manages automatically the lifetime of the state associated with the 
operation, the command response and the timeout if the operation didn't succeed 
in the expected time.

Consider the following procedure as an example: 

```c++
struct MyLongProcedure : public AsyncProcedure {
    // Construct the procedure. 
    // The state required by the procedure is passed at construction time. 
    // It is mandatory to pass a CommandResponse and a timeout as these values are 
    // needed by the base class AsyncProcedure.
    MyLongProcedure(A0 stateA, A1 stateB, ..., const CommandResponse& res, uint32_t timeout) :
        AsyncProcedure(res, procedureTimeout), _stateA(stateA), _stateB(stateB), ... {
    }

    // destructor defined if non RAII resources have to be released.
    virtual ~MyLongProcedure() { }

    // Procedure starting point
    // If the procedure launch was successful should returns true otherwise 
    // returns false
    virtual bool doStart() {
        // start the long operation, 
        // In this case, once the operation is done, whenResult member function 
        // will be called.
        // 
        return my_long_operation(_stateA, ..., this, &MyLongProcedure::whenResult)
    }

    // what to do when timeout occur 
    virtual void doWhenTimeout() { 
        // set response 
        // cleanup resources 
        // ...

        // DO NOT CALL terminate in this function.
    }

    //////////////////////////////
    // CODE SPECIFIC TO the class 
    //////////////////////////////

    // async handler, specific to MyLongProcedure
    void whenResult(bool success) { 
        // asynchronous operation done, fill the response (stored in the base
        // class)
        if (success) { 
            response->success();
        } else { 
            response->faillure("Oups!");
        }

        // terminate the procedure. This step **is** explicit and shouldn't be 
        // omitted.
        terminate();
    }

    A0 _stateA;
    A1 _stateB;
    // ... other state needed by the procedure
};
```

To launch the procedure, the helper function `startProcedure` has to be used 
from the command handler: 

```c++
startProcedure<MyLongProcedure>(a0, a1, ..., response, /* timeout */ 10000);
```


## Module creation 

A module is a structure declaring four static functions describing a cohesive 
group of commands. 

Lets start by creating a simple module: 

```c++
struct MyCommandModule {
    static const char* name() { 
        return "my_module";
    }

    static const char* info() { 
        return "infos about my module";
    }

    static const char* man() { 
        return "enter 'my_module list' to list the commands available in this 
        module and 'my_module help <command name>' to get the help of a command.";
    }

    // not implemented here
    static ConstArray<const Command*> commands();
};
```

* `const char* name()`: Return the name used to access to the module. It 
should not contain space. 
Users will use this value to gain access to the commands of the module on the CLI. 
For instance, if this function returns `my_module`, users will access commands 
by invoking `my_module <command_name> <args...>`.
* `const char* info()`: Short readable description of the module.
* `const char* man()`: Manual of the module. It can just instruct the user how 
to use the builtin `list` and `help`.
* `ConstArray<const Command*> commands()`: Return an array containing all the 
commands of the module.


Like for Commands declaration, it is possible to use an helper to define the 
`commands` function: 

```c++
DECLARE_SUITE_COMMANDS(MyCommandModule, 
    <command instances...>
)
```

If the commands have been declared with the `DECLARE_CMD` helper then it is 
possible to use the helper `CMD_INSTANCE` to get the instance of the command.

For instance, with the addition command defined earlier: 

```c++
DECLARE_SUITE_COMMANDS(MyCommandModule, 
    CMD_INSTANCE(AdditionCommand),
    <other commands instances>
)
```

## Register command suite 

To register a new command suite in the system, the user have to call the 
template function `registerCommandSuite` with the type of the command suite as 
template argument. This call must be made in the function 
`initialize_app_commands` defined in the file `main.cpp`.

```c++
void initialize_app_commands(void) {
    // existing code ....

    // register new command module here:
    registerCommandSuite<MyCommandModule>();
}
```

## Serialization 

The serialization framework define a simple protocol to serialize c++ values 
into JSON and deserialize string into c++ values. 

Since the framework was built around mbed-client-app needs and limitations it 
is assymetric: 
* Deserialization of arguments in input convert string tokens into c++ value. It 
is **not** structured and it is the job of the command handler to reconstruct 
the aggregates (if any). 
* Serialization of the command response is structured and made in JSON. 

Therefore, it is common that aggregates type only implement the serialization 
to JSON.

### String deserialization

One functions is the pilar of the string deserialization process.For a type T, 
this function would be defined the following way: 

```c++
bool fromString(const char* input, T& output);
```

This function takes a string in input and fill the output variable if it is 
possible to convert the input in a T. The returned value of the operation is the 
status of the operation: `true` if it has succeed and `false` otherwise.

Users wishing to generate automatically the deserialization part of their 
command handler should provide an overload of this function for any type in 
input of the command handler. 

### Enum deserialization/serialization

Serialization and deserialization of enumerations can be achieved in a 
declarative fashion rather than an imperative one. User code should describe 
what the enumeration is and what is the mapping between string values and 
enumeration values rather than implementing functions directly. 

Users just have to provide a complete specialization for the enum type of the 
class template SerializerDescription (see 
[here](source.Serialization/Serializer.h)). The specialization requires two 
**static** member functions: 

* `const ConstArray<ValueToStringMapping<T> > mapping()`: This function should 
return a map between string values and enum values. 
* `const char* errorMessage()`: Message in case of error during the 
deserialization process.

As an example consider the serialization of the following enumeration: 

```c++
enum Gender { 
  MALE, 
  FEMALE
};
```

The description can be achieved with the following specialization:

```c++
template<>
struct SerializerDescription<Gender> {
    typedef Gender type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { MALE, "male" },
            { FEMALE, "female" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gender";
    }
};
```

No JSON serialization function are automatically generated, instead a function 
converting the enum to a string is generated (`const char* toStringT()`). 

User code should either declare the operator `<<` for the type T with the 
JSONOutput stream or stream value using the `toString` function.

```c++
using namespace serialization;

JSONOutputStream os;
Gender g = MALE;

// will add the string "male" into the stream
os << toString(g);

// overload of operator << for Gender
JSONOutputStream& operator<<(JSONOutputStream& os, Gender gender) { 
  return os << toString(gender);
}

os << g;
```


### JSON serialization

Serialization to JSON is achieved with a 
[JSONOutputStream](source/Serialization/JSONOutputStream.h). Providing an 
overload of the operator `<<` for this stream is enough to achieve the 
serialization process: 

```c++
namespace ser = serialization;
ser::JSONOutputStream& operator<<(ser::JSONOutputStream& os, const T& v);
```

All the tools necessary to declare and structure JSON are bundled with the 
JSONOutputStream: 


#### Number serialization 

Serialization of numbers is done using the operator `<<`.

```c++ 
using namespace serialization;

JSONOutputStream os;

uint16_t value = 42;

// will ad the number 42 in the JSON stream
os << value;
```


#### String serialization 

Like numbers, string serialization is implemented by the operator `<<`.

```c++ 
using namespace serialization;

JSONOutputStream os;
const char* value = "Hello Word!";

// will add the string "Hello Word!" in the JSON stream
os << value;
```

It is important to note that the string added in the JSON stream is **not** 
escaped.


#### Boolean serialization 

Like others serialization of boolean is implemented by the operator `<<`.

```c++ 
using namespace serialization;

JSONOutputStream os;
bool value = true;

// will add the boolean true in the JSON stream
os << value;
```


#### Null serialization 

Null value can be added to a JSON stream by pushing the function `nil`.

```c++ 
using namespace serialization;

JSONOutputStream os;
// will add the nill value in the JSON stream
os << nil;
```


#### Object declaration 

To create a JSON object insert the function `startObject` into the stream. 
To create properties, insert keys using the function `key` followed by the value 
associated with the key. Finally to end the object insert the function 
`endObject` in the stream.


```c++ 
using namespace serialization;

struct Pixel { 
    uint8_t red; 
    uint8_t green; 
    uint8_t blue; 
};

// JSON serialization function of a Pixel 
JSONOutputStream& operator<<(JSONOutputStream& os, const Pixel& p) { 
    return os << startObject <<
        key("red") << p.red <<
        key("green") << p.green <<
        key("blue") << p.blue <<
    endObject;
}


// usage 
JSONOutputStream os;
Pixel p = { 0, 0. 255 };

os << p;
```

#### Array declaration 

Arrays can be created by inserting the function `startArray` and closed by 
inserting the function `endArray`. 

As a simple example consider JSON serialization of this POD which mix recursion,
object and arrays: 

```c++ 
struct Person { 
  const char* name; 
  uint8_t age;
  struct Person* childrens; 
  uint32_t childrens_count; 
};
```

The POD itself can be converted to a JSON object where each property map the 
name and the value of a POD field. The field `childrens` can be converted into 
a JSON array of Person. 

```c++
using namespace serialization;

JSONOutputStream& operator<<(JSONOutputStream& os, const Person& p);

JSONOutputStream& operator<<(JSONOutputStream& os, const Person& p) { 
  // Person is an aggregate, declare it inside an object.
  os << startObject <<
    // each field of the POD match a property
    key("name") << p.name <<
    key("age") << p.age <<
    key("childrens_count") << p.childrens_count <<
    // childrens is a list of Person, store them into a JSON array.
    key("childrens") << startArray;  
    for (uint32_t i = 0; i < p.childrens_count; ++i) { 
      os << childrens[i];
    }
    // end the array of childrens 
    os << endArray <<
  // end the person object
  endobject;
}

```
