/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef dddwgc_bits_h
#define dddwgc_bits_h

#include <bitset>

namespace dddwgc {
#ifdef USE_V1_DEFAULT
inline
#else
#error Define USE_V1_DEFAULT
#endif
namespace _V1 {

template<typename... Args>
struct bits;

template<typename First, typename Second, typename... Tail>
struct bits<First, Second, Tail...>
{
    static constexpr size_t value = bits<First>::value + bits<Second, Tail...>::value;
};

template<typename Last>
struct bits<Last>
{
private:
    static constexpr size_t val() {
        if constexpr (std::is_enum_v<Last>)
            return bits<std::underlying_type_t<Last>>::value;
        return sizeof(Last) * 8;
    }

public:
    static constexpr size_t value = val();
};

template<>
struct bits<>
{
    static constexpr size_t value = 0;
};

template<>
struct bits<bool>
{
    static constexpr size_t value = 1;
};

template<typename... Args>
inline constexpr size_t bits_v = bits<Args...>::value;

} // namespace _V1
} // namespace dddwgc

#endif // dddwgc_bits_h
