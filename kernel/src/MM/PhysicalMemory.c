#include <McpgOS.h>

#define MM_BITMAP_SIZE 1048576

static uint8_t MmPmBitmap[MM_BITMAP_SIZE / 4];
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
    // TODO: parsing the physical memory map (provided by the bootloader
    //       via some tag or whatever)
    memset(MmPmBitmap, MmPmPageFree, sizeof(MmPmBitmap));
    memset(MmPmBitmap, MmPmPageUnusable, 64);
    memset(&MmPmBitmap[64], MmPmPageUsed, KernelSizePages);

    MmPmBitmapTop = 64 + KernelSizePages;
}

uint32_t MmPmAllocate()
{
    uint32_t i;
    bool foundPage = false;

    for (i = MmPmBitmapTop; i < MM_BITMAP_SIZE; i++)
    {
        if (MmPmGetBitmapValue(i) == MmPmPageFree)
        {
            foundPage = true;
            MmPmBitmapTop = i + 1;
            break; 
        }
    }

    if (!foundPage)
        return 0;

    MmPmSetBitmapValue(i, MmPmPageUsed);

    return i << 12;
}

void MmPmFree(uint32_t phys)
{
    uint32_t pageIndex = phys >> 10;
    
    if (MmPmGetBitmapValue(pageIndex) != MmPmPageUsed)
        return;

    MmPmSetBitmapValue(pageIndex, MmPmPageFree);

    if (MmPmBitmapTop > pageIndex)
        MmPmBitmapTop = pageIndex;
}
