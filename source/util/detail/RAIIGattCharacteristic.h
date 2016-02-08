#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_

#include <cstdint>
#include <ble/GattCharacteristic.h>
#include "util/Vector.h"
#include "RAIIGattAttribute.h"

namespace detail {

class RAIIGattCharacteristic : public GattCharacteristic {
public:
    template<typename T>
    using Vector = container::Vector<T>;

    RAIIGattCharacteristic(const UUID& uuid);
    RAIIGattCharacteristic(const RAIIGattCharacteristic&) = delete;
    RAIIGattCharacteristic& operator=(const RAIIGattCharacteristic&) = delete;

    ~RAIIGattCharacteristic();

    void setValue(const Vector<std::uint8_t>& value);

    bool setMaxLength(std::uint16_t max);

    void setVariableLength(bool hasVariableLen);

    void setProperties(std::uint8_t newProperties);

    // note: the attribute will be moved and owned by this data structure ...
    void addDescriptor(RAIIGattAttribute* descriptor);
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_
