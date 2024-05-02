#ifndef INFO_H
#define INFO_H

#include "procinfo.h"
#include "fileinfo.h"
#include "mjfunc.h"

struct Info {

    MJFUNC      MjFunc;
    TIME_FIELDS TimeFields;
    PROC_INFO   ProcInfo;
    LIST_ENTRY  ListEntry;

    union {
        PFILE_INFO FileInfo;
    } Info;

    POOL_TYPE PoolType;
};

typedef struct Info INFO, * PINFO;

/*
 *  InfoAlloc() - Allocate a 'INFO' structure.
 *
 *  @PoolType: The type of memory pool to allocate from (paged or nonpaged).
 *
 *  Return:
 *    - Pointer to the allocated 'INFO' structure on success.
 *    - 'NULL' if memory allocation fails.
 */
extern PINFO
InfoAlloc(
    __drv_strictTypeMatch(__drv_typeExpr)POOL_TYPE PoolType
);

extern PINFO
InfoGet(
    __drv_strictTypeMatch(__drv_typeExpr)POOL_TYPE PoolType,
    _In_ ULONG NameQueryMethod,
    _In_ PFLT_CALLBACK_DATA Data
);

extern NTSTATUS
InfoInit(
    _Out_ PINFO Info,
    _In_ ULONG NameQueryMethod,
    _In_ PFLT_CALLBACK_DATA Data
);

extern void
InfoDeInit(
    _Inout_ PINFO Info
);

extern void
InfoFree(
    _Inout_ PINFO* Info
);

#endif  /* INFO_H */