#pragma once

#include <stdint.h>

typedef struct IOStream IOStream;
struct IOStream
{
    int (*WriteCallback)(IOStream*, void*, uint32_t);
    int (*ReadCallback)(IOStream*, void*, uint32_t);
    int (*CanReadCallback)(IOStream*);
    int (*CanWriteCallback)(IOStream*);
    int (*CloseCallback)(IOStream*);
};

static inline int IOWrite(IOStream* stream, void* buf, uint32_t size)
{
    return stream->WriteCallback(stream, buf, size);
}

static inline int IORead(IOStream* stream, void* buf, uint32_t size)
{
    return stream->ReadCallback(stream, buf, size);
}

static inline int IOCanRead(IOStream* stream)
{
    return stream->CanReadCallback(stream);
}

static inline int IOCanWrite(IOStream* stream)
{
    return stream->CanWriteCallback(stream);
}

static inline int IOClose(IOStream* stream)
{
    return stream->CloseCallback(stream);
}
