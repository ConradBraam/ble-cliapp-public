/**
 * @brief collection of polyfill for ARMCC
 */

#ifndef BLE_CLIAPP_UTIL_STANDARD_LIBRARY_POLYFILL_
#define BLE_CLIAPP_UTIL_STANDARD_LIBRARY_POLYFILL_

#if !defined(__CC_ARM)
#include <type_traits>
#include <utility>
#endif

namespace util {

#if defined(__CC_ARM)

namespace armcc_polyfill {

// implementation of is_empty type traits for armcc
struct BigObject {
    const char bigField[256];
};

template<typename T>
struct is_empty_checker : T, BigObject { };

template<typename T>
struct is_empty {
    static const bool value = sizeof(BigObject) == sizeof(is_empty_checker<T>);
};


template<class T> struct remove_reference      {typedef T type;};
template<class T> struct remove_reference<T&>  {typedef T type;};
template<class T> struct remove_reference<T&&> {typedef T type;};

template<class T>
typename remove_reference<T>::type&& move(T&& a) noexcept
{
    typedef typename remove_reference<T>::type&& RvalRef;
    return static_cast<RvalRef>(a);
}
template<class T>
T&& forward(typename remove_reference<T>::type& a) noexcept
{
    return static_cast<T&&>(a);
}

} // namespace armcc_polyfill

#endif


#if defined(__CC_ARM)
namespace polyfill = armcc_polyfill;
#else
namespace polyfill = std;
#endif


template<typename T>
using is_empty = polyfill::is_empty<T>;

template<typename T>
using remove_reference = polyfill::remove_reference<T>;

using polyfill::move;
using polyfill::forward;
}




#endif /* BLE_CLIAPP_UTIL_STANDARD_LIBRARY_POLYFILL_ */
