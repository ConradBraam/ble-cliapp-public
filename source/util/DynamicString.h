#ifndef BLE_CLIAPP_DYNAMIC_DYNAMIC_STRING_H_
#define BLE_CLIAPP_DYNAMIC_DYNAMIC_STRING_H_

#include <memory>

namespace container {

/**
 * @brief RAII object to store a string dynamically allocated
 */
class DynamicString {

public:
    /**
     * @brief construct an empty DynamicString
     */
    DynamicString();

    /**
     * @brief Construct a string of a given length
     *
     * @param str The str used to construct this object
     * @param size The number of characters to copy excluding the null character at the end
     */
    DynamicString(const char* str, size_t size);

    /**
     * @brief Create a DynamicString from a null terminated byte string
     *
     * @param str The null terminated byte strin
     */
    DynamicString(const char* str);

    /**
     * @brief Create a DynamicString from a null terminated byte array
     *
     * @param str The null terminated byte array  used to create this object
     * @tparam N The number of elements in the byte array
     */
    template<size_t N>
    DynamicString(const char (&str)[N]) : DynamicString(str, N - 1) { }

    /**
     * @brief Copy constructor
     *
     * @param other The DynamicString used in the copy construction
     */
    DynamicString(const DynamicString& other);

    /**
     * @brief Move constructor
     *
     * @param that The DynamicString instance moved into the constructed one.
     */
    DynamicString(DynamicString&& that);

    /**
     * @brief Destructor
     */
    ~DynamicString();

    /**
     * @brief copy assignement operator
     *
     * @param other the DynamicString to copy assign to this
     * @return *this
     */
    DynamicString& operator=(const DynamicString& other);

    /**
     * @brief move assignement operator
     *
     * @param other the DynamicString moved into this instance
     * @return *this
     */
    DynamicString& operator=(DynamicString&& other);

    /**
     * @brief return the c string held
     */
    const char* c_str() const { return _str.get(); }

    /**
     * length of the string without the null terminated character
     */
    size_t size() const { return _size; }

private:
    std::unique_ptr<char[]> _str;
    size_t _size;
};

} // namespace container

#endif //BLE_CLIAPP_DYNAMIC_DYNAMIC_STRING_H_
