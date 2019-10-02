#include <McpgOS.h>

static uint8_t MmPmBitmap[262144];
static uint32_t MmPmBitmapTop = 0;

static const uint8_t MmPmPageFree = 0;
static const uint8_t MmPmPageUsed = 1;
static const uint8_t MmPmPageUnusable = 2;

static inline void MmPmSetBitmapValue(uint32_t index, uint8_t value)
{
    MmPmBitmap[index >> 2] &= 3 << ((index & 3) * 2);
    if (value > 0)
        MmPmBitmap[index >> 2] |= value << ((index & 3) * 2); 
}

static inline uint8_t MmPmGetBitmapValue(uint32_t index)
{
    return (MmPmBitmap[index >> 2] >> ((index & 3) * 2)) & 3;
}

void MmPmInitAllocator()
{
    memset(MmPmBitmap, MmPmPageFree, sizeof(MmPmBitmap));
    memset(MmPmBitmap, MmPmPageUnusable, 64);
    memset(&(MmPmBitmap + 64), MmPmPageUsed, KernelSizePages);

    MmPmBitmapTop = 64 + KernelSizePages;
}

uint32_t MmPmAllocate()
{
    int i;
    bool foundPage = false;

    for (i = MmPmBitmapTop; i < 1048576; i++)
    {
        
    }

    if (!foundPage)
        return 0;

    return 0;
}

void MmPmFree(uint32_t phys)
{

}
