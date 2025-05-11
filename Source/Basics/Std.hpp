#ifndef STD_H
#define STD_H

#include <windows.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef float f32;
typedef double f64;

typedef size_t usz;
typedef void *any;
typedef const void *const_any;

typedef uint32_t rgba;

struct rect
{
    f32 X, Y, W, H;
};

namespace Mem
{

constexpr static inline void Set(any dest, u8 c, usz n)
{
    for (usz i = 0; i < n; i++)
    {
        ((u8*)dest)[i] = c;
    }
}

constexpr static inline void Copy(any dest, const_any src, usz n)
{
    for (usz i = 0; i < n; i++)
    {
        ((u8*)dest)[i] = ((u8*)src)[i];
    }
}

constexpr static inline void Zero(any dest, usz n)
{
    Set(dest, 0, n);
}

static inline void *Alloc(usz size)
{
    return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

static inline void Free(void *ptr)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}

}

#endif