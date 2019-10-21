#pragma once

#include <stdint.h>

typedef enum IpOrigin
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
} IoOrigin;

typedef struct IoStream IoStream;
struct IoStream
{
    int (*WriteCallback)(IoStream*, void*, uint32_t);
    int (*ReadCallback)(IoStream*, void*, uint32_t);
    int (*CanReadCallback)(IoStream*);
    int (*CanWriteCallback)(IoStream*);
    int (*CloseCallback)(IoStream*);
    
    int (*SeekCallback)(IoStream*, uint64_t, int);
    int (*TellCallback)(IoStream*);
};

static inline void IoInsertStreamCallbacks(
    IoStream* target,

    void* writeCallback,
    void* readCallback,
    void* canReadCallback,
    void* canWriteCallback,
    void* closeCallback,
    void* seekCallback,
    void* tellCallback
)
{
    target->WriteCallback = writeCallback;
    target->ReadCallback = readCallback;
    target->CanReadCallback = canReadCallback;
    target->CanWriteCallback = canWriteCallback;
    target->CloseCallback = closeCallback;
    target->SeekCallback = seekCallback;
    target->TellCallback = tellCallback;
}

static inline int IoWrite(IoStream* stream, void* buf, uint32_t size)
{
    if (stream->WriteCallback)
        return stream->WriteCallback(stream, buf, size);
    else
        return 0;
}

static inline int IoWriteByte(IoStream* stream, uint8_t n)
{
    return IoWrite(stream, &n, 1);
}

static inline int IoRead(IoStream* stream, void* buf, uint32_t size)
{
    if (stream->ReadCallback)
        return stream->ReadCallback(stream, buf, size);
    else
        return 0;
}

static inline int IoCanRead(IoStream* stream)
{
    if (stream->CanReadCallback)
        return stream->CanReadCallback(stream);
    else
        return 0;
}

static inline int IoCanWrite(IoStream* stream)
{
    if (stream->CanWriteCallback)
        return stream->CanWriteCallback(stream);
    else
        return 0;
}

static inline int IoClose(IoStream* stream)
{
    if (stream->CloseCallback)
        return stream->CloseCallback(stream);
    else
        return 0;
}

static inline int IoSeek(IoStream* stream, uint64_t pos, int origin)
{
    if (stream->SeekCallback)
        return stream->SeekCallback(stream, pos, origin);
    else
        return 0;
}

static inline int IoTell(IoStream* stream)
{
    if (stream->TellCallback)
        return stream->TellCallback(stream);
    else
        return 0;
}
