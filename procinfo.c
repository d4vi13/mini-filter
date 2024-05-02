
#include "procinfo.h"

/*
 *  GetRequestorProcStatus() -
 *
 *  @ProcStatus:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */

/*
static NTSTATUS GetRequestorProcStatus(_Out_ PBOOL ProcStatus, _In_ PFLT_CALLBACK_DATA Data) {

    NTSTATUS Status;

    return Status;
}
*/

/*
 *  GetRequestorSID() -
 *
 *  @SID:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
static inline NTSTATUS GetRequestorSID(_Out_ PULONG SID, _In_ PFLT_CALLBACK_DATA Data) {

    /*
     *  Even though 'FltGetRequestorSessionId()' may return '-1', meaning
     *  that the process has no session, we consider it a success.
     */
    return FltGetRequestorSessionId(Data, SID);
}

/*
 *  GetRequestorPID() - Retrieves the Process ID.
 *
 *  @Data: Pointer to a 'FLT_CALLBACK_DATA' structure containing information
 *         about the callback.
 *
 *  Return:
 *    - Process ID.
 */
static inline ULONG GetRequestorPID(_In_ PFLT_CALLBACK_DATA Data) {

    /*
     *  Gets the process ID for the process associated with the thread that
     *  requested the I/O operation.
     */
    return FltGetRequestorProcessId(Data);
}

static inline PEPROCESS GetEProcess(PFLT_CALLBACK_DATA Data) {
    return FltGetRequestorProcess(Data);
}

/*
 *  AcquireElevationStatus() - Retrieves TOKEN_ELEVATION structure.
 *
 *  @proc: Pointer to a EPROCESS structure
 * 
 *  @tokenInfo: Pointer to TOKEN_ELEVATION to be filled
 *
 *  Return:
 *    - NTSTATUS .
 */
NTSTATUS AcquireElevationStatus(_In_ PEPROCESS proc, _Out_ TOKEN_ELEVATION* tokenInfo) {
    PACCESS_TOKEN ptoken;
    
    /*Acquires reference to the primary token*/
    ptoken= PsReferencePrimaryToken(proc);
    
    if (ptoken == NULL) {
        tokenInfo = NULL;
        return STATUS_SUCCESS;
    }

    return SeQueryInformationToken(ptoken, TokenElevation, &tokenInfo);
}

/*
 *  GetElevationStatus() - Retrieves Elevation status.
 *
 *  @proc: Pointer to a EPROCESS structure
 *
 *  Return:
 *    - NTSTATUS .
 */
NTSTATUS GetElevationStatus(_Out_ PPROC_INFO ProcInfo, _In_ PFLT_CALLBACK_DATA Data) {
    NTSTATUS Status;
    TOKEN_ELEVATION tokenInfo;
    PEPROCESS eprocess;
    
    eprocess = GetEProcess(Data);
    if (eprocess == NULL) {
        KdPrint(("GetEProcessFalhou\n"));
        return 1;
        //TODO: Find the proper NTSTATUS value to return
    }

    Status = AcquireElevationStatus(eprocess, &tokenInfo);
    if (!NT_SUCCESS(Status)) {
        KdPrint(("Acquire falhou\n"));
        return Status;
    }

    ProcInfo->IsElevated = tokenInfo.TokenIsElevated;

    return STATUS_SUCCESS;
}

/*
 *  ProcInfoInit() -
 *
 *  @ProcInfo:
 *  @Data:
 *
 *  Return:
 *    -
 *    -
 */
NTSTATUS ProcInfoInit(_Out_ PPROC_INFO ProcInfo, _In_ PFLT_CALLBACK_DATA Data) {
    NTSTATUS Status;
    PVOID tokenInfo;
    PEPROCESS eprocess;

    ProcInfo->PID = GetRequestorPID(Data);
    KdPrint(("%lu %p\n", ProcInfo->PID,ProcInfo->PID));

    Status = GetRequestorSID(&ProcInfo->PID, Data);
    if (!NT_SUCCESS(Status)) {
        KdPrint(("GetRequestorSid falhou %x\n", Status));
        return Status;
    }

    Status = GetElevationStatus(ProcInfo, Data);
    if (!NT_SUCCESS(Status)) {
        KdPrint(("GetElevationStatus falhou %x\n",Status));
        return Status;
    }

    /*
    Status = GetRequestorStatus(&ProcInfo->Status, Data);
    if(!NT_SUCCESS(Status)) {
        return Status;
    }
    */

    return Status;
}

/*
 *  ProcInfoAlloc() - Allocates PROC_INFO structure
 *
 *  @PoolType: The type of memory pool to allocate from (paged or nonpaged).
 *
 *  Return:
 *    - Pointer to the allocated 'PROC_INFO' structure on success.
 *    - 'NULL' if memory allocation fails.
 */
PPROC_INFO ProcInfoAlloc(POOL_TYPE PoolType) {
    PPROC_INFO ProcInfo;

    ProcInfo = ExAllocatePoolWithTag(PoolType, sizeof(PROC_INFO), 'proc');
    if (ProcInfo == NULL)
        return NULL;

    RtlZeroMemory(ProcInfo, sizeof(PROC_INFO));

    ProcInfo->PoolType = PoolType;

    return ProcInfo;

}


/*
 *  ProcInfoFree() - Deallocates the memory associated with a 'PROC_INFO'
 *                   struct dynamically allocated.
 *
 *  @ProcInfo: Pointer to a reference of a 'PROC_INFO' structure.
 */
void ProcInfoFree(_Inout_ PPROC_INFO* ProcInfo) {

    ExFreePoolWithTag(*ProcInfo, 'proc');

    *ProcInfo = NULL;
}


PPROC_INFO ProcInfoGet(_In_ POOL_TYPE PoolType, _In_ PFLT_CALLBACK_DATA Data) {
    NTSTATUS status;
    PPROC_INFO ProcInfo;

    ProcInfo = ProcInfoAlloc(PoolType);

    if (ProcInfo == NULL) {
        KdPrint(("ProcInfoAlloc\n"));
        return NULL;
    }

    status = ProcInfoInit(ProcInfo, Data);
    if (!NT_SUCCESS(status)) {
        KdPrint(("ProcInfoInit falhou\n"));
        ProcInfoFree(&ProcInfo);
        return NULL;
    }

    return ProcInfo;

}
