#pragma once
#include "info.h"
#include "infolist.h"
#include "callbacks.h"
#include "ioctl.h"
#include "utils.h"
#ifndef CALLBACKS_H
#define CALLBACKS_H

extern FLT_PREOP_CALLBACK_STATUS
AmaterasuDefaultPreCallback(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

extern NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS Flags);

#endif  /* CALLBACKS_H */