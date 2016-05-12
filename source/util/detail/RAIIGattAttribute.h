#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_

#include <stdint.h>
#include <ble/GattAttribute.h>
#include "util/Vector.h"

namespace detail {

class RAIIGattAttribute : public GattAttribute {
public:

    RAIIGattAttribute(const UUID& uuid);

    ~RAIIGattAttribute();

    void setValue(const container::Vector<uint8_t>& value);

    bool setMaxLength(uint16_t max);

    void setVariableLength(bool hasVariableLen);

    void releaseAttributeValue();
private:
    RAIIGattAttribute(const RAIIGattAttribute&);
    RAIIGattAttribute& operator=(const RAIIGattAttribute&);
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_
