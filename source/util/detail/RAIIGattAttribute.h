#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_

#include <cstdint>
#include <ble/GattAttribute.h>
#include "util/Vector.h"

namespace detail {

class RAIIGattAttribute : public GattAttribute {
public:
    template<typename T>
    using Vector = container::Vector<T>;

    RAIIGattAttribute(const UUID& uuid);
    RAIIGattAttribute(const RAIIGattAttribute&) = delete;
    RAIIGattAttribute& operator=(const RAIIGattAttribute&) = delete;
    RAIIGattAttribute(RAIIGattAttribute&&) = default;
    RAIIGattAttribute& operator=(RAIIGattAttribute&&) = default;

    ~RAIIGattAttribute();

    void setValue(const Vector<std::uint8_t>& value);

    bool setMaxLength(std::uint16_t max);

    void setVariableLength(bool hasVariableLen);
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_ATTRIBUTE_H_
