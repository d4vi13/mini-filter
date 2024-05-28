#include "utils.h"


void CopyFileInfo(PFILE_PACK Dest, PFILE_INFO Src) {
    ULONG minPathSize, minFNameSize;
    minPathSize = min(256, Src->PathSize);
    minFNameSize = min(256, Src->FinalNameSize);

    RtlCopyBytes(&Dest->PathSize, &Src->PathSize, sizeof Src->PathSize);
    RtlCopyBytes(&Dest->FinalNameSize, &Src->FinalNameSize, sizeof Src->FinalNameSize);
    RtlCopyBytes(Dest->Path, Src->Path, minPathSize * sizeof *Dest->Path);
    RtlCopyBytes(Dest->FinalName, Src->FinalName, minFNameSize * sizeof *Dest->FinalName);
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
