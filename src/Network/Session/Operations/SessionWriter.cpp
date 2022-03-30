#include "SessionWriter.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"
#include "Misc/ScopeGuard.hpp"

/*TAwaitVoid SessionWriter::all(std::vector<uint8_t>&& v) {
    if (v.size() > buffer().size())
        throw;

    std::copy(v.begin(), v.end(), buffer().begin());
    co_await all(ptr(), buffer().size());
}*/

TAwaitVoid SessionWriter::all(const uint8_t* ptr, size_t size) {
    co_return co_await session()->socket()->co_writeAll(ptr, size);
}
