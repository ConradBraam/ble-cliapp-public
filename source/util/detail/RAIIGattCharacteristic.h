#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_

#include <stdint.h>
#include <ble/GattCharacteristic.h>
#include "util/Vector.h"
#include "RAIIGattAttribute.h"

namespace detail {

class RAIIGattCharacteristic : public GattCharacteristic {
public:
    RAIIGattCharacteristic(const UUID& uuid);

    ~RAIIGattCharacteristic();

    void setValue(const container::Vector<uint8_t>& value);

    bool setMaxLength(uint16_t max);

    void setVariableLength(bool hasVariableLen);

    void setProperties(uint8_t newProperties);

    // note: the attribute will be moved and owned by this data structure ...
    void addDescriptor(RAIIGattAttribute* descriptor);

    void releaseAttributesValue();
private:
    RAIIGattCharacteristic(const RAIIGattCharacteristic&);
    RAIIGattCharacteristic& operator=(const RAIIGattCharacteristic&);
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_CHARACTERISTIC_H_
