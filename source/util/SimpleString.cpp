

#include <memory>
#include <utility>
#include <cstring>

#include "SimpleString.h"

namespace container {

SimpleString::SimpleString() : _static() {}

SimpleString::SimpleString(const char* str, ImplementationType_t type) :
    _type(type) {
    if(_type == ImplementationType_t::STATIC_STRING) {
        new(&_static) StaticString(str);
    } else {
        new(&_dynamic) DynamicString(str);
    }
}

SimpleString::SimpleString(const StaticString& staticString) :
    _static(staticString), _type(ImplementationType_t::STATIC_STRING) {
}

SimpleString::SimpleString(StaticString&& staticString) :
    _static(staticString), _type(ImplementationType_t::STATIC_STRING) {
}

SimpleString::SimpleString(const DynamicString& dynamicString) :
    _dynamic(dynamicString), _type(ImplementationType_t::DYNAMIC_STRING) {
}

SimpleString::SimpleString(DynamicString&& dynamicString) :
    _dynamic(dynamicString), _type(ImplementationType_t::DYNAMIC_STRING) {
}

SimpleString::SimpleString(const SimpleString& that) :
    _type(that._type) {
    if(_type == ImplementationType_t::STATIC_STRING) {
        new(&_static) StaticString(that._static);
    } else {
        new(&_dynamic) DynamicString(that._dynamic);
    }
}

SimpleString::SimpleString(SimpleString&& that) :
    _type(that._type) {
    if(_type == ImplementationType_t::STATIC_STRING) {
        new(&_static) StaticString(std::move(that._static));
    } else {
        new(&_dynamic) DynamicString(std::move(that._dynamic));
    }
}

SimpleString::~SimpleString() {
    if(_type == ImplementationType_t::STATIC_STRING) {
        _static.~StaticString();
    } else {
        _dynamic.~DynamicString();
    }
}

SimpleString& SimpleString::operator=(const SimpleString& that) {
    if(_type == that._type) {
        if(_type == ImplementationType_t::STATIC_STRING) {
            _static = that._static;
        } else {
            _dynamic = that._dynamic;
        }
    } else {
        if(_type == ImplementationType_t::STATIC_STRING) {
            _static.~StaticString();
            new(&_dynamic) DynamicString(that._dynamic);
        } else {
            _dynamic.~DynamicString();
            new(&_static) StaticString(that._static);
        }
        _type = that._type;
    }

    return *this;
}

SimpleString& SimpleString::operator=(SimpleString&& that) {
    if(_type == that._type) {
        if(_type == ImplementationType_t::STATIC_STRING) {
            _static = std::move(that._static);
        } else {
            _dynamic = std::move(that._dynamic);
        }
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

const char* SimpleString::c_str() const {
    if(_type == ImplementationType_t::STATIC_STRING) {
        return _static.c_str();
    } else {
        return _dynamic.c_str();
    }
}

size_t SimpleString::size() const {
    if(_type == ImplementationType_t::STATIC_STRING) {
        return _static.size();
    } else {
        return _dynamic.size();
    }
}

SimpleString::const_iterator SimpleString::cbegin() const {
    if(_type == ImplementationType_t::STATIC_STRING) {
        return _static.c_str();
    } else {
        return _dynamic.c_str();
    }
}

SimpleString::const_iterator SimpleString::cend() const {
    if(_type == ImplementationType_t::STATIC_STRING) {
        return _static.c_str() + _static.size();
    } else {
        return _dynamic.c_str() + _dynamic.size();
    }
}

bool operator==(const SimpleString& lhs, const SimpleString& rhs) {
    if(lhs._type == SimpleString::ImplementationType_t::STATIC_STRING) {
        if(rhs._type == SimpleString::ImplementationType_t::STATIC_STRING) {
            return lhs._static.size() == rhs._static.size() && memcmp(lhs._static.c_str(), rhs._static.c_str(), lhs._static.size()) == 0;
        } else {
            return lhs._static.size() == rhs._dynamic.size() && memcmp(lhs._static.c_str(), rhs._dynamic.c_str(), lhs._static.size()) == 0;
        }
    } else {
        if(rhs._type == SimpleString::ImplementationType_t::STATIC_STRING) {
            return lhs._dynamic.size() == rhs._static.size() && memcmp(lhs._dynamic.c_str(), rhs._static.c_str(), lhs._dynamic.size()) == 0;
        } else {
            return lhs._dynamic.size() == rhs._dynamic.size() && memcmp(lhs._dynamic.c_str(), rhs._dynamic.c_str(), lhs._dynamic.size()) == 0;
        }
    }
}

bool operator!=(const SimpleString& lhs, const SimpleString& rhs) {
    return !(lhs == rhs);
}

} // namespace container
