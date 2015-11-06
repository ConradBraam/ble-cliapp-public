#ifndef BLE_CLIAPP_DYNAMIC_STATIC_STRING_H_
#define BLE_CLIAPP_DYNAMIC_STATIC_STRING_H_

#include <cstring>

namespace container {

class StaticString {

public:
    /**
     * @brief construct an empty StaticString
     */
    constexpr StaticString() : _str(0), _size(0) {}

    StaticString(const char* str) : _str(str), _size(strlen(str)) { }

    constexpr StaticString(const char* str, size_t size) : _str(str), _size(size) { }

    template<size_t N>
    constexpr StaticString(const char (&str)[N]) : _str(str), _size(N - 1) { } 

    /**
     * @brief return the c string held
     */
    constexpr const char* c_str() const { return _str; }

    /**
     * length of the string without the null terminated character
     */
    constexpr size_t size() const { return _size; } 

private:
    const char* _str;
    size_t _size;
};

} // namespace container

constexpr container::StaticString operator "" _ss(const char* str, size_t size) {
    return container::StaticString(str, size);
}

#endif //BLE_CLIAPP_DYNAMIC_STATIC_STRING_H_
