#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include "common.h"
#include "utils.h"

struct LoadImageData {
    PUNICODE_STRING FullImageName;
    HANDLE ProcessId;
    PIMAGE_INFO ImageInfo;
};

typedef struct LoadImageData LOAD_IMAGE_DATA, * PLOAD_IMAGE_DATA;

// Define the structure for image information
struct LoadImageInfo {
    IMAGE_INFO ImageInfo;           // Image information structure
    HANDLE PID;                     // Process ID
    ULONG FullImageNameSize;        // Size of the full image name
    ULONG FileNameSize;             // Size of the file name
    WCHAR FullImageName[MAX_PATH];       // Pointer to the full image name
    WCHAR FileName[MAX_PATH];                 // Pointer to the file name (if extended info is present)
    POOL_TYPE PoolType;             // Type of memory pool
};

typedef struct LoadImageInfo LOAD_IMAGE_INFO, * PLOAD_IMAGE_INFO;

struct LoadImageInfoStatic {
    IMAGE_INFO ImageInfo;           // Image information structure
    HANDLE PID;                     // Process ID
    ULONG FullImageNameSize;        // Size of the full image name
    ULONG FileNameSize;             // Size of the file name
    WCHAR FullImageName[MAX_PATH];       // Pointer to the full image name
    WCHAR FileName[MAX_PATH];                 // Pointer to the file name (if extended info is present)
};

typedef struct LoadImageInfoStatic LOAD_IMAGE_INFO_STATIC, * PLOAD_IMAGE_INFO_STATIC;

// Function prototypes
PLOAD_IMAGE_INFO LoadImageInfoAlloc(_In_ POOL_TYPE PoolType);

NTSTATUS GetExtendedInfo(
    _In_ PLOAD_IMAGE_INFO LoadImageInfo,
    _In_ PIMAGE_INFO ImageInfo
);

NTSTATUS InitLoadImageInfoFields(
    _In_ PLOAD_IMAGE_INFO LoadImageInfo,
    _In_ PIMAGE_INFO ImageInfo,
    _In_ PUNICODE_STRING FullImageName
);

NTSTATUS LoadImageInfoInit(
    _In_ PLOAD_IMAGE_INFO LoadImageInfo,
    _In_ PIMAGE_INFO ImageInfo,
    _In_ PUNICODE_STRING FullImageName
);

void LoadImageInfoDeInit(
    _In_ PLOAD_IMAGE_INFO LoadImageInfo
);

void LoadImageInfoFree(
    _Inout_ PLOAD_IMAGE_INFO* LoadImageInfo
);

PLOAD_IMAGE_INFO LoadImageInfoGet(
    _In_ POOL_TYPE PoolType,
    _In_ PLOAD_IMAGE_DATA LoadData
);

extern void LoadImageCopy(
    PLOAD_IMAGE_INFO_STATIC Dest,
    PLOAD_IMAGE_INFO Src
);


#endif // LOADIMAGE_H
