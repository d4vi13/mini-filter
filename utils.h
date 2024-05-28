#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include "fileinfo.h"
#endif

#ifndef UTILS_H
#define UTILS_H

extern void CopyFileInfo(PFILE_PACK Dest, PFILE_INFO Src);

extern NTSTATUS
UnicodeStrToWSTR(
        _In_ POOL_TYPE PoolType,
        _In_ PUNICODE_STRING Src,
        _Out_ PWSTR* Dest,
        _Out_ PULONG pSize
    );

#endif  /* UTILS_H */
