#ifndef BLE_CLIAPP_CONST_ARRAY_H_
#define BLE_CLIAPP_CONST_ARRAY_H_

#include <cstddef>
#include <stdint.h>


template<typename T>
struct ConstArray {

public:
	constexpr ConstArray() :
		_count(0), _elements(NULL) {
	}

	/**
	 * Construct a new array of elements from a size and pointer to an array
	 */
	constexpr ConstArray(int count, const T* elements) :
		_count(count), _elements(elements) {
	}


	template<size_t Elementscount>
	constexpr ConstArray(const T (&elements)[Elementscount]) :
		_count(Elementscount), _elements(elements) {
	}	


	/**
	 * return the count of arguments available
	 */ 
	constexpr size_t count() const {
		return _count;
	} 

	/**
	 * return the element at the given index
	 */
	constexpr const T& operator[](size_t index) const {
		return _elements[index];
	}



	constexpr ConstArray drop(size_t countTodrop) const {
		return (countTodrop < _count) ? ConstArray(_count - countTodrop, _elements + countTodrop) : ConstArray();
	}



// variables 	
private:	
	const size_t _count;
	const T* _elements;
};


template<typename T, size_t Elementscount>
constexpr ConstArray<T> makeConstArray(const T (&elements)[Elementscount]) {
	return ConstArray<T>(elements);
}





#endif //BLE_CLIAPP_CONST_ARRAY_H_