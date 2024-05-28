#include "callbacks.h"
#include "amaterasu.h"

#define ARQ_MONITORADO L"target.exe"

AmaterasuDefaultPreCallback(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
	NTSTATUS status;
	PFILE_INFO fileInfo;
	HANDLE pid = PsGetCurrentProcessId();
	PEPROCESS proc;
	PUNICODE_STRING nome;
	WCHAR targetProcNome[] = ARQ_MONITORADO;
	KIRQL irq;

	PPROC_INFO ProcInfo;
	PINFO Info;


	//Get Process name 
	PsLookupProcessByProcessId(pid, &proc);
	SeLocateProcessImageName(proc, &nome);
	if (nome->Buffer != NULL) {
		if (wcsstr(nome->Buffer, targetProcNome) != NULL) {
			KdPrint(("callback\n"));

			/*irq = KeGetCurrentIrql();
			KdPrint(("%ws %c %d\n", irq, irq, irq));
			if (irq == PASSIVE_LEVEL)
				KdPrint(("nivel passivo\n"));
			if (irq == DISPATCH_LEVEL)
				KdPrint(("nivel de despacho\n"));
			fileInfo = FileInfoGet(NonPagedPool, FLT_FILE_NAME_QUERY_DEFAULT, Data);
			if (fileInfo != NULL) {
				KdPrint(("------------------------------------\n"));
				KdPrint(("PID: %lu %p\n", pid, pid));
				KdPrint(("Proc Name: %wZ\n", nome->Buffer));
				KdPrint(("Path: %ws\n", fileInfo->Path));
				KdPrint(("Path size: %ld\n", fileInfo->PathSize));
				KdPrint(("File Name: %ws\n", fileInfo->FinalName));
				KdPrint(("File Name Size: %ld\n", fileInfo->FinalNameSize));
				FileInfoFree(&fileInfo);
			}
			ProcInfo = ProcInfoGet(NonPagedPool, Data);
			if (ProcInfo != NULL) {
				KdPrint(("------------------------------------\n"));
				KdPrint(("PID: %ld\n", ProcInfo->PID));
				KdPrint(("SID: % ld\n", ProcInfo->SID));
				KdPrint(("IsElevated: % ld\n", ProcInfo->IsElevated));
				ProcInfoFree(&ProcInfo);
			}
			else
				KdPrint(("ProcInfoGet failed\n"));*/
			InfoListAppend(Amaterasu.InfoList, Data);
			Info = InfoGet(NonPagedPool, FLT_FILE_NAME_QUERY_DEFAULT, Data);
			//InsertHeadList(&RecordList, &(Info->ListEntry));
			if (Info != NULL) {
				KdPrint(("------------------------------------\n"));
				KdPrint(("PID: %lu %p\n", pid, pid));
				KdPrint(("Day: %hu\n", Info->TimeFields.Day));
				KdPrint(("Hour: %hu\n", Info->TimeFields.Hour));
				KdPrint(("Minute: %hu\n", Info->TimeFields.Minute));
				KdPrint(("Seconds: %hu\n", Info->TimeFields.Second));
				KdPrint(("Seconds: %hu\n", Info->TimeFields.Second));
				KdPrint(("Seconds: %hu\n", Info->TimeFields.Second));
				KdPrint(("PID: %ld\n", Info->ProcInfo.PID));
				KdPrint(("SID: % ld\n", Info->ProcInfo.SID));
				KdPrint(("IsElevated: % ld\n", Info->ProcInfo.IsElevated));
				KdPrint(("Proc Name: %wZ\n", nome->Buffer));
				KdPrint(("Path: %ws\n", Info->Info.FileInfo->Path));
				KdPrint(("Path size: %ld\n", Info->Info.FileInfo->PathSize));
				KdPrint(("File Name: %ws\n", Info->Info.FileInfo->FinalName));
				KdPrint(("File Name Size: %ld\n", Info->Info.FileInfo->FinalNameSize));
				KdPrint(("MajorFunction: %ws\n", Info->MjFunc.Name));
				KdPrint(("MajorFunction: %zu\n", Info->MjFunc.Code));
				InfoFree(&Info);

			}

		}
	}


	return FLT_PREOP_SUCCESS_NO_CALLBACK;

}

NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	FltUnregisterFilter(Amaterasu.FilterHandle);

	return STATUS_SUCCESS;
}