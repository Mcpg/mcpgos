#include <string.h>
#include <stdint.h>

void* memset(void* ptr, int val, size_t n)
{
    for (; n > 0; n--)
    {
        ((uint8_t*) ptr)[n - 1] = val;
    }
    return ptr;
}
