#include <McpgOS.h>

static uint16_t* TextBuffer = (uint16_t*) 0xB8000;

const uint8_t TTYWidth = 80;
const uint8_t TTYHeight = 25;
struct TTYState TTYState;
IOStream _TTYStream;
IOStream* TTYStream = &_TTYStream;
static DmDriverDefinition DriverDefinition;

static int TTYIOWrite(IOStream* s, void* buf, uint32_t len)
{
    TTYWrite((uint8_t*) buf, len);
    return len;
}

static int TTYIORead(IOStream* s, void* buf, uint32_t len)
{
    return 0; // Read isn't supported by the TTY driver
}

static int TTYIOCanRead(IOStream* s)
{
    return 0;
}

static int TTYIOCanWrite(IOStream* s)
{
    return 1;
}

static int TTYIOClose(IOStream* s)
{
    return 0;
}

void TTYInit()
{
    uint16_t cursorPos = 0;

    TTYSetCursorVisible(1);

    Outb(0x3D4, 0x0F);
    cursorPos |= Inb(0x3D5);
    Outb(0x3D4, 0x0E);
    cursorPos |= ((uint16_t) Inb(0x3D5)) << 8;

    TTYState.CursorX = cursorPos % TTYWidth;
    TTYState.CursorY = cursorPos / TTYWidth;
    TTYState.Attribute = 0x07;

    _TTYStream.WriteCallback = TTYIOWrite;
    _TTYStream.ReadCallback = TTYIORead;
    _TTYStream.CanReadCallback = TTYIOCanRead;
    _TTYStream.CanWriteCallback = TTYIOCanWrite;
    _TTYStream.CloseCallback = TTYIOClose;

    DriverDefinition.DriverID = "TTY";
    DmRegisterDriver(&DriverDefinition);
}

void TTYScroll()
{
    int i;

    for (int i = 1; i < TTYHeight; i++)
    {
        memcpy(
            TextBuffer + ((i - 1) * TTYWidth),
            TextBuffer + (i * TTYWidth),
            TTYWidth
        );
    }

    if (TTYState.CursorY > 0)
        TTYState.CursorY--;
}

void TTYMoveCursor(uint8_t x, uint8_t y)
{
    uint16_t pos = y * TTYWidth + x;
 
    Outb(0x3D4, 0x0F);
    Outb(0x3D5, (uint8_t) (pos & 0xFF));
    Outb(0x3D4, 0x0E);
    Outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));

    TTYState.CursorX = x;
    TTYState.CursorY = y;
}

void TTYSetCursorVisible(int visible)
{
    if (visible)
    {
        Outb(0x3D4, 0x0A);
        Outb(0x3D5, (Inb(0x3D5) & 0xC0) | 12);
        Outb(0x3D4, 0x0B);
        Outb(0x3D5, (Inb(0x3D5) & 0xE0) | 13);
    }
    else
    {
        Outb(0x3D4, 0x0A);
        Outb(0x3D5, 0x20);
    }
}

void TTYWrite(uint8_t* buf, uint32_t len)
{
    // TODO: ANSI escape sequences

    uint32_t i;
    uint8_t cursorX = TTYState.CursorX;
    uint8_t cursorY = TTYState.CursorY;

    for (i = 0; i < len; i++)
    {
        switch (buf[i])
        {
        case '\b':
            if (cursorX > 0)
                cursorX--;
            break;
        case '\r':
            cursorX = 0;
            break;
        case '\n':
            cursorX = 0;
            if (cursorY == TTYWidth - 1)
                TTYScroll();
            else
                cursorY++;
            break;
        default:
            TextBuffer[cursorY * TTYWidth + cursorX] =
                (TTYState.Attribute << 8) | buf[i];
            if (++cursorX == TTYWidth)
            {
                cursorX = 0;
                if (++cursorY == TTYHeight)
                {
                    cursorY--;
                    TTYScroll();
                }
            }
            break;
        }
    }
    TTYMoveCursor(cursorX, cursorY);
}
