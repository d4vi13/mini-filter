#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <ntifs.h>
#endif

#ifndef PROC_INFO_H
#define PROC_INFO_H

struct ProcInfo {

    ULONG PID;
    ULONG SID;

    BOOLEAN IsElevated;

    POOL_TYPE PoolType;
};

typedef struct ProcInfo PROC_INFO, *PPROC_INFO;

/*
 *  ProcInitInfo() - Initializes a 'PROC_INFO' structure based on 
 *                   'FLT_CALLBACK_DATA'.
 *
 *  @Info: Pointer to the 'INFO' structure to be initialized.
 *  @Data: Pointer to the 'FLT_CALLBACK_DATA' structure containing process 
 *         related data.
 *
 *  Return:
 *    - Status code indicating the result of the initialization.
 *    - Upon success, returns 'STATUS_SUCCESS'.
 *    - The appropriate error code in case of failure.
 */
extern NTSTATUS
ProcInfoInit(
        _Out_ PPROC_INFO ProcInfo,
        _In_ PFLT_CALLBACK_DATA Data
    );

extern PPROC_INFO
ProcInfoGet(
    _In_ POOL_TYPE PoolType,
    _In_ PFLT_CALLBACK_DATA Data
    );

extern void
ProcInfoFree(
    _Inout_  PPROC_INFO* ProcInfo
   );

#endif  /* PROC_INFO_H */
