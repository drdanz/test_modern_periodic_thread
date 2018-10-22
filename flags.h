/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

template<typename Enum,
         typename = typename std::enable_if_t<std::is_enum_v<Enum>>>
struct EnableBitMaskOperators : std::false_type {};

#define ENABLE_BITMASK_OPERATORS(x)                                  \
template<>                                                           \
struct EnableBitMaskOperators<x> : std::true_type {};


template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum operator |(Enum lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    return static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) |
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum operator &(Enum lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    return static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) &
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum operator ^(Enum lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    return static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) ^
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum operator ~(Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    return static_cast<Enum> (
        ~static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum& operator |=(Enum &lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    lhs = static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) |
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
    return lhs;
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum& operator &=(Enum &lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    lhs = static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) &
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
    return lhs;
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr Enum& operator ^=(Enum &lhs, Enum rhs)
{
    static_assert(std::is_enum_v<Enum>, "Enum is not an enum type");
    lhs = static_cast<Enum> (
        static_cast<typename std::underlying_type_t<Enum>>(lhs) ^
        static_cast<typename std::underlying_type_t<Enum>>(rhs)
    );
    return lhs;
}

template<typename Enum,
         typename = typename std::enable_if_t<EnableBitMaskOperators<Enum>::value>>
constexpr bool test_bit(Enum flags, Enum bit)
{
    return std::underlying_type_t<Enum>(flags & bit);
}
