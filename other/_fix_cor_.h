// Bypass GCC / MSVC coroutine guards when using clang code model
#if defined(__GNUC__) && defined(__clang__) && !defined(__cpp_impl_coroutine)
#define __cpp_impl_coroutine true
#elif defined(_MSC_VER) && defined(__clang__) && !defined(__cpp_lib_coroutine)
#define __cpp_lib_coroutine true
#endif
// Clang requires coroutine types in std::experimental
#include <coroutine>
#if defined(__clang__)
namespace std::experimental {
using std::coroutine_traits;
using std::coroutine_handle;
using std::suspend_always;
using std::suspend_never;
}
#endif

#define BOOST_ASIO_HAS_CO_AWAIT 1
#define BOOST_ASIO_HAS_STD_COROUTINE 1
