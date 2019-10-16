#include <McpgOS.h>

void KPanic(char* reason)
{
    Kprintf("\n\n");
    Kprintf("-----------------------------\n");
    Kprintf(" Kernel panic!               \n");
    Kprintf("-----------------------------\n");
    Kprintf(" ");
    Kprintf(reason);
    Kprintf("\n\n");
    Kprintf(" McpgOS kernel will now halt \n");
    Kprintf("-----------------------------\n");

    while (true)
    {
        CLI();
        HALT();
    }
}
