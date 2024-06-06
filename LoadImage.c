#include "LoadImage.h"

/**
 * LoadImageInfoAlloc - Allocate memory for a LOAD_IMAGE_INFO structure
 * @PoolType: The type of pool memory to allocate
 *
 * Return: A pointer to the allocated LOAD_IMAGE_INFO structure or NULL on failure
 */
PLOAD_IMAGE_INFO LoadImageInfoAlloc(_In_ POOL_TYPE PoolType)
{
    PLOAD_IMAGE_INFO LoadImageInfo;

    LoadImageInfo = ExAllocatePoolWithTag(PoolType, sizeof(*LoadImageInfo), 'load');
    if (!LoadImageInfo)
        return NULL;

    LoadImageInfo->PoolType = PoolType;

    return LoadImageInfo;
}

/**
 * GetExtendedInfo - Get the extended information from an IMAGE_INFO structure
 * @LoadImageInfo: Pointer to the LOAD_IMAGE_INFO structure
 * @ImageInfo: Pointer to the IMAGE_INFO structure
 *
 * Return: NTSTATUS code indicating success or failure
 */
NTSTATUS GetExtendedInfo(_In_ PLOAD_IMAGE_INFO LoadImageInfo, _In_ PIMAGE_INFO ImageInfo)
{
    NTSTATUS Status;
    PIMAGE_INFO_EX ImageInfoEx;
    PUNICODE_STRING FileName;

    ImageInfoEx = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);
    FileName = &ImageInfoEx->FileObject->FileName;

    LoadImageInfo->FileNameSize = MAX_PATH;
    Status = UnicodeStrToStaticWSTR(LoadImageInfo->FileName, FileName, &LoadImageInfo->FileNameSize);
    if (!NT_SUCCESS(Status))
        ExFreePoolWithTag(LoadImageInfo->FullImageName, 'wstr');

    return Status;
}

/**
 * InitLoadImageInfoFields - Initialize the fields of a LOAD_IMAGE_INFO structure
 * @LoadImageInfo: Pointer to the LOAD_IMAGE_INFO structure
 * @ImageInfo: Pointer to the IMAGE_INFO structure
 * @FullImageName: Pointer to the full image name
 *
 * Return: NTSTATUS code indicating success or failure
 */
NTSTATUS InitLoadImageInfoFields(_In_ PLOAD_IMAGE_INFO LoadImageInfo, _In_ PIMAGE_INFO ImageInfo, _In_ PUNICODE_STRING FullImageName)
{
    NTSTATUS Status;

    LoadImageInfo->FullImageNameSize = MAX_PATH;
    Status = UnicodeStrToStaticWSTR(LoadImageInfo->FullImageName, FullImageName, &LoadImageInfo->FullImageNameSize);
    if (!NT_SUCCESS(Status)) {
        ExFreePoolWithTag(LoadImageInfo->FullImageName, 'wstr');
        return Status;
    }

    if (!ImageInfo->ExtendedInfoPresent) {
        //LoadImageInfo->FileName = NULL;
        return Status;
    }

    Status = GetExtendedInfo(LoadImageInfo, ImageInfo);
    return Status;
}

/**
 * LoadImageInfoInit - Initialize a LOAD_IMAGE_INFO structure
 * @LoadImageInfo: Pointer to the LOAD_IMAGE_INFO structure
 * @ImageInfo: Pointer to the IMAGE_INFO structure
 * @FullImageName: Pointer to the full image name
 *
 * Return: NTSTATUS code indicating success or failure
 */
NTSTATUS LoadImageInfoInit(_In_ PLOAD_IMAGE_INFO LoadImageInfo, _In_ PIMAGE_INFO ImageInfo, _In_ PUNICODE_STRING FullImageName)
{
    NTSTATUS Status;

    Status = InitLoadImageInfoFields(LoadImageInfo, ImageInfo, FullImageName);
    if (!NT_SUCCESS(Status))
        return Status;

    RtlCopyMemory(&LoadImageInfo->ImageInfo, ImageInfo, sizeof(*ImageInfo));

    return Status;
}

/**
 * LoadImageInfoDeInit - Deinitialize a LOAD_IMAGE_INFO structure
 * @LoadImageInfo: Pointer to the LOAD_IMAGE_INFO structure
 */
void LoadImageInfoDeInit(_In_ PLOAD_IMAGE_INFO LoadImageInfo)
{
    if (LoadImageInfo) {
        if (LoadImageInfo->FullImageName)
            ExFreePoolWithTag(LoadImageInfo->FullImageName, 'wstr');
        if (LoadImageInfo->FileName)
            ExFreePoolWithTag(LoadImageInfo->FileName, 'wstr');
    }
}

/**
 * LoadImageInfoFree - Free a LOAD_IMAGE_INFO structure
 * @LoadImageInfo: Pointer to the pointer of the LOAD_IMAGE_INFO structure
 */
void LoadImageInfoFree(_Inout_ PLOAD_IMAGE_INFO* LoadImageInfo)
{
    LoadImageInfoDeInit(*LoadImageInfo);
    ExFreePoolWithTag(*LoadImageInfo, 'load');
    *LoadImageInfo = NULL;
}

/**
 * LoadImageInfoGet - Allocate and initialize a LOAD_IMAGE_INFO structure
 * @PoolType: The type of pool memory to allocate
 * @ImageInfo: Pointer to the IMAGE_INFO structure
 * @FullImageName: Pointer to the full image name
 * @PID: Process ID
 *
 * Return: A pointer to the allocated and initialized LOAD_IMAGE_INFO structure or NULL on failure
 */
PLOAD_IMAGE_INFO LoadImageInfoGet(_In_ POOL_TYPE PoolType, PLOAD_IMAGE_DATA LoadData)
{
    NTSTATUS Status;
    PLOAD_IMAGE_INFO LoadImageInfo;

    LoadImageInfo = LoadImageInfoAlloc(PoolType);
    if (!LoadImageInfo)
        return NULL;

    LoadImageInfo->PID = LoadData->ProcessId;

    Status = LoadImageInfoInit(LoadImageInfo, LoadData->ImageInfo, LoadData->FullImageName);
    if (!NT_SUCCESS(Status))
        LoadImageInfoFree(&LoadImageInfo);

    return LoadImageInfo;
}

static inline void ImageInfoCopy(PIMAGE_INFO Dest, PIMAGE_INFO Src) {
    RtlCopyMemory(Dest, Src, sizeof * Dest);
}

void LoadImageCopy(PLOAD_IMAGE_INFO_STATIC Dest, PLOAD_IMAGE_INFO Src) {
    ImageInfoCopy(&Dest->ImageInfo, &Src->ImageInfo);

    Dest->PID = Src->PID;
    Dest->FullImageNameSize = Src->FullImageNameSize;
    Dest->FileNameSize = Src->FileNameSize;

    RtlCopyMemory(Dest->FullImageName, Src->FullImageName, sizeof Dest->FullImageName );
    RtlCopyMemory(Dest->FileName, Src->FileName, sizeof Dest->FileName);
}
