#include <memory>
#include <cstring>

#include "DynamicString.h"

namespace container {

DynamicString::DynamicString() : _str(), _size(0) { }

DynamicString::DynamicString(const char* str, size_t size) :
    _str(new char[size + 1]), _size(size) {
    std::memcpy(_str.get(), str, size);
    _str[size] = '\0';
}

DynamicString::DynamicString(const char* str) : DynamicString(str, strlen(str)) { }

DynamicString::DynamicString(const DynamicString& other) : DynamicString(other.c_str(), other.size()) { }

DynamicString::DynamicString(DynamicString&& other) : _str(std::move(other._str)), _size(other._size) {
    other._size = 0;
}

DynamicString::~DynamicString() { }

DynamicString& DynamicString::operator=(const DynamicString& other) {
    _size = other._size;
    _str = std::unique_ptr<char []>(new char[_size + 1]);
    std::memcpy(_str.get(), other.c_str(), _size + 1);
    return *this;
}

DynamicString& DynamicString::operator=(DynamicString&& other) {
    _str.swap(other._str);
    _size = other._size;
    other._size = 0;
    return *this;
}

} // namespace container
