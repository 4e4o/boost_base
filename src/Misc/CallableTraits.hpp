#ifndef CALLABLE_TRAITS_HPP
#define CALLABLE_TRAITS_HPP

#include <tuple>
#include <concepts>

#include <stddef.h>

// from https://betterprogramming.pub/inspecting-properties-of-callables-with-function-traits-in-c-6bb2d9388fc5

/**
 *  Works with callables:
 *
 *  Lambdas
 *  Functions
 *  Member class functions
 *  Functors (without overloaded operator())
 *
 */

// for any type that implements operator()
template <typename Function>
struct CallableTraits :
        public CallableTraits<decltype(&std::remove_reference<Function>::type::operator())> { };

// convert const member function to function pointer version
template <typename ClassType, typename ReturnType, typename... Arguments>
struct CallableTraits<ReturnType(ClassType::*)(Arguments...) const> :
    CallableTraits<ReturnType(*)(Arguments...)> { };

// convert member function to function pointer version
template <typename ClassType, typename ReturnType, typename... Arguments>
struct CallableTraits<ReturnType(ClassType::*)(Arguments...)> :
    CallableTraits<ReturnType(*)(Arguments...)> { };

// The main version that we want to specialize is for function pointers
template <typename ReturnType, typename... Arguments>
struct CallableTraits<ReturnType(*)(Arguments...)> {
    typedef ReturnType result_type;
    typedef std::tuple<Arguments...> args_tuple;

    template <std::size_t Index>
    using arg = typename std::tuple_element<
    Index,
    std::tuple<Arguments...>
    >::type;

    static constexpr std::size_t arity = sizeof...(Arguments);
};

template <class Callable, class CallableResultBase,
          typename CallableResult =
          typename std::remove_pointer<typename CallableTraits<Callable>::result_type>::type>
concept CallablePointerResultDerived = std::is_base_of<CallableResultBase, CallableResult>::value;

/** Test example:
 *
 *
 *

#include <Misc/CallableTraits.hpp>
#include <functional>

template<typename Callable>
    void traits_test(Callable && c) {
    typedef CallableTraits<Callable> traits;
    typedef typename traits::result_type TResult;
    typedef typename traits::template arg<0> TArg0;

    const int args_count = traits::arity;

    static_assert(std::is_same<int, TResult>::value, "err");
    static_assert(std::is_same<int, TArg0>::value, "err");
}

struct Functor {
    int operator()(int a) {
        return 1;
    }
};

// not works because functor has overloaded operator()
struct NotCompiledFunctor {
    int operator()(int a) {
        return 1;
    }

    double operator()(int a, int b) {
        return 1;
    }

};

class MemberTest {
public:
    int someMethod(int a) {
        return 1;
    }
};

static int functionTest(int a) {
    return 1;
}

int main() {
    std::function<int(int)> sss;

    // std function functor test
    traits_test(sss);

    // functor test
    traits_test(Functor());

    // non working functor example
    //traits_test(NotCompiledFunctor());

    // lambda test
    traits_test([](int a) -> int {
        return 1;
    });

    // class member test
    traits_test(&MemberTest::someMethod);

    // function test
    traits_test(&functionTest);
    return 0;
}

*/

#endif // CALLABLE_TRAITS_HPP
