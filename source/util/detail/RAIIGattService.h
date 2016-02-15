#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_

#include <cstdint>
#include <ble/GattService.h>
#include "util/Vector.h"
#include "RAIIGattCharacteristic.h"

namespace detail {

class RAIIGattService : public GattService {
public:
    template<typename T>
    using Vector = container::Vector<T>;

    RAIIGattService(const UUID& uuid);
    RAIIGattService(const RAIIGattService&) = delete;
    RAIIGattService& operator=(const RAIIGattService&) = delete;

    ~RAIIGattService();

    // note: the characteristic will be moved and owned by this data structure ...
    void addCharacteristic(RAIIGattCharacteristic* characteristic);

    void releaseAttributesValue();
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_
