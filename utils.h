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

extern void InitUnicodeString(
    PUNICODE_STRING Str
);

void BinaryDataToUnicodeSTR(
    PVOID BinData,
    PUNICODE_STRING UnicodeStr,
    ULONG DataSize
);

void WORDDataToUnicodeSTR(
    PVOID WordData,
    PUNICODE_STRING UnicodeStr,
    ULONG DataSize
);

NTSTATUS UnicodeStrToStaticWSTR(
    _Out_ WCHAR Dest[MAX_PATH],
    _In_ PUNICODE_STRING Src,
    _Inout_ PULONG pSize
);

#endif  /* UTILS_H */
