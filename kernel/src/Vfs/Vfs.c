#include <McpgOS.h>

typedef struct
{
    LListElement Element;
    VfsFilesystemInfo* Filesystem;
} VfsFilesystemEntry;
static VfsFilesystemEntry* VfsFsList = NULL;

void VfsInit()
{

}

void VfsRegisterFilesystem(VfsFilesystemInfo* info)
{
    VfsFilesystemEntry* entry;

    KAssert(info != NULL);
    KAssert(!VfsIsFsRegistered(info->Name));

    entry = tmalloc(VfsFilesystemEntry);
    KAssert(entry != NULL);

    entry->Element.Next = NULL;
    entry->Element.Prev = NULL;
    entry->Filesystem = info;

    if (VfsFsList == NULL)
        VfsFsList = entry;
    else
        LListPushBackC(VfsFsList, entry);

    Kprintf("[Vfs] Registered filesystem `%s`\n", info->Name);
}

VfsFilesystemInfo* VfsGetFilesystem(char* fsName)
{
    VfsFilesystemEntry* entry = VfsFsList;

    if (VfsFsList == NULL)
        return NULL;

    while (strcmp(entry->Filesystem->Name, fsName) != 0)
    {
        entry = (VfsFilesystemEntry*) entry->Element.Next;
        if (entry == NULL)
            return NULL;
    }
    return entry->Filesystem;
}

bool VfsIsFsRegistered(char* name)
{
    return VfsGetFilesystem(name) != NULL;
}

VfsFsInstance* VfsCreateInstance(char* fsName, IoStream* stream)
{
    KAssert(fsName != NULL);

    VfsFilesystemInfo* info = VfsGetFilesystem(fsName);
    KAssert(info != NULL);
    return info->CreateCallback(stream);
}
