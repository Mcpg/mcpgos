#pragma once

#include <McpgOS.h>

typedef struct VfsDirectory VfsDirectory;
typedef struct VfsInode VfsInode;
typedef struct FileHandle FileHandle;

typedef struct
{
    VfsDirectory* RootDirectory;
} VfsFsInstance;

typedef enum
{
    VFS_INODE_NORMAL, VFS_INODE_DIRECTORY,
    VFS_INODE_BLOCK
} VfsInodeType;

typedef struct
{
    
} VfsInodeOperations;

typedef struct
{
    VfsInodeOperations InodeOperations;
} VfsDirectoryOperations;

struct VfsInode
{
    char* Name;
    //uint32_t uid; // TODO: file permissions
    //uint32_t gid;

    VfsInodeType Type;
    VfsDirectory* Parent;
    VfsFsInstance* FsInstance;

    VfsInodeOperations* Operations;
};

struct VfsDirectory
{
    VfsInode Inode;

    uint32_t ChildrenAmount;
    VfsInode* Children;

    VfsDirectoryOperations* Operations;
};

struct FileHandle
{
    IoStream* Stream;
    VfsInode* Inode;
};

typedef VfsFsInstance* (*VfsFsInstCreateFunc)(IoStream*);
typedef struct
{
    char* Name;
    VfsFsInstCreateFunc CreateCallback;
} VfsFilesystemInfo;

void VfsInit();

void VfsRegisterFilesystem(VfsFilesystemInfo* info);
VfsFilesystemInfo* VfsGetFilesystem(char* fsName);
bool VfsIsFsRegistered(char* name);

VfsFsInstance* VfsCreateInstance(char* fsName, IoStream* stream);