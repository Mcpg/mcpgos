#include <McpgOS.h>

static uint16_t* TextBuffer = (uint16_t*) 0xB8000;

const uint8_t TTYWidth = 80;
const uint8_t TTYHeight = 25;
struct TTYState TTYState;
IoStream _TTYStream;
IoStream* TTYStream = &_TTYStream;
static DmDriverDefinition DriverDefinition;

static int TTYIOWrite(IoStream* s, void* buf, uint32_t len)
{
    TTYWrite((uint8_t*) buf, len);
    return len;
}

static int TTYIOCanWrite(IoStream* s)
{
    return 1;
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

    IoInsertStreamCallbacks(
        TTYStream,

        TTYIOWrite,
        NULL,
        NULL,
        TTYIOCanWrite,
        NULL,
        NULL,
        NULL
    );

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
            TTYWidth * 2
        );
    }

    if (TTYState.CursorY > 0)
        TTYState.CursorY--;
}

void TTYNewLine()
{
    TTYState.CursorX = 0;
    if (TTYState.CursorY == (TTYHeight - 1))
    {
        TTYScroll();
    }
    else
    {
        TTYState.CursorY++;
    }
}

void TTYUpdateVGACursorPosition()
{
    TTYVGAMoveCursor(TTYState.CursorX, TTYState.CursorY);
}

void TTYVGAMoveCursor(uint8_t x, uint8_t y)
{
    uint16_t pos = y * TTYWidth + x;
 
    Outb(0x3D4, 0x0F);
    Outb(0x3D5, (uint8_t) (pos & 0xFF));
    Outb(0x3D4, 0x0E);
    Outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
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

    for (i = 0; i < len; i++)
    {
        switch (buf[i])
        {
        case '\b':
            if (TTYState.CursorX > 0)
                TTYState.CursorX--;
            break;
        case '\r':
            TTYState.CursorX = 0;
            break;
        case '\n':
            TTYState.CursorX = 0;
            if (TTYState.CursorY == TTYWidth - 1)
                TTYScroll();
            else
                TTYState.CursorY++;
            break;
        default:
            TextBuffer[TTYState.CursorY * TTYWidth + TTYState.CursorX] =
                (TTYState.Attribute << 8) | buf[i];
            if (++TTYState.CursorX == TTYWidth)
            {
                TTYNewLine();
            }
            break;
        }
    }
    
    TTYUpdateVGACursorPosition();   
}
