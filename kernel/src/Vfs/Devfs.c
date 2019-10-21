#include <McpgOS.h>

static VfsFilesystemInfo DevfsInfo;



typedef struct
{
    LListElement Element;
    DevfsDevice* Device;
} DevfsDeviceEntry;

static DevfsDeviceEntry* DevfsDeviceList = NULL;

static VfsFsInstance* DevfsCreateInst(IoStream* device)
{
    UNIMPLEMENTED();
    return NULL;
}

void DevfsInit()
{
    DevfsInfo.Name = "devfs";
    DevfsInfo.CreateCallback = DevfsCreateInst;
    VfsRegisterFilesystem(&DevfsInfo);
}

void DevfsAddDevice(DevfsDevice* device)
{
    DevfsDeviceEntry* entry;

    KAssert(device != NULL);

    entry = tmalloc(DevfsDeviceEntry);
    KAssert(entry != NULL);

    entry->Element.Next = NULL;
    entry->Element.Prev = NULL;
    entry->Device = device;

    if (DevfsDeviceList == NULL)
        DevfsDeviceList = entry;
    else
        LListPushBackC(DevfsDeviceList, entry);

    Kprintf("[Devfs] Registered device /dev/%s\n", device->Name);
}
