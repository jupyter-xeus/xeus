#ifndef XMAKE_UNIQUE_HPP
#define XMAKE_UNIQUE_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace xeus
{

    // C++11 implementation of C++14 std::make_unique
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

#endif
