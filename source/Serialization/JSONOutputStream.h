#ifndef BLE_CLIAPP_UTIL_JSON_OUTPUT_STREAM_H_
#define BLE_CLIAPP_UTIL_JSON_OUTPUT_STREAM_H_

#include <type_traits>
#include <stdint.h>
#include <cstdarg>
#include <cstdio>
#include <memory>

namespace serialization {

/**
 * @brief Output JSON data to stdout
 * @details [long description]
 *
 * @param os [description]
 * @return [description]
 */
class JSONOutputStream {

    friend JSONOutputStream& endArray(JSONOutputStream& os);
    friend JSONOutputStream& endObject(JSONOutputStream& os);

public:
    /**
     * @brief Function type which interract with the stream
     */
    typedef JSONOutputStream& (*Function_t)(JSONOutputStream&);

    /**
     * @brief Instantiate a new output stream
     */
    JSONOutputStream(std::FILE* output = stdout) :
        out(output), startNewValue(false) {
    }

    // disable all copy operation and move assignment (delete of move operations
    // is more questionable here)
    JSONOutputStream(const JSONOutputStream&) = delete;
    JSONOutputStream& operator=(const JSONOutputStream&) = delete;
    JSONOutputStream(JSONOutputStream&&) = delete;
    JSONOutputStream& operator=(JSONOutputStream&&) = delete;

    /**
     * @brief insert a boolean value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(bool value);

    /**
     * @brief insert a int8_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(int8_t value);

    /**
     * @brief insert a uint8_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(uint8_t value);

    /**
     * @brief insert a int16_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(int16_t value);

    /**
     * @brief insert a uint16_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(uint16_t value);

    /**
     * @brief insert a int32_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(int32_t value);

    /**
     * @brief insert a uint32_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(uint32_t value);

    /**
     * @brief insert a int64_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(int64_t value);

    /**
     * @brief insert a uint64_t value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(uint64_t value);

    /**
     * @brief insert a byte string value value into the stream
     * @param value The value to insert
     * @return *this
     */
    JSONOutputStream& operator<<(const char* value);

    /**
     * @brief insert data in this through an insertion function
     * @param f The insertion function
     * @return *this
     */
    JSONOutputStream& operator<<(Function_t f) {
        return f(*this);
    }

/// LOW LEVEL API

    /**
     * @brief directly print data using printf format.
     * @detail This function will just insert data into the stream. The data
     * written will not be committed as a value. Subsequent call to this function
     * will not insert a value separator into the stream. To do that, the user
     * have to explicitly call commitValue once all the data of the value has been
     * written.
     * @param fmt The format string
     * @return *this
     */
    JSONOutputStream& format(const char *fmt, ...);

    /**
     * @brief same as format function but take a va_list instead of an ellipsis.
     * @param fmt The format string
     * @param args The arguments which will be applied through fmt
     * @return *this
     */
    JSONOutputStream& vformat(const char *fmt, va_list args);

    /**
     * @brief put a character into the stream
     * @detail This function will just insert data into the stream. The data
     * written will not be committed as a value. Subsequent call to this function
     * will not insert a value separator into the stream. To do that, the user
     * have to explicitly call commitValue once all the data of the value has been
     * written.
     */
    void put(char c);

    /**
     * @brief write an array of character into the stream
     * @detail This function will just insert data into the stream. The data
     * written will not be committed as a value. Subsequent call to this function
     * will not insert a value separator into the stream. To do that, the user
     * have to explicitly call commitValue once all the data of the value has been
     * written.
     */
    void write(const char* data, size_t count);

    /**
     * @brief write an array of character into the stream
     * @detail This function will just insert data into the stream. The data
     * written will not be committed as a value. Subsequent call to this function
     * will not insert a value separator into the stream. To do that, the user
     * have to explicitly call commitValue once all the data of the value has been
     * written.
     */
    void write(const char* data);

    /**
     * Flush the current content
     */
    void flush();

    /**
     * Indicate that the value has to be committed. When it is required, the system
     * will insert a comma character to distinguish the last JSON value from the new
     * one.
     */
    void commitValue();

    /**
     * @brief Write a new value into the stream by using printf format. This function
     * is equivalent to a call to format then a call to commitValue.
     * @param fmt The format string
     * @return *this
     */
    JSONOutputStream& formatValue(const char *fmt, ...);

    /**
     * @brief same as formatValue function but take a va_list instead of an ellipsis.
     * @param fmt The format string
     * @param args The arguments which will be applied through fmt
     * @return *this
     */
    JSONOutputStream& vformatValue(const char *fmt, va_list list);

private:
    void handleNewValue();

    std::FILE* out;
    bool startNewValue;
};

/**
 * @brief Start a new array in the output stream.
 * @param os The output stream to operate on
 * @return os
 */
JSONOutputStream& startArray(JSONOutputStream& os);

/**
 * @brief End an array in the output stream.
 * @param os The output stream to operate on
 * @return os
 */
JSONOutputStream& endArray(JSONOutputStream& os);

/**
 * @brief Start a new object in the output stream.
 * @param os The output stream to operate on
 * @return os
 */
JSONOutputStream& startObject(JSONOutputStream& os);

/**
 * @brief End an object in the output stream.
 * @param os The output stream to operate on
 * @return os
 */
JSONOutputStream& endObject(JSONOutputStream& os);

/**
 * @brief Inser a null value in the output stream
 * @param os The output stream to operate on
 * @return os
 */
JSONOutputStream& nil(JSONOutputStream& os);

/**
 * @brief Functor used for writing a key into a JSONOutputStream
 */
struct Key {
    Key(const char* _str) : str(_str) { }
    const char* str;
};

/**
 * @brief Simple generator of a Key
 * @param str The str of the key
 * @return The Key object
 */
static inline Key key(const char* str) {
    return Key(str);
}

/**
 * @brief Insertion operator for a key into the JSON output stream
 * @param os The output stream to operate on
 * @param k The key to insert
 */
JSONOutputStream& operator<<(JSONOutputStream& os, const Key& k);


} // namespace serialization

#endif //BLE_CLIAPP_UTIL_JSON_OUTPUT_STREAM_H_
