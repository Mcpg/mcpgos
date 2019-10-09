#include <McpgOS.h>

unsigned int DmDriverCount;
DmDriverDefinition* DmDriverList[DM_MAX_REGISTERED_DRIVERS];

void DmInit()
{
    DmDriverCount = 0;
}

int DmRegisterDriver(DmDriverDefinition* def)
{
    KAssert(def != NULL);
    if (DmDriverCount == DM_MAX_REGISTERED_DRIVERS)
        return 0;
    DmDriverList[DmDriverCount++] = def;
    return 1;
}
