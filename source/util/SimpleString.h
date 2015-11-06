#ifndef BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_
#define BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_

#include <memory>
#include <utility>
#include <cstring>
#include "StaticString.h"
#include "DynamicString.h"

namespace container {

class SimpleString {

    enum class ImplementationType_t : bool {
        STATIC_STRING,
        DYNAMIC_STRING
    };

public:
    typedef const char* const_iterator;

    /**
     * @brief construct an empty DynamicString
     */
    SimpleString() : _static() {}

    SimpleString(const StaticString& staticString) : _static(staticString), _type(ImplementationType_t::STATIC_STRING) { }

    SimpleString(StaticString&& staticString) : _static(staticString), _type(ImplementationType_t::STATIC_STRING) { }

    SimpleString(const DynamicString& dynamicString) : _dynamic(dynamicString), _type(ImplementationType_t::DYNAMIC_STRING) { }

    SimpleString(DynamicString&& dynamicString) : _dynamic(dynamicString), _type(ImplementationType_t::DYNAMIC_STRING) { }

    SimpleString(const SimpleString& that) : _type(that._type) { 
        if(_type == ImplementationType_t::STATIC_STRING) {
            new(&_static) StaticString(that._static);
        } else {
            new(&_dynamic) DynamicString(that._dynamic);
        }
    }

    SimpleString(SimpleString&& that) : _type(that._type) { 
        if(_type == ImplementationType_t::STATIC_STRING) {
            new(&_static) StaticString(std::move(that._static));
        } else {
            new(&_dynamic) DynamicString(std::move(that._dynamic));
        }
    }

    ~SimpleString() { 
        if(_type == ImplementationType_t::STATIC_STRING) {
            _static.~StaticString();
        } else {
            _dynamic.~DynamicString();
        }
    }


    SimpleString& operator=(SimpleString that) {
        if(_type == that._type) { 
            if(_type == ImplementationType_t::STATIC_STRING) {
                std::swap(_static, that._static);
            } else {
                std::swap(_dynamic, that._dynamic);
            }
            std::swap(_type, that._type);
        } else {
            if(_type == ImplementationType_t::STATIC_STRING) { 
                _static.~StaticString();
                new(&_dynamic) DynamicString(std::move(that._dynamic));
            } else {
                _dynamic.~DynamicString();
                new(&_static) StaticString(std::move(that._static));
            }
            _type = that._type;
        }

        return *this;
    }

    /**
     * @brief return the c string held
     */
    const char* c_str() const { 
        if(_type == ImplementationType_t::STATIC_STRING) {
            return _static.c_str();
        } else {
            return _dynamic.c_str();
        } 
    }

    /**
     * length of the string without the null terminated character
     */
    size_t size() const {  
        if(_type == ImplementationType_t::STATIC_STRING) {
            return _static.size();
        } else {
            return _dynamic.size();
        } 
    }   

    friend bool operator==(const SimpleString& lhs, const SimpleString& rhs) { 
        if(lhs._type == ImplementationType_t::STATIC_STRING) { 
            if(rhs._type == ImplementationType_t::STATIC_STRING) { 
                return lhs._static.size() == rhs._static.size() && memcmp(lhs._static.c_str(), rhs._static.c_str(), lhs._static.size()) == 0;
            } else {
                return lhs._static.size() == rhs._dynamic.size() && memcmp(lhs._static.c_str(), rhs._dynamic.c_str(), lhs._static.size()) == 0;
            }
        } else {
            if(rhs._type == ImplementationType_t::STATIC_STRING) { 
                return lhs._dynamic.size() == rhs._static.size() && memcmp(lhs._dynamic.c_str(), rhs._static.c_str(), lhs._dynamic.size()) == 0;
            } else {
                return lhs._dynamic.size() == rhs._dynamic.size() && memcmp(lhs._dynamic.c_str(), rhs._dynamic.c_str(), lhs._dynamic.size()) == 0;
            }
        }
    }

    friend bool operator!=(const SimpleString& lhs, const SimpleString& rhs) {
        return !(lhs == rhs);
    }

    const_iterator cbegin() const {
        if(_type == ImplementationType_t::STATIC_STRING) {
            return _static.c_str();
        } else {
            return _dynamic.c_str();
        }
    }

    const_iterator cend() const {
        if(_type == ImplementationType_t::STATIC_STRING) {
            return _static.c_str() + _static.size();
        } else {
            return _dynamic.c_str() + _dynamic.size();
        }
    }

private:
    union {
        StaticString _static;
        DynamicString _dynamic;
    };
    ImplementationType_t _type;
};

} // namespace container

#endif //BLE_CLIAPP_DYNAMIC_SIMPLE_STRING_H_
