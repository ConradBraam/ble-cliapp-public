#ifndef BLE_CLIAPP_GATT_CALLABCK_PARAM_TYPES_SERIALIZER_H_
#define BLE_CLIAPP_GATT_CALLABCK_PARAM_TYPES_SERIALIZER_H_

#include "Serializer.h"
#include "ble/DiscoveredCharacteristic.h"
#include "dynamic/Value.h"

#include "JSONOutputStream.h"

/**
 * @brief Convert a result of GattClient read operation
 * @details The returned object will be a map with the following attributes:
 *     - "connection_handle": The handle of the connection where the read occur
 *     - "attribute_handle": The handle of attribute read
 *     - "offset": Offset of the data read
 *     - "length": lenght of the data read
 *     - "data": The data represented as an hex string
 *
 * @param readResult An instance of GattReadCallbackParams containing the value
 * of a read operation
 * @return The result of the read operation serialized to a dynamic::Value
 */
dynamic::Value toDynamicValue(const GattReadCallbackParams* readResult);

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GattReadCallbackParams& readResult);

/**
 * @brief Convert the result of GattClient write operation
 * @details The returned object will be a map with the following attributes:
 *     - "connection_handle": The handle of the connection where the write occur
 *     - "attribute_handle": The handle of the attribute written
 *     - "offset": Offset of the data written
 *     - "length": lenght of the data written
 *     - "data": The data written (as an hex string)
 *     - "write_operation_type": The type of write operation
 *
 * @param writeResult An instance of GattWriteCallbackParams containing the value
 * of a write operation
 * @return The result of the write operation serialized to a dynamic::Value
 */
dynamic::Value toDynamicValue(const GattWriteCallbackParams* writeResult);

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GattWriteCallbackParams& writeResult);

#endif //BLE_CLIAPP_GATT_CALLABCK_PARAM_TYPES_SERIALIZER_H_
