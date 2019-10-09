#pragma once

#include <McpgOS.h>

struct TTYState
{
    uint8_t CursorX;
    uint8_t CursorY;
    uint8_t Attribute;
    uint8_t CursorVisible;
};

extern const uint8_t TTYWidth;
extern const uint8_t TTYHeight;
extern struct TTYState TTYState;
extern IOStream* TTYStream;

void TTYInit();
void TTYScroll();
void TTYMoveCursor(uint8_t x, uint8_t y);
void TTYSetCursorVisible(int visible);
void TTYWrite(uint8_t* buf, uint32_t len);
