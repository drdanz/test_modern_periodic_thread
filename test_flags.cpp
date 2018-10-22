#include "bitmask.h"
#include "bits.h"


#include <iostream>
#include <bitset>
#include <climits>



template<typename T, typename... Args>
struct unary_operators;

template<typename T, typename First, typename Second, typename... Tail>
struct unary_operators<T, First, Second, Tail...>
        : unary_operators<T, First>
        , unary_operators<T, Second, Tail...>
{
};

template<typename T, typename Arg>
struct unary_operators<T, Arg>
{
    friend constexpr T& operator |=(T& lhs, Arg rhs)
    {
        return lhs |= static_cast<T>(rhs);
    }

    friend constexpr T operator |(T lhs, Arg rhs)
    {
        return lhs | static_cast<T>(rhs);
    }

    friend constexpr T operator |(Arg lhs, T rhs)
    {
        return static_cast<T>(lhs) | rhs;
    }
};

// =============================================================================



template<typename T, typename... Args>
struct binary_operators;

template<typename T, typename First, typename Second, typename Third, typename... Tail>
struct binary_operators<T, First, Second, Third, Tail...>
        : binary_operators<T, First, Second>
        , binary_operators<T, First, Third, Tail...>
        , binary_operators<T, Second, Third, Tail...>
{
};

template<typename T, typename First, typename Second>
struct binary_operators<T, First, Second>
{
//     friend constexpr T operator |(First lhs, Second rhs)
//     {
//         return static_cast<T>(lhs) | static_cast<T>(rhs);
//     }
// 
//     friend constexpr T operator |(Second lhs, First rhs)
//     {
//         return static_cast<T>(lhs) | static_cast<T>(rhs);
//     }
};



// =============================================================================


template<size_t sz, typename... Args>
struct XXX;

template<size_t sz, typename First, typename Second, typename... Tail>
struct XXX<sz, First, Second, Tail...>
        : XXX<sz + dddwgc::bits_v<First>, Second, Tail...>
{
    static_assert(std::is_enum_v<First>, "First is not an enum type");

    using next = XXX<sz + dddwgc::bits_v<First>, Second, Tail...>;

    static constexpr size_t size = dddwgc::bits_v<First>;
    static constexpr size_t total_size = next::total_size;
    static constexpr size_t shift = total_size - dddwgc::bits_v<Second, Tail...> - size;

    using type = typename next::type;

    static constexpr type bitmask = dddwgc::bitmask_v<total_size, size, shift>;

    // Constructors
#if defined(__clang__)
    using XXX<sz + dddwgc::bits_v<First>, Second, Tail...>::XXX;
#else
    using next::XXX;
#endif

    explicit constexpr XXX() = default;

#ifdef EXPLICIT_CONSTRUCTOR
    explicit
#endif
    constexpr XXX(First arg)
    {
        next::_bs |= (static_cast<type>(static_cast<typename std::underlying_type_t<First>>(arg)) << shift);
    }

    // Cast operator
#ifdef EXPLICIT_CAST
    explicit
#endif
    constexpr operator First() const
    {
        return static_cast<First>(((next::_bs & XXX::bitmask) >> shift).to_ullong()) ;
    }
};

// =============================================================================

template<size_t sz, typename Last>
struct XXX<sz, Last>
{
    static_assert(std::is_enum_v<Last>, "Last is not an enum type");

    static constexpr size_t size = dddwgc::bits_v<Last>;
    static constexpr size_t total_size = sz + size;
    static constexpr size_t shift = sz;

    using type = std::bitset<total_size>;

    static constexpr type bitmask = dddwgc::bitmask_v<total_size, size, shift>;

    // The actual container
    type _bs;

    // Constructors
    explicit constexpr XXX() = default;

#ifdef EXPLICIT_CONSTRUCTOR
    explicit
#endif
    constexpr XXX(Last arg)
    {
        _bs |= (static_cast<type>(static_cast<typename std::underlying_type_t<Last>>(arg)) << shift);
        std::cout << " [CTOR  ] ---> " << _bs.to_string('_', 'X') << std::endl;
    }

    // Cast operator
#ifdef EXPLICIT_CAST
    explicit
#endif
    constexpr operator Last() const
    {
        return static_cast<Last>(((_bs & bitmask) >> shift).to_ullong()) ;
    }
};




// =============================================================================

template<typename... Args>
struct YYY
        : XXX<0, Args...>
        , unary_operators<YYY<Args...>, Args...>
        , binary_operators<YYY<Args...>, Args...>
{
    // Constructors
    using XXX<0, Args...>::XXX;

    // Operators
    constexpr YYY& operator |=(YYY rhs)
    {
        this->_bs |= rhs._bs;
        return *this;
    }

    friend constexpr YYY operator |(YYY lhs, YYY rhs)
    {
        auto x = lhs;
        x._bs |= rhs._bs;
        return x;
    }

};

// =============================================================================


enum class A : bool
{
    No = 0,
    Yes = 1,
};

enum class B : bool
{
    No = 0,
    Yes = 1,
};

enum class C : bool
{
    No = 0,
    Yes = 1,
};

enum class D : unsigned char
{
    Opt00 = 0b00,
    Opt01 = 0b01,
    Opt10 = 0b10,
    Opt11 = 0b11,
};

enum class E : unsigned char
{
    Opt00 = 0b00,
    Opt01 = 0b01,
    Opt10 = 0b10,
    Opt11 = 0b11,
};

enum class F : unsigned char
{
    Opt000 = 0b000,
    Opt001 = 0b001,
    Opt010 = 0b010,
    Opt011 = 0b011,
    Opt100 = 0b100,
    Opt101 = 0b101,
    Opt110 = 0b110,
    Opt111 = 0b111,
};


enum class PAD
{
};

enum class Y : bool
{
    No = 0,
    Yes = 1,
};

enum class Z : bool
{
    No = 0,
    Yes = 1,
};


// Force E to squeeze in 2 dddwgc::bits
// FIXME find a way to ensure that this is enough to fit all options
template<>
struct dddwgc::bits<E>
{
    static constexpr size_t value = 2;
};

// Force F to squeeze in 3 dddwgc::bits
// FIXME find a way to ensure that this is enough to fit all options
template<>
struct dddwgc::bits<F>
{
    static constexpr size_t value = 3;
};


// Add some padding
template<>
struct dddwgc::bits<PAD>
{
    static constexpr size_t value = 4;
};



using Flags = YYY<A,B,C,D,E,F,PAD>;

template<typename T1,
         typename T2,
         typename = typename std::enable_if_t<std::is_enum_v<T1>
                                           && std::is_enum_v<T2>
                                           && std::is_convertible_v<T1, Flags>
                                           && std::is_convertible_v<T2, Flags>>>
constexpr Flags operator |(T1 lhs, T2 rhs)
{
    Flags ret{lhs};
    ret |= rhs;
    return ret;
}


using FlagsZ = YYY<B,Y,Z>;

template<typename T1,
         typename T2,
         typename = typename std::enable_if_t<std::is_enum_v<T1>
                                           && std::is_enum_v<T2>
                                           && std::is_convertible_v<T1, FlagsZ>
                                           && std::is_convertible_v<T2, FlagsZ>>>
constexpr FlagsZ operator |(T1 lhs, T2 rhs)
{
    FlagsZ ret{lhs};
    ret |= rhs;
    return ret;
}




void print(Flags flags)
{
        std::cout << std::endl;
        std::cout << "------------------------------------" << std::endl;
#ifdef EXPLICIT_CAST
        A a = static_cast<A>(flags);
        B b = static_cast<B>(flags);
        C c = static_cast<C>(flags);
        D d = static_cast<D>(flags);
        E e = static_cast<E>(flags);
        F f = static_cast<F>(flags);
#else
        A a = flags;
        B b = flags;
        C c = flags;
        D d = flags;
        E e = flags;
        F f = flags;
#endif
        std::cout << "bs = " << flags._bs.to_string('_', 'X') << std::endl;
        std::cout << "a = " << (a == A::Yes ? "Yes" : "No") << std::endl;
        std::cout << "b = " << (b == B::Yes ? "Yes" : "No") << std::endl;
        std::cout << "c = " << (c == C::Yes ? "Yes" : "No") << std::endl;
        std::cout << "d = " << (d == D::Opt00 ? "Opt00" : (d == D::Opt01 ? "Opt01" : (d == D::Opt10 ? "Opt10" : "Opt11"))) << std::endl;
        std::cout << "e = " << (e == E::Opt00 ? "Opt00" : (e == E::Opt01 ? "Opt01" : (e == E::Opt10 ? "Opt10" : "Opt11"))) << std::endl;
        std::cout << "f = " << (f == F::Opt000 ? "Opt000" :
                               (f == F::Opt001 ? "Opt001" :
                               (f == F::Opt010 ? "Opt010" :
                               (f == F::Opt011 ? "Opt011" :
                               (f == F::Opt100 ? "Opt100" :
                               (f == F::Opt101 ? "Opt101" :
                               (f == F::Opt110 ? "Opt110" :
                               "Opt111"))))))) << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << std::endl;
}



int main()
{
    std::cout << "\n====== Test bits ======\n" << std::endl;

    std::cout << "dddwgc::bits_v<A> = " << dddwgc::bits_v<A> << std::endl;
    std::cout << "dddwgc::bits_v<B> = " << dddwgc::bits_v<B> << std::endl;
    std::cout << "dddwgc::bits_v<C> = " << dddwgc::bits_v<C> << std::endl;
    std::cout << "dddwgc::bits_v<D> = " << dddwgc::bits_v<D> << std::endl;
    std::cout << "dddwgc::bits_v<E> = " << dddwgc::bits_v<E> << std::endl;
    std::cout << "dddwgc::bits_v<F> = " << dddwgc::bits_v<F> << std::endl;
    std::cout << "dddwgc::bits_v<PAD> = " << dddwgc::bits_v<PAD> << std::endl;

#if 0
    std::cout << "sizeof(TAB<A,B,C,D,E,PAD>) = " << sizeof(TAB<A,B,C,D,E,PAD>) << std::endl;

    {
        TAB<A,B,C,D,E,PAD> foo;
        std::cout << foo.to_string() << std::endl;
    }

    {
        TAB<A,B,C,D,E,PAD> foo(A::Yes);
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

    {
        TAB<A,B,C,D,E,PAD> foo(A::Yes);
#ifdef EXPLICIT_CAST
        A a = static_cast<A>(foo);
#else
        A a = foo;
#endif
        std::cout << "a = " << (a == A::Yes ? "Yes" : "No") << std::endl;
    }

    {
        TAB<A> foo(A::Yes);
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

#endif

    std::cout << "\n====== Test bitmask ======\n" << std::endl;

    {
        auto foo = dddwgc::bitmask_v<15, dddwgc::bits_v<D>, 6>;
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

    {
        auto foo = dddwgc::bitmask_v<15, dddwgc::bits_v<E>, 6>;
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

    {
        auto foo = dddwgc::bitmask_v<15>;
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

    {
        auto foo = dddwgc::bitmask_v<15, 6>;
        std::cout << foo.to_string('_', 'X') << std::endl;
    }

    std::cout << "\n====== Test XXX and YYY ======\n" << std::endl;

    std::cout << "sizeof(unsigned long long)*8 = " << sizeof(unsigned long long)*8 << std::endl;
//     std::cout << "sizeof(YYY<A>)*8 = " << sizeof(YYY<A>)*8 << std::endl;
//     std::cout << "sizeof(YYY<A,B>)*8 = " << sizeof(YYY<A,B>)*8 << std::endl;
//     std::cout << "sizeof(YYY<A,B,C>)*8 = " << sizeof(YYY<A,B,C>)*8 << std::endl;
//     std::cout << "sizeof(YYY<A,B,C,D>)*8 = " << sizeof(YYY<A,B,C,D>)*8 << std::endl;
//     std::cout << "sizeof(YYY<A,B,C,D,E>)*8 = " << sizeof(YYY<A,B,C,D,E>)*8 << std::endl;
//     std::cout << "sizeof(YYY<A,B,C,D,E,F>)*8 = " << sizeof(YYY<A,B,C,D,E,F>)*8 << std::endl;
    std::cout << "sizeof(YYY<A,B,C,D,E,F,PAD>)*8 = " << sizeof(YYY<A,B,C,D,E,F,PAD>)*8 << std::endl;
//     std::cout << "bitmask A:   " << XXX<dddwgc::bits_v<>,A,B,C,D,E,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask B:   " << XXX<dddwgc::bits_v<A>,B,C,D,E,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask C:   " << XXX<dddwgc::bits_v<A,B>,C,D,E,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask D:   " << XXX<dddwgc::bits_v<A,B,C>,D,E,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask E:   " << XXX<dddwgc::bits_v<A,B,C,D>,E,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask F:   " << XXX<dddwgc::bits_v<A,B,C,D,E>,F,PAD>::bitmask.to_string('_', 'X') << std::endl;
//     std::cout << "bitmask PAD: " << XXX<dddwgc::bits_v<A,B,C,D,E,F>,PAD>::bitmask.to_string('_', 'X') << std::endl;

    std::cout << "\n====== Test Flags ======\n" << std::endl;

    {
        Flags foo{A::Yes};
        print(foo);
    }

    {
        Flags foo{B::Yes};
        print(foo);
    }

    {
        Flags foo{C::Yes};
        print(foo);
    }

    {
        Flags foo{D::Opt11};
        print(foo);
    }

    {
        Flags foo{E::Opt11};
        print(foo);
    }

    {
        Flags foo{F::Opt111};
        print(foo);
    }
std::cout << "=============================================================================" << std::endl;

    {
        Flags f1{D::Opt11};
        Flags f2{E::Opt11};
        print(f1 | f2);
        print(f1);
        print(f2);
    }

std::cout << "=============================================================================" << std::endl;

    {
        Flags f1{D::Opt11};
        Flags f2{E::Opt11};
        f1 |= f2;
        print(f1);
        print(f2);
    }

std::cout << "=============================================================================" << std::endl;

    {
        Flags f1{D::Opt11};
        print(f1 | E::Opt11);
    }

std::cout << "=============================================================================" << std::endl;

    {
        Flags f1{D::Opt11};
        print(E::Opt11 | f1);
    }

std::cout << "=============================================================================" << std::endl;

    {
        Flags f1{D::Opt11};
        f1 |= E::Opt11;
        print(f1);
    }

    Flags x = A::Yes | B::Yes;

    A::Yes | B::Yes;
    B::Yes | A::Yes;
    B::Yes | Z::Yes;
    Z::Yes | B::Yes;

//     A::Yes | Z::Yes;
//     Z::Yes | A::Yes;
//     D::Opt11 | E::Opt11;
//     E::Opt11 | F::Opt111;
//     F::Opt111 | PAD{};
    {
        Flags foo = D::Opt11 | E::Opt11;
        print(foo);
    }

    {
        Flags foo{B::Yes | A::Yes | D::Opt10 | E::Opt01 | F::Opt101};
        print(foo);
    }
}
