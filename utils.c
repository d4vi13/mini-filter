#include "utils.h"


void InitUnicodeString(PUNICODE_STRING Str) {
    Str->Buffer = ExAllocatePoolWithTag(NonPagedPool, MAX_STR_SIZE, 'buf');
    Str->Length = 0;
    Str->MaximumLength = MAX_STR_SIZE;
}

void BinaryDataToUnicodeSTR(PVOID BinData, PUNICODE_STRING UnicodeStr, ULONG DataSize) {
    UNICODE_STRING hexValue;
    InitUnicodeString(&hexValue);

    for (ULONG i = 0; i < DataSize; ++i) {
        RtlIntegerToUnicodeString(((UCHAR*)BinData)[i], 16, &hexValue);

        RtlUnicodeStringCat(UnicodeStr, &hexValue);
        RtlUnicodeStringCatString(UnicodeStr, L" ");
    }

    ExFreePoolWithTag(hexValue.Buffer, 'buf');
}

void WORDDataToUnicodeSTR(PVOID WordData, PUNICODE_STRING UnicodeStr, ULONG DataSize) {
    UNICODE_STRING hexValue;
    InitUnicodeString(&hexValue);

    for (ULONG i = DataSize; i > 0; --i) {
        RtlIntegerToUnicodeString(((UCHAR*)WordData)[i], 16, &hexValue);

        RtlUnicodeStringCat(UnicodeStr, &hexValue);
        RtlUnicodeStringCatString(UnicodeStr, L" ");
    }

    ExFreePoolWithTag(hexValue.Buffer, 'buf');
}

/*
 *  UnicodeStrToWSTR() -
 *
 *  @PoolType:
 *  @Src:
 *  @Dest:
 *  @pSize:
 *
 *  Return:
 *    -
 *    -
 */
NTSTATUS UnicodeStrToWSTR(_In_ POOL_TYPE PoolType, _In_ PUNICODE_STRING Src, _Out_ PWSTR* Dest, _Out_ PULONG pSize) {

    ULONG Size;
    PWSTR Wstr;
    /*
     *  As 'UNICODE_STRING::Length' does not include the '/0', we
     *  add '1' to it.
     */
    Size = Src->Length + 1;
    Wstr = ExAllocatePoolWithTag(PoolType, 2*Size, 'wstr');
    if(!Wstr) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* null terminate the string. */
    Wstr[Size - 1] = 0;
    RtlCopyBytes(Wstr, Src->Buffer, Size);

    *pSize = Size;
    *Dest = Wstr;

    return STATUS_SUCCESS;
}

NTSTATUS UnicodeStrToStaticWSTR(_Out_ WCHAR Dest[MAX_PATH], _In_ PUNICODE_STRING Src, _Inout_ PULONG pSize) {
    ULONG minSize = min(MAX_PATH, Src->Length/sizeof(WCHAR));

    /* null terminate the string. */
    Dest[minSize - 1] = 0;
    
    RtlCopyBytes(Dest, Src->Buffer, minSize);

    *pSize = minSize;

    return STATUS_SUCCESS;
}

