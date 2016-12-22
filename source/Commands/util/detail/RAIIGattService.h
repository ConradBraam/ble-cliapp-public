#ifndef BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_
#define BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_

#include <stdint.h>
#include <ble/GattService.h>
#include "util/Vector.h"
#include "RAIIGattCharacteristic.h"

namespace detail {

class RAIIGattService : public GattService {
public:
    RAIIGattService(const UUID& uuid);

    ~RAIIGattService();

    // note: the characteristic will be moved and owned by this data structure ...
    void addCharacteristic(RAIIGattCharacteristic* characteristic);

    void releaseAttributesValue();

private:
    RAIIGattService(const RAIIGattService&);
    RAIIGattService& operator=(const RAIIGattService&);
};

}

#endif //BLE_CLIAPP_UTIL_DETAIL_RAII_GATT_SERVICE_H_
