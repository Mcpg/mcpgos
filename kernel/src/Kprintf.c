#include <McpgOS.h>
#include <stdarg.h>

IoStream* KprintfStream = NULL;

void Kprintf(char* msg, ...)
{
    va_list vargs;

    if (KprintfStream == NULL)
        KprintfStream = TTYStream;

    va_start(vargs, msg);

    // TODO: implement formatting support
    IoWrite(KprintfStream, msg, strlen(msg));

    va_end(vargs);
}
