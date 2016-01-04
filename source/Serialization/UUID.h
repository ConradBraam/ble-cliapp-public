#ifndef BLE_CLIAPP_SERIALIZATION_UUID_H_
#define BLE_CLIAPP_SERIALIZATION_UUID_H_

#include "ble/UUID.h"
#include "JSONOutputStream.h"


/**
 * @brief Construct a UUID from an input string
 * @param str The string containing the UUID, it can have the following format:
 *   - 0xYYYY for 16 bits UUID constructed from the hex value
 *   - XXXXX for 16 bits UUID
 *   - XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX for 128 bits UUID
 * @param uuid The UUID to fill with the result of the conversion
 * @return true if the conversion has succeed and false otherwise
 */
bool fromString(const char* str, UUID& uuid);

/**
 * @brief convert an UUID instance to its string representation
 *
 * @param uuid the uuid to convert
 * @return It depend on the size of the UUID:
 *   - 16 bits UUID: the string will use the format 0xYYYY
 *   - 128 bits UUID: the string will use the format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
 */
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const UUID& uuid);

#endif //BLE_CLIAPP_SERIALIZATION_UUID_H_
