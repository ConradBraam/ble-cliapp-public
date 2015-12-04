#ifndef BLE_CLIAPP_SERIALIZATION_HEX_H_
#define BLE_CLIAPP_SERIALIZATION_HEX_H_

/**
 * @brief Convert the string representation of a byte in asci hexadecimal
 * characters to a byte.
 *
 * @param msb The ascii character representing the msb part of the byte
 * @param lsb The ascii character representing the lsb part of the byte
 * @param result the variable which will be filled with the result if the
 * conversion succeed.
 * @return true if the conversion has been made and false otherwise.
 */
bool asciiHexByteToByte(char msb, char lsb, uint8_t& result);

#endif //BLE_CLIAPP_SERIALIZATION_HEX_H_
