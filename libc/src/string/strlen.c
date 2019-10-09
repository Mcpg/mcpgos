#include <string.h>

size_t strlen(char* str)
{
    size_t result = 0;
    for (; *str; str++) result++;
    return result;
}
