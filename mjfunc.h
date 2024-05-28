
#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <ntifs.h>
#endif

#ifndef MJFUNC_H
#define MJFUNC_H

struct MjFunc {

    UCHAR Code;
    ULONG Len;
    PWSTR Name;
};

typedef struct MjFunc MJFUNC, * PMJFUNC;

extern NTSTATUS
MjFuncInit(
    _Out_ PMJFUNC MjFunc,
    _In_ UCHAR Code
);

#endif  /* MJFUNC_H */