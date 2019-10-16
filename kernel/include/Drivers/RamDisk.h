#pragma once

typedef struct RamDiskEntry RamDiskEntry;

typedef struct
{
    IoStream Base;
    uint64_t StreamCursor;
    RamDiskEntry* ParentEntry;
} RamDiskStream;

struct RamDiskEntry
{
    uint32_t ID;

    uint8_t* Buffer;
    size_t BufferSize;

    RamDiskStream* Stream;
};

#define RAMDISK_MAX_DISKS 64    
extern RamDiskEntry RamDiskEntries[RAMDISK_MAX_DISKS];
extern uint32_t RamDiskCount;

void RamDiskInit();
int RamDiskAddDisk(void* buffer, size_t bufferSize);
