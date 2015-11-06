#ifndef BLE_CLIAPP_DYNAMIC_VALUE_H_
#define BLE_CLIAPP_DYNAMIC_VALUE_H_

#include <cstdlib>
#include <cstdio>
#include "util/SimpleString.h"
#include "util/Vector.h"
#include "util/Map.h"

namespace dynamic {

/**
 * @brief This class can hold several type of value (like a variant)
 * @details Types wich can be hold by this class are:
 *   - null
 *   - bool
 *   - int64_t
 *   - double
 *   - SimpleString 
 *   - array of Value
 *   - map of (string, Value)
 */
class Value {

    static const int INDENT_WIDTH = 2;

public:
    enum class ValueType_t : uint8_t {
        NULL_TYPE,
        BOOL_TYPE,
        INT_TYPE,
        DOUBLE_TYPE,
        STRING_TYPE,
        ARRAY_TYPE,
        OBJECT_TYPE
    };

    typedef container::SimpleString String_t;
    typedef container::Vector<Value> Array_t;
    typedef container::Map<String_t, Value> Object_t;

    /**
     * @brief construct an empty value (null)
     */
    Value() : _bool(false), _type(ValueType_t::NULL_TYPE) { }

    Value(std::nullptr_t) : _bool(false), _type(ValueType_t::NULL_TYPE) { }

    Value(bool v) : _bool(v), _type(ValueType_t::BOOL_TYPE) { }

    Value(int64_t v) : _int(v), _type(ValueType_t::INT_TYPE) { }

    Value(double v) : _double(v), _type(ValueType_t::DOUBLE_TYPE) { }

    Value(const char* str) : Value(container::DynamicString(str)) { }

    Value(String_t v) : _string(std::move(v)), _type(ValueType_t::STRING_TYPE) { }

    Value(container::StaticString v) : _string(std::move(v)), _type(ValueType_t::STRING_TYPE) { }

    Value(container::DynamicString v) : _string(std::move(v)), _type(ValueType_t::STRING_TYPE) { }

    Value(Array_t v) : _array(std::move(v)), _type(ValueType_t::ARRAY_TYPE) { }

    Value(Object_t v) : _object(std::move(v)), _type(ValueType_t::OBJECT_TYPE) { }

    /**
     * @brief copy constructor of Value
     * @param that The Value object to copy
     */
    Value(const Value& that) : _type(that._type) {
        switch(that._type) { 
            case ValueType_t::NULL_TYPE:
                break;

            case ValueType_t::BOOL_TYPE:
                _bool = that._bool;
                break;

            case ValueType_t::INT_TYPE:
                _int = that._int;
                break;

            case ValueType_t::DOUBLE_TYPE:
                _double = that._double;
                break;

            case ValueType_t::STRING_TYPE:
                new(&_string) String_t(that._string);
                break;

            case ValueType_t::ARRAY_TYPE:
                new(&_array) Array_t(that._array);
                break;

            case ValueType_t::OBJECT_TYPE:
                new(&_object) Object_t(that._object);
                break;
        }
    }

    /**
     * @brief move constructor of Value
     * @param that The Value object to copy
     */
    Value(Value&& that) : _type(that._type) {
        switch(that._type) { 
            case ValueType_t::NULL_TYPE:
                break;

            case ValueType_t::BOOL_TYPE:
                _bool = that._bool;
                break;

            case ValueType_t::INT_TYPE:
                _int = that._int;
                break;

            case ValueType_t::DOUBLE_TYPE:
                _double = that._double;
                break;

            case ValueType_t::STRING_TYPE:
                new(&_string) String_t(std::move(that._string));
                break;

            case ValueType_t::ARRAY_TYPE:
                new(&_array) Array_t(std::move(that._array));
                break;

            case ValueType_t::OBJECT_TYPE:
                new(&_object) Object_t(std::move(that._object));
                break;
        }
    }

    ~Value() {
        switch(_type) { 
            case ValueType_t::NULL_TYPE:
            case ValueType_t::BOOL_TYPE:
            case ValueType_t::INT_TYPE:
            case ValueType_t::DOUBLE_TYPE:
                break;

            case ValueType_t::STRING_TYPE:
                _string.~String_t();
                break;

            case ValueType_t::ARRAY_TYPE:
                _array.~Array_t();
                break;

            case ValueType_t::OBJECT_TYPE:
                _object.~Object_t();
                break;
        }       
    }

    /**
     * @brief Assignement operator for Value
     * @details This construct will enable move and copy assignement
     * 
     * @param value The Value value to assign to this object
     */
    Value& operator=(Value that) { 
        if(_type == that._type) {
            switch(_type) {
                case ValueType_t::NULL_TYPE:
                    break;
                case ValueType_t::BOOL_TYPE:
                    _bool = that._bool;
                    break;
                case ValueType_t::INT_TYPE:
                    _int = that._int;
                    break;
                case ValueType_t::DOUBLE_TYPE:
                    _double = that._double;
                    break;
                case ValueType_t::STRING_TYPE:
                    _string = std::move(that._string);
                    break;
                case ValueType_t::ARRAY_TYPE:
                    _array = std::move(that._array);
                    break;
                case ValueType_t::OBJECT_TYPE:
                    _object = std::move(that._object);
                    break;              
            }
        } else {
            // destroy the current object
            switch(_type) {
                case ValueType_t::NULL_TYPE:
                case ValueType_t::BOOL_TYPE:
                case ValueType_t::INT_TYPE:
                case ValueType_t::DOUBLE_TYPE:
                    break;

                case ValueType_t::STRING_TYPE:
                    _string.~String_t();
                    break;
                case ValueType_t::ARRAY_TYPE:
                    _array.~Array_t();
                    break;
                case ValueType_t::OBJECT_TYPE:
                    _object.~Object_t();
                    break;              
            }

            // now, copy the new calue 
            switch(that._type) {
                case ValueType_t::NULL_TYPE:
                    break;
                case ValueType_t::BOOL_TYPE:
                    _bool = that._bool;
                    break;
                case ValueType_t::INT_TYPE:
                    _int = that._int;
                    break;
                case ValueType_t::DOUBLE_TYPE:
                    _double = that._double;
                    break;
                case ValueType_t::STRING_TYPE:
                    new(&_string) String_t(std::move(that._string));
                    break;
                case ValueType_t::ARRAY_TYPE:
                    new(&_array) Array_t(std::move(that._array));
                    break;
                case ValueType_t::OBJECT_TYPE:
                    new(&_object) Object_t(std::move(that._object));
                    break;              
            }

            _type = that._type;
        }

        return *this;
    }


    Value& operator[](size_t i) { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            // TODO : print something
            std::abort();
        }

        return _array[i];
    }

    const Value& operator[](size_t i) const { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            // TODO : print something
            std::abort();
        }

        return _array[i];
    }

    Value& operator[](const String_t& key) {
        if(_type != ValueType_t::OBJECT_TYPE) { 
            *this = Value { Object_t { } };
        }

        return _object[key];
    }

    Value& operator[](String_t&& key) { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            *this = Value { Object_t { } };
        }

        return _object[std::move(key)];
    }

    typename Array_t::iterator array_begin() { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            return nullptr;
        }
        return _array.begin();
    }

    typename Array_t::const_iterator array_cbegin() const { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            return nullptr;
        }
        return _array.cbegin();
    }

    typename Array_t::iterator array_end() { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            return nullptr;
        }
        return _array.end();
    }

    typename Array_t::const_iterator array_cend() const { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            return nullptr;
        }
        return _array.cend();
    }

    typename Object_t::iterator object_begin() { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            return nullptr;
        }
        return _object.begin();
    }

    typename Object_t::const_iterator object_cbegin() const { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            return nullptr;
        }
        return _object.cbegin();
    }

    typename Object_t::iterator object_end() { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            return nullptr;
        }
        return _object.end();
    }

    typename Object_t::const_iterator object_cend() const { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            return nullptr;
        }
        return _object.cend();
    }

    void push_back(const Value& v) {
        if(_type != ValueType_t::ARRAY_TYPE) { 
            *this = Value { Array_t { } };
        }

        _array.push_back(v);
    }

    void push_back(Value&& v) {
        if(_type != ValueType_t::ARRAY_TYPE) {
            *this = Value { Array_t { } };
        }

        _array.push_back(std::move(v));
    }

    ValueType_t type() const { 
        return _type;
    } 

    std::nullptr_t as_null() const { 
        if(_type != ValueType_t::NULL_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return nullptr;
    }

    bool as_bool() const { 
        if(_type != ValueType_t::BOOL_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _bool;
    }

    int64_t as_int() const { 
        if(_type != ValueType_t::INT_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _int;
    }

    double as_double() const { 
        if(_type != ValueType_t::DOUBLE_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _double;
    }

    String_t& as_string() { 
        if(_type != ValueType_t::STRING_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _string;
    }

    const String_t& as_string() const { 
        if(_type != ValueType_t::STRING_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _string;
    }

    Array_t& as_array() { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _array;
    }

    const Array_t& as_array() const { 
        if(_type != ValueType_t::ARRAY_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _array;
    }

    Object_t& as_object() { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _object;
    }

    const Object_t& as_object() const { 
        if(_type != ValueType_t::OBJECT_TYPE) { 
            // TODO : print something
            std::abort();
        }
        return _object;
    }

    // TODO cast operator ????

    friend bool operator==(const Value& lhs, const Value& rhs) { 
        if(lhs._type != rhs._type) {
            return false;
        }

        switch(lhs._type) { 
            case ValueType_t::NULL_TYPE:
                return true;
            case ValueType_t::BOOL_TYPE:
                return lhs._bool == rhs._bool;
            case ValueType_t::INT_TYPE:
                return lhs._int == rhs._int;
            case ValueType_t::DOUBLE_TYPE:
                return lhs._double == rhs._double;
            case ValueType_t::STRING_TYPE:
                return lhs._string == rhs._string;
            case ValueType_t::ARRAY_TYPE:
                return lhs._array == rhs._array;
            case ValueType_t::OBJECT_TYPE:
                return lhs._object == rhs._object;
        }

        return false;
    }

    friend bool operator!=(const Value& lhs, const Value& rhs) {
        return !(lhs == rhs);
    }

    String_t to_str() const {
        switch(_type) { 
            case ValueType_t::NULL_TYPE:
                return "null"_ss;
            case ValueType_t::BOOL_TYPE:
                return _bool ? "true"_ss : "false"_ss;
            case ValueType_t::INT_TYPE: {
                char buf[sizeof("-9223372036854775808")];
                size_t size = snprintf(buf, sizeof(buf), "%ld", (int32_t) _int);
                return container::DynamicString(buf, size);
            }
            case ValueType_t::DOUBLE_TYPE: {
                char buf[256];
                size_t size = snprintf(buf, sizeof(buf), "%f" , _double);
                return container::DynamicString(buf, size);
            }
            case ValueType_t::STRING_TYPE:
                return _string;
            case ValueType_t::ARRAY_TYPE:
                return "array"_ss;
            case ValueType_t::OBJECT_TYPE:
                return "object"_ss;
        }
        return ""_ss;
    }   

    template <typename Iter> 
    static void copy(const String_t& s, Iter oi) {
        std::copy(s.cbegin(), s.cend(), oi);
    }

    template <typename Iter> 
    static void serialize_str(const String_t& s, Iter oi) {
        *oi++ = '"';
        for (String_t::const_iterator i = s.cbegin(); i != s.cend(); ++i) {
            switch (*i) {
#define MAP(val, sym) case val: copy(sym, oi); break
                MAP('"', "\\\""_ss);
                MAP('\\', "\\\\"_ss);
                MAP('/', "\\/"_ss);
                MAP('\b', "\\b"_ss);
                MAP('\f', "\\f"_ss);
                MAP('\n', "\\n"_ss);
                MAP('\r', "\\r"_ss);
                MAP('\t', "\\t"_ss);
#undef MAP
                default:
                    if (static_cast<unsigned char>(*i) < 0x20 || *i == 0x7f) {
                        char buf[7];
                        snprintf(buf, sizeof(buf), "\\u%04x", *i & 0xff);
                        std::copy(buf, buf + 6, oi);
                    } else {
                        *oi++ = *i;
                    }
                    break;
            }
        }
        *oi++ = '"';
    }

    template <typename Iter> 
    void serialize(Iter oi, bool prettify) const {
        return _serialize(oi, prettify ? 0 : -1);
    }

    template <typename Iter> 
    void _serialize(Iter oi, int indent) const {
        switch (_type) {
            case ValueType_t::STRING_TYPE:
                serialize_str(_string, oi);
                break;
            case ValueType_t::ARRAY_TYPE: {
                *oi++ = '[';
                if (indent != -1) {
                    ++indent;
                }
                for (Array_t::const_iterator i = _array.cbegin(); i != _array.cend(); ++i) {
                    if (i != _array.cbegin()) {
                        *oi++ = ',';
                    }
                    if (indent != -1) {
                        _indent(oi, indent);
                    }
                    i->_serialize(oi, indent);
                }
                if (indent != -1) {
                    --indent;
                    if (! _array.size() == 0) {
                        _indent(oi, indent);
                    }
                }
                *oi++ = ']';
                break;
            }
            case ValueType_t::OBJECT_TYPE: {
                *oi++ = '{';
                if (indent != -1) {
                    ++indent;
                }
                for (Object_t::const_iterator i = _object.cbegin(); i != _object.cend(); ++i) {
                    if (i != _object.cbegin()) {
                        *oi++ = ',';
                    }
                    if (indent != -1) {
                        _indent(oi, indent);
                    }
                    serialize_str(i->first, oi);
                    *oi++ = ':';
                    if (indent != -1) {
                        *oi++ = ' ';
                    }
                    i->second._serialize(oi, indent);
                }
                if (indent != -1) {
                    --indent;
                    if (! _object.empty()) {
                        _indent(oi, indent);
                    }
                }
                *oi++ = '}';
                break;
            }
            default:
                copy(to_str(), oi);
                break;
        }
        if (indent == 0) {
            *oi++ = '\n';
        }
    }

    template <typename Iter> 
    static void _indent(Iter oi, int indent) {
        *oi++ = '\r';
        *oi++ = '\n';
        for(int i = 0; i < indent * INDENT_WIDTH; ++i) {
          *oi++ = ' ';
        }
    }

private:
    union { 
        // no need for null type here
        bool _bool;
        int64_t _int;
        double  _double;
        String_t _string;
        Array_t _array;
        Object_t _object;
    };

    ValueType_t _type;
};

} // namespace dynamic

#endif //BLE_CLIAPP_DYNAMIC_VALUE_H_
