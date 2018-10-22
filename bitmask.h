/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef dddwgc_bitmask_h
#define dddwgc_bitmask_h

#include <bitset>

namespace dddwgc {
#ifdef USE_V1_DEFAULT
inline
#else
#error Define USE_V1_DEFAULT
#endif
namespace _V1 {

template<size_t total_size, size_t size = total_size, size_t shift = 0>
struct bitmask
{
private:
    static constexpr unsigned long long generate_mask()
    {
        unsigned long long ret = 0;
        for (size_t i = shift; i < shift + size; ++i) {
            ret |= (1ULL << i);
        }
        return ret;
    }

public:
    static constexpr std::bitset<total_size> value = generate_mask();
    template<class CharT = char,
             class Traits = std::char_traits<CharT>,
             class Allocator = std::allocator<CharT>>
    std::basic_string<CharT, Traits, Allocator> to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const
    {
        return value.template to_string<CharT, Traits, Allocator>(zero, one);
    }
};

template<size_t total_size, size_t size = total_size, size_t shift = 0>
inline constexpr std::bitset<total_size> bitmask_v = bitmask<total_size, size, shift>::value;

} // namespace _V1
} // namespace dddwgc

#endif // dddwgc_bitmask_h
