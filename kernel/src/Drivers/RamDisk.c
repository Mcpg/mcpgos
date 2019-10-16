#include <McpgOS.h>

static DmDriverDefinition RamDiskDriverDef;

RamDiskEntry RamDiskEntries[RAMDISK_MAX_DISKS];
uint32_t RamDiskCount = 0;

static int RamDiskWrite(RamDiskStream* s, void* b, uint32_t size)
{
    if ((s->StreamCursor + size) >= s->ParentEntry->BufferSize)
        size = s->ParentEntry->BufferSize - s->StreamCursor;

    memcpy(b, s->ParentEntry->Buffer + s->StreamCursor, size);
    s->StreamCursor += size;
    return size;
}

static int RamDiskRead(RamDiskStream* s, void* b, uint32_t size)
{
    if ((s->StreamCursor + size) >= s->ParentEntry->BufferSize)
        size = s->ParentEntry->BufferSize - s->StreamCursor;

    memcpy(s->ParentEntry->Buffer + s->StreamCursor, b, size);
    s->StreamCursor += size;
    return size;
}

static int RamDiskCanRead(RamDiskStream* s)
{
    return true;
}

static int RamDiskCanWrite(RamDiskStream* s)
{
    return true;
}

static int RamDiskClose(RamDiskStream* s)
{
    return false;
}

static int RamDiskSeek(RamDiskStream* s, uint64_t pos, int mode)
{
    switch (mode)
    {
    case SEEK_SET:
        if (pos >= s->ParentEntry->BufferSize)
            return false;
        s->StreamCursor = pos;
        return true;
    case SEEK_CUR:
        if ((s->StreamCursor + pos) >= s->ParentEntry->BufferSize)
            return false;
        s->StreamCursor += pos;
        return true;
    case SEEK_END:
        if (pos >= s->ParentEntry->BufferSize)
            return false;
        s->StreamCursor = s->ParentEntry->BufferSize - pos;
        return true;
    default:
        return false;
    }
}

static int RamDiskTell(RamDiskStream* s)
{
    return s->StreamCursor;
}

void RamDiskInit()
{
    RamDiskDriverDef.DriverID = "RamDisk";
    DmRegisterDriver(&RamDiskDriverDef);
}

int RamDiskAddDisk(void* buffer, size_t bufferSize)
{
    uint32_t ramDiskID = ++RamDiskCount;
    RamDiskStream* stream;

    if (RamDiskCount == RAMDISK_MAX_DISKS)
        return 0;

    stream = (RamDiskStream*) malloc(sizeof(RamDiskStream));
    if (!stream)
        return 0;

    stream->StreamCursor = 0;
    stream->ParentEntry = &RamDiskEntries[ramDiskID];

    IoInsertStreamCallbacks(
        &stream->Base,

        RamDiskWrite,
        RamDiskRead,
        RamDiskCanRead,
        RamDiskCanWrite,
        RamDiskClose,
        RamDiskSeek,
        RamDiskTell
    );

    RamDiskEntries[ramDiskID].ID = ramDiskID;
    RamDiskEntries[ramDiskID].Stream = stream;
    RamDiskEntries[ramDiskID].Buffer = buffer;
    RamDiskEntries[ramDiskID].BufferSize = bufferSize;

    return ramDiskID;
}
