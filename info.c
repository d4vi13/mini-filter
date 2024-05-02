
#include "info.h"
#include "fileinfo.h"

/*
 *  InfoAlloc() - Allocate a 'INFO' structure.
 *
 *  @PoolType: The type of memory pool to allocate from (paged or nonpaged).
 *
 *  Return:
 *    - Pointer to the allocated 'INFO' structure on success.
 *    - 'NULL' if memory allocation fails.
 */
PINFO InfoAlloc(_In_ POOL_TYPE PoolType) {

    PINFO Info;

    Info = ExAllocatePoolWithTag(PoolType, sizeof * Info, 'info');
    if (!Info) {
        return NULL;
    }

    RtlZeroMemory(Info, sizeof * Info);

    /*
     *  Store the pool type used for the allocation in 'Info' to ensure
     *  correct memory handling, regardless of whether the caller routine is
     *  paged or nonpaged.
     */
    Info->PoolType = PoolType;

    return Info;
}

/*
 *  InfoGet() -
 *
 *  @PoolType:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
PINFO InfoGet(_In_ POOL_TYPE PoolType, _In_ ULONG NameQueryMethod, _In_ PFLT_CALLBACK_DATA Data) {

    PINFO Info;
    NTSTATUS Status;

    Info = InfoAlloc(PoolType);
    if (!Info) {
        return NULL;
    }

    Status = InfoInit(Info,NameQueryMethod, Data);
    if (!NT_SUCCESS(Status)) {
        InfoFree(&Info);
    }

    return Info;
}

/*
 *  InitTimeFields() -
 *
 *  @TimeFields:
 *
 *  Return:
 *    -
 */
static inline void InitTimeFields(_Out_ PTIME_FIELDS TimeFields) {

    LARGE_INTEGER SysTime;
    LARGE_INTEGER LocalTime;

    KeQuerySystemTime(&SysTime);
    ExSystemTimeToLocalTime(&SysTime, &LocalTime);
    RtlTimeToTimeFields(&LocalTime, TimeFields);
}

/*
 *  InfoInit() -
 *
 *  @Info:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
NTSTATUS InfoInit(_Out_ PINFO Info,_In_ ULONG NameQueryMethod,_In_ PFLT_CALLBACK_DATA Data) {

    NTSTATUS Status;

 
    Status = MjFuncInit(&Info->MjFunc, Data->Iopb->MajorFunction);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }
  

    Status = ProcInfoInit(&Info->ProcInfo, Data);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Info->Info.FileInfo = FileInfoGet(Info->PoolType, NameQueryMethod, Data);
    if (!NT_SUCCESS(Status))
        return Status;

    InitTimeFields(&Info->TimeFields);

    return Status;
}


/*
 *  InfoDeInit() -
 *
 *  @Info:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
void InfoDeInit(_Inout_ PINFO Info) {
    FileInfoFree(&(Info->Info.FileInfo));
    
    return;
}

/*
 *  InfoFree() -
 *
 *  @Info:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
void InfoFree(_Inout_ PINFO* Info) {
    InfoDeInit(*Info);

    ExFreePoolWithTag(*Info, 'info');

    *Info = NULL;

    return;
}