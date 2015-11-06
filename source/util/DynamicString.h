#ifndef BLE_CLIAPP_DYNAMIC_DYNAMIC_STRING_H_
#define BLE_CLIAPP_DYNAMIC_DYNAMIC_STRING_H_

#include <memory>
#include <cstring>

namespace container {

class DynamicString {

public:
	/**
	 * @brief construct an empty DynamicString
	 */
	DynamicString() : _str(), _size(0) {}

	DynamicString(const char* str, size_t size) : _str(new char[size + 1]), _size(size) { 
		std::memcpy(_str.get(), str, size);
		_str[size] = '\0';
	}

	DynamicString(const char* str) : DynamicString(str, strlen(str)) { }

	template<size_t N>
	DynamicString(const char (&str)[N]) : _str(new char[N]), _size(N - 1) { 
		std::memcpy(_str.get(), str, N);
	}

	DynamicString(const DynamicString& that) : _str(new char[that._size + 1]), _size(that._size) { 
		std::memcpy(_str.get(), that._str.get(), _size + 1);
	}

	DynamicString(DynamicString&& that) : _str(std::move(that._str)), _size(that._size) { 
		that._size = 0;
	}

	DynamicString& operator=(DynamicString that) { 
		std::swap(this->_str, that._str);
		std::swap(this->_size, that._size);
		return *this;
	}

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