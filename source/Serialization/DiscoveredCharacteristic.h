#ifndef BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_
#define BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_

#include "Serializer.h"
#include "ble/DiscoveredCharacteristic.h"
#include "dynamic/Value.h"
#include "JSONOutputStream.h"

/**
 * @brief Convert a discovered characteristic intance to a dynamic::Value.
 * @details The returned object will be a map with the following attributes:
 *     - "UUID": The uuid of this gatt characteristic
 *     - "properties": An array of dynamic::Values containing the properties
 *                     associated with this characteristic
 *     - "start_handle": The first handle of the characteristic description
 *     - "value_handle": The handle containing the value of this characteristic
 *     - "end_handle": The last handle of the characteristic description
 *
 * @param characteristic An instance of DiscoveredCharacteristic
 * @return The characteristic serialized to a dynamic::Value
 */
dynamic::Value toDynamicValue(const DiscoveredCharacteristic* characteristic);
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic& characteristic);

/**
 * @brief Convert properties of discovered characteristic to a dynamic::Value
 * @details The returned object will be an array of the properties, the list of
 * the availables values which can populate the array are:
 *   - "broadcast": if the broadcasting the value permitted.
 *   - "read": if reading the value is permitted.
 *   - "writeWoResp": if writing the value with Write Command is permitted.
 *   - "write": if writing the value with Write Request permitted.
 *   - "notify": if notifications of the value is permitted
 *   - "indicate": if indication of the value is permitted
 *   - "authSignedWrite": if writing the value with Signed Write Command is permitted.
 *
 * @param properties The properties to convert
 * @return An array of the available properties
 */
dynamic::Value toDynamicValue(const DiscoveredCharacteristic::Properties_t& properties);
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic::Properties_t& properties);


#endif //BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_
