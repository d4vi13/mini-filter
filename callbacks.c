#include "amaterasu.h"

#define ARQ_MONITORADO L"target.exe"

AmaterasuDefaultPreCallback(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{

	NTSTATUS Status;

	Status = InfoListAppend(&Amaterasu.InfoList, Data, INFO_FS);
	if (!NT_SUCCESS(Status)) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;


}


void AmaterasuLoadImageCallback(
	PUNICODE_STRING FullImageName,
	HANDLE ProcessId,
	PIMAGE_INFO ImageInfo
) {

	LOAD_IMAGE_DATA LoadImageData;
	
	LoadImageData.FullImageName = FullImageName;
	LoadImageData.ProcessId = ProcessId;
	LoadImageData.ImageInfo = ImageInfo;

	InfoListAppend(Amaterasu.InfoList, &LoadImageData, INFO_LOAD);

	return;
}

NTSTATUS AmaterasuRegCallback(
	PVOID CallbackContext,
	PVOID RegNotifyClass,
	PVOID RegStruct
){
	NTSTATUS Status;
	
	REG_INFO_DATA RegInfoData;

	RegInfoData.RegNotifyClass = (REG_NOTIFY_CLASS)RegNotifyClass;
	RegInfoData.RegStruct = RegStruct;

	Status = InfoListAppend(Amaterasu.InfoList, &RegInfoData, INFO_REG);

	return Status;
}

void AmaterasuProcCallback(_In_ HANDLE PPID, _In_ HANDLE PID, _In_ BOOLEAN Active) {

	NTSTATUS Status;

	IDENTIFIER IDs = {
		PPID,
		PID,
		FALSE,
		Active
	};

	Status = InfoListAppend(Amaterasu.InfoList, &IDs, INFO_PROC);
	if (!NT_SUCCESS(Status)) {
		return;
	}
}

void AmaterasuThreadCallback(_In_ HANDLE PPID, _In_ HANDLE TID, _In_ BOOLEAN Active) {

	NTSTATUS Status;

	IDENTIFIER IDs = {
		PPID,
		TID,
		TRUE,
		Active
	};

	Status = InfoListAppend(Amaterasu.InfoList, &IDs, INFO_PROC);
	if (!NT_SUCCESS(Status)) {
		return;
	}
}




