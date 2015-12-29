#ifndef BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_
#define BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_

#include "StaticString.h"
#include "DynamicString.h"

namespace container {

class SimpleString {
public:
    enum class ImplementationType_t : bool {
        STATIC_STRING,
        DYNAMIC_STRING
    };

    typedef const char* const_iterator;

    /**
     * @brief construct an empty DynamicString
     */
    SimpleString();

    /**
     * @brief Create a string from a null terminated byte string, the user can
     * choose if the string is a dynamic one or the static one by providing the
     * implementation type
     *
     * @param str the null terminated byte string used to create this string
     * @param type The type of the string (dynamic or static)
     */
    SimpleString(const char* str, ImplementationType_t type = ImplementationType_t::DYNAMIC_STRING);

    /**
     * @brief copy construct a SimpleString from a StaticString
     */
    SimpleString(const StaticString& staticString);

    /**
     * @brief move construct a SimpleString from a static string
     */
    SimpleString(StaticString&& staticString);

    /**
     * @brief copy construct a SimpleString from a DynamicString
     */
    SimpleString(const DynamicString& dynamicString);

    /**
     * @brief move construct a SimpleString from a DynamicString
     */
    SimpleString(DynamicString&& dynamicString);

    /**
     * @brief Copy construct a SimpleString
     */
    SimpleString(const SimpleString& that);

    /**
     * @brief Move construct a SimpleString
     */
    SimpleString(SimpleString&& that);

    /**
     * @brief Destructor of a SimpleString
     */
    ~SimpleString();

    /**
     * @brief Copy assignement of a SimpleString
     */
    SimpleString& operator=(const SimpleString& that);

    /**
     * @brief Move assignement of a SimpleString
     */
    SimpleString& operator=(SimpleString&& that);

    /**
     * @brief return the c string held
     */
    const char* c_str() const;

    /**
     * length of the string without the null terminated character
     */
    size_t size() const;

    /**
     * @brief return a constant iterator to the begining of the string
     */
    const_iterator cbegin() const;

    /**
     * @brief return a constant iterator to the end of the string
     */
    const_iterator cend() const;

    /**
     * @brief equal to operator for SimpleString
     * @return true if lhs == rhs and false otherwise
     */
    friend bool operator==(const SimpleString& lhs, const SimpleString& rhs);

    /**
     * @brief not equal to operator for SimpleString
     * @return true if lhs != rhs and false otherwise
     */
    friend bool operator!=(const SimpleString& lhs, const SimpleString& rhs);

private:
    union {
        StaticString _static;
        DynamicString _dynamic;
    };
    ImplementationType_t _type;
};

} // namespace container

#endif //BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_
