#ifndef BLE_CLIAPP_UTIL_LAMBDA_HELPERS_
#define BLE_CLIAPP_UTIL_LAMBDA_HELPERS_

#include <type_traits>

namespace OneShotCallBackDetail {

template<typename T>
struct getMemberFunctionArgType;

template<typename R, typename ClassType, typename Arg>
struct getMemberFunctionArgType<R (ClassType::*)(Arg) const> {
    typedef Arg type;
};

template<typename T>
struct getLambdaArgType {
    typedef typename getMemberFunctionArgType<decltype(&T::operator())>::type type;
};

template<typename LambdaType>
struct OneShotCallBack {

    typedef typename getLambdaArgType<LambdaType>::type ArgType;

    OneShotCallBack(LambdaType lambda) : func(lambda) { }

    void operator()(ArgType arg) {
        func(arg);
        delete this;
    }

    FunctionPointerWithContext<ArgType> getFP() {
        return makeFunctionPointer(this, &OneShotCallBack<LambdaType>::operator());
    }

    LambdaType func;
};

}

template<typename V>
FunctionPointerWithContext<typename OneShotCallBackDetail::getLambdaArgType<V>::type> makeOneShotCallbackFP(V&& lambda) {
    using namespace OneShotCallBackDetail;
    auto foo = new OneShotCallBack<V>(std::forward<V>(lambda));
    return foo->getFP();
}


template<typename V>
FunctionPointerWithContext<typename OneShotCallBackDetail::getLambdaArgType<V>::type> makeNewOneShotCallback(V&& lambda) {
    using namespace OneShotCallBackDetail;
    return new OneShotCallBack<V>(std::forward<V>(lambda));
}





namespace RepeatedCallbackDetail {

template<typename T>
struct getMemberFunctionArgType;

template<typename R, typename ClassType, typename Arg>
struct getMemberFunctionArgType<R (ClassType::*)(Arg, bool&) const> {
    typedef Arg type;
};

template<typename T>
struct getLambdaArgType {
    typedef typename getMemberFunctionArgType<decltype(&T::operator())>::type type;
};

template<typename LambdaType>
struct RepeatedCallback {

    typedef typename getLambdaArgType<LambdaType>::type ArgType;

    RepeatedCallback(LambdaType lambda) : func(lambda) { }

    void operator()(ArgType arg) {
        bool terminate = false;
        func(arg, terminate);
        if(terminate) {
            delete this;
        }
    }

    FunctionPointerWithContext<ArgType> getFP() {
        return makeFunctionPointer(this, &RepeatedCallback<LambdaType>::operator());
    }

    LambdaType func;
};

}

template<typename V>
FunctionPointerWithContext<typename RepeatedCallbackDetail::getLambdaArgType<V>::type> makeRepeatedCallback(V&& lambda) {
    using namespace RepeatedCallbackDetail;
    auto foo = new RepeatedCallback<V>(std::forward<V>(lambda));
    return foo->getFP();
}

#endif //BLE_CLIAPP_UTIL_LAMBDA_HELPERS_
