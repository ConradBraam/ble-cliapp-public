#ifndef BLE_CLIAPP_CONST_ARRAY_H_
#define BLE_CLIAPP_CONST_ARRAY_H_

#include <stdint.h>


template<typename T>
struct ConstArray {

public: 
	/**
	 * Construct a new array of elements from a size and pointer to an array
	 */
	ConstArray(int count, const T* elements) :
		_count(count), _elements(elements) {
	}


	template<size_t Elementscount>
	ConstArray(const T (&elements)[Elementscount]) :
		_count(Elementscount), _elements(elements) {
	}	


	/**
	 * return the count of arguments available
	 */ 
	size_t count() const {
		return _count;
	} 

	/**
	 * return the element at the given index
	 */
	const T& operator[](size_t index) const {
		return _elements[index];
	}



	ConstArray drop(size_t countTodrop) const {
		if(countTodrop < _count) {
			return ConstArray(_count - countTodrop, _elements + countTodrop);
		} 
		return ConstArray(0, NULL);
	}



// variables 	
private:	
	const size_t _count;
	const T* _elements;
};





#endif //BLE_CLIAPP_CONST_ARRAY_H_