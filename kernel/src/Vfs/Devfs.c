#include <McpgOS.h>

static VfsFilesystemInfo DevfsInfo;

typedef struct
{
    LListElement Element;
    DevfsDevice* Device;
} DevfsDeviceEntry;

typedef struct
{
    VfsInode Base;
    DevfsDevice* Device;
} DevfsInode;

static DevfsDeviceEntry* DevfsDeviceList = NULL;
static VfsFsInstance DevfsInstance;
static VfsDirectory DevfsRootDir;
static VfsDirectoryOperations DevfsRootDirOps;

static VfsFsInstance* DevfsCreateInst(IoStream* device)
{
    return &DevfsInstance;
}

void DevfsInit()
{
    DevfsInstance.RootDirectory = &DevfsRootDir;
    DevfsRootDir.Inode.Name = "";
    DevfsRootDir.Inode.Uid = 0;
    DevfsRootDir.Inode.Gid = 0;
    DevfsRootDir.Inode.Type = VFS_INODE_DIRECTORY;
    DevfsRootDir.Inode.Parent = NULL;
    DevfsRootDir.Inode.FsInstance = &DevfsInstance;
    DevfsRootDir.Inode.Operations
        = (VfsInodeOperations*) &DevfsRootDirOps;
    DevfsRootDir.ChildrenAmount = 0;
    DevfsRootDir.Children = NULL;
    DevfsRootDir.Operations = &DevfsRootDirOps;

    DevfsInfo.Name = "devfs";
    DevfsInfo.CreateCallback = DevfsCreateInst;
    VfsRegisterFilesystem(&DevfsInfo);
}

void DevfsAddDevice(DevfsDevice* device)
{
    DevfsDeviceEntry* entry;
    DevfsInode* deviceInode;
    VfsInode** reallocated;

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

    reallocated = tnrealloc(
        DevfsRootDir.Children,
        VfsInode*,
        ++DevfsRootDir.ChildrenAmount
    );
    KAssert(reallocated != NULL);
    DevfsRootDir.Children = reallocated;
    
    deviceInode = tmalloc(DevfsInode);
    KAssert(deviceInode != NULL);

    deviceInode->Device = device;
    deviceInode->Base.Name = device->Name;
    deviceInode->Base.Uid = 0;
    deviceInode->Base.Gid = 0;
    deviceInode->Base.Type = VFS_INODE_BLOCK;
    deviceInode->Base.Parent = &DevfsRootDir;
    deviceInode->Base.FsInstance = &DevfsInstance;

    // TODO: Fill device inode operations with proper data in DevfsAddDevice
    deviceInode->Base.Operations = NULL;

    DevfsRootDir.Children[DevfsRootDir.ChildrenAmount - 1]
        = (VfsInode*) deviceInode;

    Kprintf("[Devfs] Registered device /dev/%s\n", device->Name);
}
