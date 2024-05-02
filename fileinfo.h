#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#endif

#ifndef FILE_INFO_H
#define FILE_INFO_H

extern NTSTATUS
UnicodeStrToWSTR(
    _In_ POOL_TYPE PoolType,
    _In_ PUNICODE_STRING Src,
    _Out_ PWSTR* Dest,
    _Out_ PULONG pSize
);


struct FileInfo {

    POOL_TYPE PoolType;

    ULONG PathSize;
    ULONG FinalNameSize;

    PWSTR Path;
    PWSTR FinalName;
};

typedef struct FileInfo FILE_INFO, * PFILE_INFO;

/*
 *  FileInfoAlloc() - Allocate a 'FILE_INFO' structure.
 *
 *  @PoolType: The type of memory pool to allocate from (paged or nonpaged).
 *
 *  Return:
 *    - Pointer to the allocated 'FILE_INFO' structure on success.
 *    - 'NULL' if memory allocation fails.
 */
extern PFILE_INFO
FileInfoAlloc(
    __drv_strictTypeMatch(__drv_typeExpr)POOL_TYPE PoolType
);

/*
 *  FileInfoGet() - Allocates and initializes a 'FILE_INFO' structure.
 *
 *  @PoolType: The type of memory pool to allocate from (paged or nonpaged).
 *  @NameQueryMethod:
 *  @Data: Pointer to a FLT_CALLBACK_DATA object containing file-related data.
 *
 *  'GetFileInfo()' is a wrapper for 'FileInfoAlloc()' and 'FileInfoInit()',
 *  providing a simple interface to obtain an allocated and initialized
 *  'FILE_INFO' structure.
 *
 *  Returns:
 *    - Pointer to the allocated 'FILE_INFO' structure on success.
 *    - 'NULL' if memory allocation or initialization fails.
 */
extern PFILE_INFO
FileInfoGet(
    __drv_strictTypeMatch(__drv_typeExpr)POOL_TYPE PoolType,
    _In_ ULONG NameQueryMethod,
    _In_ PFLT_CALLBACK_DATA Data
);

/*
 *  FileInfoInit() - Initializes a 'FILE_INFO' structure allocate by 'FileInfoAlloc()'
 *                   based on 'FLT_CALLBACK_DATA'.
 *
 *  @Info: Pointer to the 'FILE_INFO' structure to be initialized.
 *  @NameQueryMethod:
 *  @Data: Pointer to the 'FLT_CALLBACK_DATA' structure containing file-related
 *         and process related data.
 *
 *  Return:
 *    - Status code indicating the result of the initialization.
 *    - Upon success, returns 'STATUS_SUCCESS'.
 *    - The appropriate error code in case of failure.
 */
extern NTSTATUS
FileInfoInit(
    _Out_ PFILE_INFO FileInfo,
    _In_ ULONG NameQueryMethod,
    _In_ PFLT_CALLBACK_DATA Data
);

/*
 *  FileInfoDeInit() - Deinitializes and frees the internal resources
 *                     associated with a 'FILE_INFO' structure.
 *
 *  @FileInfo: Pointer to the 'FILE_INFO' structure to be deinitialized.
 */
extern void
FileInfoDeInit(
    _Inout_ PFILE_INFO FileInfo
);

/*
 *  FileInfoFree() - Deallocates the memory associated with a 'FILE_INFO'
 *                   struct dynamically allocated.
 *
 *  @FileInfo: Pointer to a reference of a 'FILE_INFO' structure.
 */
extern void
FileInfoFree(
    _Inout_ PFILE_INFO* FileInfo
);

#endif  /* FILE_INFO_H */
