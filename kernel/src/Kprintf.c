#include <McpgOS.h>
#include <stdarg.h>

IoStream* KprintfStream = NULL;

//
// Limited formatting support in Kprintf
//  %d - 32-bit decimal
//  %x - 32-bit hex
//  %s - string (char*)
//  more to be added as needed, soon:tm:
//

void Kprintf(char* msg, ...)
{
    // TODO: cleaner implementation of Kprintf

    static const char* digits = "0123456789ABCDEF";

    va_list vargs;
    int i;
    int ii;
    
    char* providedString;
    uint32_t providedNumber;
    char numberBuffer[11] = {0};

    if (KprintfStream == NULL)
        KprintfStream = TTYStream;

    va_start(vargs, msg);
    for (i = 0; msg[i]; i++)
    {
        if (msg[i] == '%')
        {
            if (msg[i + 1] == 0)
            {
                IoWriteByte(KprintfStream, '%');
                break;
            }

            i++;

            switch (msg[i])
            {
                case 'd':
                case 'i':
                    providedNumber = va_arg(vargs, uint32_t);

                    numberBuffer[10] = 0;
                    for (ii = 0; ii < 10; ii++)
                    {
                        numberBuffer[9 - ii] = digits[providedNumber % 10];
                        providedNumber /= 10;
                    }

                    IoWrite(KprintfStream, numberBuffer, 10);
                    break;
                case 'x':
                    providedNumber = va_arg(vargs, uint32_t);
                    for (ii = 0; ii < 8; ii++)
                    {
                        numberBuffer[7 - ii] = digits[providedNumber & 0xF];
                        providedNumber >>= 4;
                    }
                    IoWrite(KprintfStream, numberBuffer, 8);
                    break;
                case 's':
                    providedString = va_arg(vargs, char*);
                    KAssert(providedString != NULL);
                    IoWrite(KprintfStream, providedString, strlen(providedString));
                    break;
                default:
                    IoWriteByte(KprintfStream, '%');
                    IoWriteByte(KprintfStream, msg[i]);
                    break;
            }
        }
        else
        {
            IoWriteByte(KprintfStream, msg[i]);
        }
    }

    va_end(vargs);
}
