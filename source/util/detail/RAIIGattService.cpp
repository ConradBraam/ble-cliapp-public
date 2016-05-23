#include <cstdlib>
#include "RAIIGattService.h"
#include "RAIIGattAttribute.h"
#include "util/HijackMember.h"

HIJACK_MEMBER(_characteristics_accessor, GattCharacteristic** GattService::*, &GattService::_characteristics);
HIJACK_MEMBER(_characteristicCount_accessor, uint8_t GattService::*, &GattService::_characteristicCount);

namespace detail {

RAIIGattService::RAIIGattService(const UUID& uuid)  :
    GattService(uuid, NULL, 0) {
}

RAIIGattService::~RAIIGattService() {
    GattCharacteristic**& characteristics = this->*_characteristics_accessor;
    uint8_t& characteristicCount = this->*_characteristicCount_accessor;

    for(uint8_t i = 0; i < characteristicCount; ++i) {
        delete static_cast<RAIIGattCharacteristic*>(characteristics[i]);
    }

    if(characteristicCount) {
        std::free(characteristics);
    }
}

void RAIIGattService::addCharacteristic(RAIIGattCharacteristic* characteristic) {
    GattCharacteristic**& characteristics = this->*_characteristics_accessor;
    uint8_t& characteristicCount = this->*_characteristicCount_accessor;

    characteristics = static_cast<GattCharacteristic**>(std::realloc(characteristics, sizeof(GattCharacteristic*) * (characteristicCount + 1)));
    characteristics[characteristicCount] = characteristic;
    characteristicCount += 1;
}


void RAIIGattService::releaseAttributesValue() {
    RAIIGattCharacteristic** characteristics = reinterpret_cast<RAIIGattCharacteristic**>(this->*_characteristics_accessor);
    uint8_t characteristicCount = this->*_characteristicCount_accessor;

    for(uint8_t i = 0; i < characteristicCount; ++i) {
        characteristics[i]->releaseAttributesValue();
    }
}

} // namespace detail
