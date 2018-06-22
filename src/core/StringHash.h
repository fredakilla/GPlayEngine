#ifndef STRINGHASH_H
#define STRINGHASH_H

#include <cstdint>

///----------------------------------------------------------------------------------------------------
/// C++11 compile-time hash of literal strings.
///----------------------------------------------------------------------------------------------------
namespace detail
{
    // FNV-1a 32bit hashing algorithm.
    constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
    {
        return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
    }
}

constexpr std::uint32_t operator"" _hash(char const* s, std::size_t count)
{
    return detail::fnv1a_32(s, count);
}


#endif // STRINGHASH_H
