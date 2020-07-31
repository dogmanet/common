#ifndef __ENUM_FLAGS_H
#define __ENUM_FLAGS_H

#include <cstdint>

#ifdef __cplusplus

// Define operator overloads to enable bit operations on enum values that are 
// used to define flags. Use XDEFINE_ENUM_FLAG_OPERATORS(YOUR_TYPE) to enable these 
// operators on YOUR_TYPE.

// Templates are defined here in order to avoid a dependency on C++ <type_traits> header file,
// or on compiler-specific contructs.
extern "C++" {

    template <size_t S>
    struct _XENUM_FLAG_INTEGER_FOR_SIZE;

    template <>
    struct _XENUM_FLAG_INTEGER_FOR_SIZE<1>
    {
        typedef int8_t type;
    };

    template <>
    struct _XENUM_FLAG_INTEGER_FOR_SIZE<2>
    {
        typedef int16_t type;
    };

    template <>
    struct _XENUM_FLAG_INTEGER_FOR_SIZE<4>
    {
        typedef int32_t type;
    };

    // used as an approximation of std::underlying_type<T>
    template <class T>
    struct _XENUM_FLAG_SIZED_INTEGER
    {
        typedef typename _XENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
    };

}

#define XDEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_XENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
}
#else
#define XDEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

#endif
