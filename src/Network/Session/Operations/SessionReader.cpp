#include "SessionReader.hpp"
#include "Network/Session/Session.hpp"
#include "Network/Socket/Socket.hpp"
#include "Misc/ScopeGuard.hpp"

TAwaitSize SessionReader::some(size_t maxSize) {
    const std::size_t size = std::min(maxSize, buffer().size());
    co_return co_await session()->socket()->co_readSome(ptr(), size);
}

TAwaitSize SessionReader::all(size_t maxSize) {
    const std::size_t size = std::min(maxSize, buffer().size());
    co_await session()->socket()->co_readAll(ptr(), size);
    co_return size;
}
