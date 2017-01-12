#ifndef BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_
#define BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_

#include "Serialization/Serializer.h"
#include "ble/DiscoveredCharacteristic.h"
#include "Serialization/JSONOutputStream.h"

/**
 * @brief Serialize a discovered characteristic intance into a JSON stream.
 * @details The serialized object will be a map with the following attributes:
 *     - "UUID": The uuid of this gatt characteristic
 *     - "properties": An array of dynamic::Values containing the properties
 *                     associated with this characteristic
 *     - "start_handle": The first handle of the characteristic description
 *     - "value_handle": The handle containing the value of this characteristic
 *     - "end_handle": The last handle of the characteristic description
 *
 * @param os The output stream where the instance will be serialized
 * @param characteristic An instance of DiscoveredCharacteristic
 * @return os
 */
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic& characteristic);

/**
 * @brief Serialize properties of discovered characteristic in a JSON stream
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
 * @param os The output stream where the instance will be serialized
 * @param properties The properties to convert
 * @return os
 */
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic::Properties_t& properties);


#endif //BLE_CLIAPP_DISCOVERED_CHARACTERISTIC_SERIALIZER_H_
