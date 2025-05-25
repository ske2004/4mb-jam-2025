#include "Std.hpp"

#pragma function(memset)
void *__cdecl memset(void *dest, int c, size_t n)
{
    Mem::Set(dest, (u8)c, n);
    return dest;
}

#pragma function(memcpy)
void *__cdecl memcpy(void *dest, const void *src, size_t n)
{
    Mem::Copy(dest, (void*)src, n);
    return dest;
}
