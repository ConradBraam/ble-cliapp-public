#ifndef BLE_CLIAPP_UTIL_STATIC_LAMBDA_H_
#define BLE_CLIAPP_UTIL_STATIC_LAMBDA_H_

#include <type_traits>
#include <cstddef>
#include <stdint.h>

/*
 * for more details, see : http://pfultz2.com/blog/2014/09/02/static-lambda/
 */
namespace static_lambda { 

template<typename T>
struct CallGenerator;

template<typename R, typename F>
struct CallGenerator<R (F::*)() const> {
	static_assert(std::is_empty<F>::value, "invalid, F should be empty");

	static R call() {
		F* f = nullptr;
		return (*f)();
	}
};

template<typename R, typename F, typename Arg0>
struct CallGenerator<R (F::*)(Arg0) const> {
	static_assert(std::is_empty<F>::value, "invalid, F should be empty");

	static R call(Arg0 arg0) {
		F* f = nullptr;
		return (*f)(arg0);
	}
};

struct wrapper_factor
{
	typedef void(*EmptyFunc_t)();

    template<class F>
    constexpr auto operator += (F*) -> decltype(&CallGenerator<decltype(&F::operator())>::call)
    {
    	return CallGenerator<decltype(&F::operator())>::call;
    }
};

struct addr_add
{
    template<class T>
    friend typename std::remove_reference<T>::type *operator+(addr_add, T &&t) 
    {
        return &t;
    }
};

} // end of namespace static_lambda

#define STATIC_LAMBDA static_lambda::wrapper_factor() += true ? nullptr : static_lambda::addr_add() + []

#endif //BLE_CLIAPP_UTIL_STATIC_LAMBDA_H_