#pragma once

#define DM_MAX_REGISTERED_DRIVERS 32

typedef struct
{
    char* DriverID;
} DmDriverDefinition;

extern uint32_t DmDriverCount;
extern DmDriverDefinition* DmDriverList[DM_MAX_REGISTERED_DRIVERS];

void DmInit();
int DmRegisterDriver(DmDriverDefinition*);
