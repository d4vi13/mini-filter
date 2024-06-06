#include "RegInfo.h"


PREG_INFO RegInfoAlloc(_PoolType_ POOL_TYPE PoolType) {

	PREG_INFO RegInfo;

	RegInfo = ExAllocatePoolWithTag(PoolType, sizeof * RegInfo, 'reg');
	if (!RegInfo)
		return NULL;

	RegInfo->PoolType = PoolType;

	return RegInfo;
}


static inline void GetRegBinaryData(PUNICODE_STRING Data, PREG_SET_VALUE_KEY_INFORMATION RegStruct) {
	PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, RegStruct->DataSize, 'buf');
	if (!buffer)
		return;

	RtlCopyMemory(buffer, RegStruct->Data, RegStruct->DataSize);

	BinaryDataToUnicodeSTR(buffer, Data, RegStruct->DataSize);

	ExFreePoolWithTag(buffer, 'buf');
}

static inline void GetRegWORDData(PUNICODE_STRING Data, PREG_SET_VALUE_KEY_INFORMATION RegStruct) {
	PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, RegStruct->DataSize, 'buf');
	if (!buffer)
		return;

	RtlCopyMemory(buffer, RegStruct->Data, RegStruct->DataSize);

	WORDDataToUnicodeSTR(buffer, Data, RegStruct->DataSize);

	ExFreePoolWithTag(buffer, 'buf');
}

static NTSTATUS GetKeyData(PREG_INFO RegInfo, PREG_SET_VALUE_KEY_INFORMATION RegStruct) {

	UNICODE_STRING Data;
	InitUnicodeString(&Data);

	RegInfo->DataType = RegStruct->Type;
	RegInfo->DataSize = RegStruct->DataSize;

	switch (RegStruct->Type) {
	case REG_SZ:
		RtlCopyMemory(Data.Buffer, RegStruct->Data, RegStruct->DataSize);
		break;
	case REG_EXPAND_SZ:
		RtlCopyMemory(Data.Buffer, RegStruct->Data, RegStruct->DataSize);
		break;
	case REG_MULTI_SZ:
		RtlCopyMemory(Data.Buffer, RegStruct->Data, RegStruct->DataSize);
		break;
	case REG_BINARY:
		GetRegBinaryData(&Data, RegStruct);
		break;
	case REG_DWORD:
		GetRegWORDData(&Data, RegStruct);
		break;
	case REG_QWORD:
		GetRegWORDData(&Data, RegStruct);
		break;

	}

	UnicodeStrToWSTR(RegInfo->PoolType, &Data, &RegInfo->Data, &RegInfo->DataSize);

	ExFreePoolWithTag(Data.Buffer, 'buf');

	return STATUS_SUCCESS;
}

NTSTATUS SetValueInit(
	PREG_INFO RegInfo,
	PREG_SET_VALUE_KEY_INFORMATION RegStruct,
	REG_NOTIFY_CLASS RegOperation
) {
	ULONG ReturnLength;
	NTSTATUS Status;

	UNICODE_STRING QueryStr;
	InitUnicodeString(&QueryStr);

	Status = ObQueryNameString(RegStruct->Object, (POBJECT_NAME_INFORMATION)&QueryStr, QueryStr.MaximumLength, &ReturnLength);
	if (!NT_SUCCESS(Status)) {
		ExFreePoolWithTag(QueryStr.Buffer, 'aux');
		return Status;
	}

	Status = UnicodeStrToWSTR(RegInfo->PoolType, &QueryStr, &RegInfo->CompleteName, &RegInfo->CompleteNameSize);
	if (!NT_SUCCESS(Status))
		ExFreePoolWithTag(QueryStr.Buffer, 'aux');

	return Status;

}

NTSTATUS DeleteValueInit(
	PREG_INFO RegInfo,
	PREG_DELETE_VALUE_KEY_INFORMATION RegStruct,
	REG_NOTIFY_CLASS RegOperation
) {
	ULONG ReturnLength;
	NTSTATUS Status;

	UNICODE_STRING QueryStr;
	InitUnicodeString(&QueryStr);

	Status = ObQueryNameString(RegStruct->Object, (POBJECT_NAME_INFORMATION)&QueryStr, QueryStr.MaximumLength, &ReturnLength);
	if (!NT_SUCCESS(Status)) {
		ExFreePoolWithTag(QueryStr.Buffer, 'aux');
		return Status;
	}

	Status = UnicodeStrToWSTR(RegInfo->PoolType, &QueryStr, &RegInfo->CompleteName, &RegInfo->CompleteNameSize);
	if (!NT_SUCCESS(Status)) {
		ExFreePoolWithTag(QueryStr.Buffer, 'aux');
		return Status;
	}


	RegInfo->Data = NULL;
	RegInfo->DataSize = 0;
	RegInfo->DataType = 0;

	return Status;
}

NTSTATUS RegInfoInit(
	PREG_INFO RegInfo,
	REG_NOTIFY_CLASS RegOperation,
	PVOID RegStruct
) {
	NTSTATUS Status;

	switch (RegOperation)
	{
	case RegNtSetValueKey:
		Status = SetValueInit(RegInfo, RegStruct, RegOperation);
		break;
	case RegNtDeleteValueKey:
		Status = DeleteValueInit(RegInfo, RegStruct, RegOperation);
		break;
	default:
		Status = STATUS_UNSUCCESSFUL;
		break;
	}
	
	return Status;
}


void RegInfoDeInit(PREG_INFO RegInfo) {

	ExFreePoolWithTag(RegInfo->CompleteName, 'wstr');

	if (RegInfo->Data)
		ExFreePoolWithTag(RegInfo->Data, 'wstr');

}

void RegInfoFree(PREG_INFO* RegInfo) {
	if (*RegInfo == NULL)
		return;
	
	RegInfoDeInit(*RegInfo);

	ExFreePoolWithTag(*RegInfo, 'reg');

	*RegInfo = NULL;
}

PREG_INFO RegInfoGet(_PoolType_ POOL_TYPE PoolType, PREG_INFO_DATA RegInfoData) {

	PREG_INFO RegInfo;
	NTSTATUS Status;

	RegInfo = RegInfoAlloc(PoolType);
	if (!RegInfo)
		return NULL;

	Status = RegInfoInit(RegInfo, RegInfoData->RegNotifyClass, RegInfoData->RegStruct);
	if (NT_SUCCESS(Status)) {
		RegInfoFree(&RegInfo);
		return NULL;
	}

	return RegInfo;
}

void RegInfoCopy(PREG_INFO_STATIC StaticRegInfo, PREG_INFO RegInfo) {
	ULONG DataSize;
	
	StaticRegInfo->RegNotifyClass = RegInfo->RegNotifyClass;
	StaticRegInfo->CompleteNameSize = RegInfo->CompleteNameSize;
	StaticRegInfo->DataSize = RegInfo->DataSize;
	StaticRegInfo->DataType = RegInfo->DataType;

	DataSize = min(MAX_PATH, RegInfo->CompleteNameSize);
	RtlCopyMemory(StaticRegInfo->CompleteName, RegInfo->CompleteName, sizeof * RegInfo->CompleteName * DataSize);

	DataSize = min(MAX_DATA, RegInfo->DataSize);
	RtlCopyMemory(StaticRegInfo->Data, RegInfo->Data, sizeof * RegInfo->Data * DataSize);
}