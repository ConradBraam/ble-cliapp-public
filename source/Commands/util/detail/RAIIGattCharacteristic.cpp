#include <cstdlib>

#include "RAIIGattCharacteristic.h"
#include "RAIIGattAttribute.h"
#include "util/HijackMember.h"

HIJACK_MEMBER(_valuePtr_accessor, uint8_t* GattAttribute::*, &GattAttribute::_valuePtr);
HIJACK_MEMBER(_lenMax_accessor, uint16_t GattAttribute::*, &GattAttribute::_lenMax);
HIJACK_MEMBER(_len_accessor, uint16_t GattAttribute::*, &GattAttribute::_len);
HIJACK_MEMBER(_hasVariableLen_accessor, bool GattAttribute::*, &GattAttribute::_hasVariableLen);
HIJACK_MEMBER(_properties_accessor, uint8_t GattCharacteristic::*, &GattCharacteristic::_properties);
HIJACK_MEMBER(_descriptors_accessor, GattAttribute** GattCharacteristic::*, &GattCharacteristic::_descriptors);
HIJACK_MEMBER(_descriptorCount_accessor, uint8_t GattCharacteristic::*, &GattCharacteristic::_descriptorCount);

namespace detail {

RAIIGattCharacteristic::RAIIGattCharacteristic(const UUID& uuid)  :
    GattCharacteristic(uuid) {
}

RAIIGattCharacteristic::~RAIIGattCharacteristic() {
    GattAttribute& attr = getValueAttribute();
    GattAttribute**& descriptors = this->*_descriptors_accessor;
    uint8_t& descriptorsCount = this->*_descriptorCount_accessor;

    if(attr.*_valuePtr_accessor) {
        delete[] (attr.*_valuePtr_accessor);
    }

    for(uint8_t i = 0; i < descriptorsCount; ++i) {
        delete static_cast<RAIIGattAttribute*>(descriptors[i]);
    }

    if(descriptorsCount) {
        std::free(descriptors);
    }
}

void RAIIGattCharacteristic::setValue(const container::Vector<uint8_t>& newValue) {
    uint8_t*& valuePtr = (this->getValueAttribute()).*_valuePtr_accessor;
    uint16_t& len = (this->getValueAttribute()).*_len_accessor;
    uint16_t& maxLen = (this->getValueAttribute()).*_lenMax_accessor;

    if(valuePtr) {
        delete[] valuePtr;
        valuePtr = NULL;
        len = 0;
    }

    if(newValue.size()) {
        valuePtr = new uint8_t[newValue.size()]();
        std::memcpy(valuePtr, newValue.cbegin(), newValue.size());
        len = newValue.size();
    }

    if(maxLen < len) {
        maxLen = len;
    }
}

bool RAIIGattCharacteristic::setMaxLength(uint16_t max) {
    if(max < (this->getValueAttribute()).*_lenMax_accessor) {
        return false;
    }

    (this->getValueAttribute()).*_lenMax_accessor = max;
    return true;
}

void RAIIGattCharacteristic::setVariableLength(bool hasVariableLen) {
    (this->getValueAttribute()).*_hasVariableLen_accessor = hasVariableLen;
}

void RAIIGattCharacteristic::setProperties(uint8_t newProperties) {
    this->*_properties_accessor = newProperties;
}

void RAIIGattCharacteristic::addDescriptor(RAIIGattAttribute* descriptor) {
    GattAttribute**& descriptors = this->*_descriptors_accessor;
    uint8_t& descriptorsCount = this->*_descriptorCount_accessor;

    descriptors = static_cast<GattAttribute**>(std::realloc(descriptors, sizeof(GattAttribute*) * (descriptorsCount + 1)));
    descriptors[descriptorsCount] = descriptor;
    descriptorsCount += 1;
}


void RAIIGattCharacteristic::releaseAttributesValue() {
    uint8_t*& valuePtr = (this->getValueAttribute()).*_valuePtr_accessor;
    uint16_t& len = (this->getValueAttribute()).*_len_accessor;

    if(valuePtr) {
        delete[] valuePtr;
        valuePtr = NULL;
        len = 0;
    }


    RAIIGattAttribute** descriptors = reinterpret_cast<RAIIGattAttribute**>(this->*_descriptors_accessor);
    uint8_t descriptorsCount = this->*_descriptorCount_accessor;

    for(uint8_t i = 0; i < descriptorsCount; ++i) {
        descriptors[i]->releaseAttributeValue();
    }
}

} // namespace detail
