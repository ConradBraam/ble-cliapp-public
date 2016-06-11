#include "RAIIGattAttribute.h"
#include "util/HijackMember.h"

HIJACK_MEMBER(_valuePtr_accessor, uint8_t* GattAttribute::*, &GattAttribute::_valuePtr);
HIJACK_MEMBER(_lenMax_accessor, uint16_t GattAttribute::*, &GattAttribute::_lenMax);
HIJACK_MEMBER(_len_accessor, uint16_t GattAttribute::*, &GattAttribute::_len);
HIJACK_MEMBER(_hasVariableLen_accessor, bool GattAttribute::*, &GattAttribute::_hasVariableLen);

namespace detail {

RAIIGattAttribute::RAIIGattAttribute(const UUID& uuid)  :
    GattAttribute(uuid, NULL, 0, 0, true) {
}

RAIIGattAttribute::~RAIIGattAttribute() {
    if(this->*_valuePtr_accessor) {
        delete[] (this->*_valuePtr_accessor);
    }
}

void RAIIGattAttribute::setValue(const container::Vector<uint8_t>& newValue) {
    uint8_t*& valuePtr = (this->*_valuePtr_accessor);
    uint16_t& len = this->*_len_accessor;
    uint16_t& maxLen = this->*_lenMax_accessor;

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

bool RAIIGattAttribute::setMaxLength(uint16_t max) {
    if(max < this->*_len_accessor) {
        return false;
    }

    this->*_lenMax_accessor = max;
    return true;
}

void RAIIGattAttribute::setVariableLength(bool hasVariableLen) {
    this->*_hasVariableLen_accessor = hasVariableLen;
}

void RAIIGattAttribute::releaseAttributeValue() {
    uint8_t*& valuePtr = (this->*_valuePtr_accessor);
    uint16_t& len = this->*_len_accessor;

    if(valuePtr) {
        delete[] valuePtr;
        valuePtr = NULL;
        len = 0;
    }
}

}
