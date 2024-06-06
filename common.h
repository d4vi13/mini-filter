#ifndef COMMON_H
#define COMMON_H

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <ntifs.h>

#define _PoolType_ __drv_strictTypeMatch(__drv_typeExpr)
#define MAX_STR_SIZE  sizeof(WCHAR) * NTSTRSAFE_UNICODE_STRING_MAX_CCH
#define MAX_PATH 260
#define MAX_DATA 2048

typedef struct InfoList INFO_LIST, * PINFO_LIST;
typedef struct LoadImageInfo LOAD_IMAGE_INFO, * PLOAD_IMAGE_INFO;
typedef struct LoadImageInfoStatic LOAD_IMAGE_INFO_STATIC, * PLOAD_IMAGE_INFO_STATIC;
typedef enum InfoType INFO_TYPE, * PINFO_TYPE;



#endif  /* COMMON_H */