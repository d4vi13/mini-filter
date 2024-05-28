#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "fileinfo.h"

extern NTSTATUS
UnicodeStrToWSTR(
        _In_ POOL_TYPE PoolType,
        _In_ PUNICODE_STRING Src,
        _Out_ PWSTR* Dest,
        _Out_ PULONG pSize
    );

extern void CopyFileInfo(PFILE_PACK Dest,PFILE_INFO Src);


#endif  /* UTILS_H */
