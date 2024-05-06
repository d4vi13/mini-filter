/*++

Module Name:

    MiniFilter.c

Abstract:

    This is the main module of the MiniFilter miniFilter driver.

Environment:

    Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include "fileinfo.h"
#include "procinfo.h"
#include "info.h"



#define ARQ_MONITORADO L"target.exe"
#define ARQ_ESCRITO L"arqImportante.txt"
#define LOG_PATH L"\\DosDevices\\C:\\WINDOWS\\arquivoLog.txt"

union logData {
	WCHAR filename[256];

};

PFLT_FILTER FilterHandle = NULL;

NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS MiniPreCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext);

FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext,
	FLT_POST_OPERATION_FLAGS Flags);

FLT_PREOP_CALLBACK_STATUS MiniPreWrite(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext);

FLT_PREOP_CALLBACK_STATUS AmaterasuDefaultPreCallback(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext);

VOID ProcCreateCallback(
	HANDLE hParent,
	HANDLE hProcess,
	BOOLEAN create
);

const FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_CREATE,0,MiniPreCreate,MiniPostCreate},
	{IRP_MJ_WRITE,0,AmaterasuDefaultPreCallback,NULL},
	{IRP_MJ_OPERATION_END}
};

const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	Callbacks, // onde registramos nossos callbacks
	MiniUnload,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};



NTSTATUS MiniUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	FltUnregisterFilter(FilterHandle);

	return STATUS_SUCCESS;
}


FLT_POSTOP_CALLBACK_STATUS MiniPostCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects, 
	PVOID* CompletionContext,
	FLT_POST_OPERATION_FLAGS Flags) 
{
	//KdPrint(("Rotina de pos operação foi chamada"));
	return FLT_POSTOP_FINISHED_PROCESSING;
}

//This function should only be called at PASSIVE_LEVEL
VOID LogEvent(HANDLE srcPid, UCHAR mjFunctionCode, union logData info, PVOID* irpBuf, PULONG bufSiz)	{
	UNICODE_STRING logPath;
	OBJECT_ATTRIBUTES logObj;
	HANDLE h;
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;
	CHAR buffer[1024] = { 0 };
	size_t cb;

	RtlInitUnicodeString(&logPath, LOG_PATH);
	InitializeObjectAttributes(&logObj, &logPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	//Cria ou abrir arquivo
	status = ZwCreateFile(&h, GENERIC_WRITE, &logObj, &ioStatusBlock,
		NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_WRITE, FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	//KdPrint(("logEventCalled\n"));
	if (NT_SUCCESS(status)) {
		//KdPrint(("Deu boa abrir o arquivo\n"));
		if (buffer != NULL) {
			switch (mjFunctionCode) {
			case IRP_MJ_CREATE:
				KdPrint(("CREATE\n"));
				/*status = RtlStringCbPrintfA(buffer, sizeof(CHAR) * (256 + (*bufSiz)), "Processo(%d) abriu um arquivo(%wZ)\n", srcPid, info.filename);
				if (NT_SUCCESS(status)) {
					status = RtlStringCbLengthA(buffer, sizeof(CHAR) * (256 + (*bufSiz)), &cb);
					if (NT_SUCCESS(status)) {
						ZwWriteFile(h, NULL, NULL, NULL, &ioStatusBlock, buffer, cb, NULL, NULL);
					}
				}
				*/
				break;
			case IRP_MJ_WRITE:
				//-->bingo KdPrint(("%s\n", *irpBuf));
				//KdPrint(("Entrou no major write\n"));
				status = RtlStringCbPrintfA(buffer, sizeof(buffer), "Processo(%d) escreveu no arquivo(%wZ)\nConteudo escrito:", srcPid, info.filename);
				if (NT_SUCCESS(status)) {
					//KdPrint(("Fez o printf\n"));
					status = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
					if (NT_SUCCESS(status)) {
						//KdPrint(("Consegui tamanho\n"));
						ZwWriteFile(h, NULL, NULL, NULL, &ioStatusBlock, buffer, cb, NULL, NULL);
						//if (NT_SUCCESS(ioStatusBlock.Status))
							//KdPrint(("Deu boa a escrita\n"));
						//else
							//KdPrint(("Deu ruim a escrita\n"));
					}
				}
				//Logs write data
				ZwWriteFile(h, NULL, NULL, NULL, &ioStatusBlock, *irpBuf, *bufSiz, NULL, NULL);
				break;
			}
		}
		ZwClose(h);
	}
	else {
		KdPrint(("Deu ruim abrir o arquivo(codigo: %x)\n", status));

	}
}

FLT_PREOP_CALLBACK_STATUS MiniPreCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext) 
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR nomeDoArquivo[250] = {0};
	WCHAR targetProcNome[] = ARQ_MONITORADO;
	HANDLE pid = PsGetCurrentProcessId();
	PEPROCESS proc;
	PUNICODE_STRING nome;

	//Get Process name 
	PsLookupProcessByProcessId(pid, &proc);
	SeLocateProcessImageName(proc, &nome);


	if (nome->Buffer != NULL) {
		if (wcsstr(nome->Buffer, targetProcNome) != NULL) {
			status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);
			if (NT_SUCCESS(status)) {
				status = FltParseFileNameInformation(FileNameInfo);
				if (NT_SUCCESS(status)) {
					if (FileNameInfo->Name.MaximumLength < 260) {
						RtlCopyMemory(nomeDoArquivo, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
						//KdPrint(("%wZ(PID: %d) realizou criou um arquivo.\n", nome, pid));
						//KdPrint(("Nome do arquivo criado: %ws\n", nomeDoArquivo));
					}
				}
				FltReleaseFileNameInformation(FileNameInfo);
			}
		}
	}

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS MiniPreWrite(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR nomeDoArquivo[256] = { 0 };
	WCHAR targetProcNome[] = ARQ_MONITORADO;
	union logData info;
	PULONG bufLen;
	PVOID* buffer;
	HANDLE pid = PsGetCurrentProcessId();
	PEPROCESS proc;
	PUNICODE_STRING nome;
	


	//Get Process name 
	PsLookupProcessByProcessId(pid, &proc);
	SeLocateProcessImageName(proc, &nome);

	if (nome->Buffer != NULL) {
		//Looks for the target process in the path returned
		if (wcsstr(nome->Buffer, targetProcNome) != NULL) {
			status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);
			if (NT_SUCCESS(status)) {
				status = FltParseFileNameInformation(FileNameInfo);
				if (NT_SUCCESS(status)) {
					if (FileNameInfo->Name.MaximumLength <= 256) {
						RtlCopyMemory(info.filename, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
						//KdPrint(("%wZ(PID: %d) realizou uma escrita em :\n", nome, pid));
						//KdPrint(("Houve uma escrita em: %ws\n", info.filename));
						status = FltDecodeParameters(Data, NULL, NULL, &bufLen, NULL);
						if (NT_SUCCESS(status)) {
							//WCHAR* buffer = (WCHAR*)ExAllocatePool(NonPagedPoolExecute, sizeof(WCHAR) * (*bufLen));

							//if (buffer != NULL) {
								//RtlZeroMemory(buffer, sizeof(WCHAR) * (*bufLen));
							status = FltDecodeParameters(Data, NULL, (PVOID**)&buffer, NULL, NULL);
							//if (NT_SUCCESS(status))
								//LogEvent(pid, Data->Iopb->MajorFunction, info, buffer, bufLen);
							//ExFreePool(buffer);
						//}
						}
						KdPrint(("TESTE\n"));
					}
				}
				FltReleaseFileNameInformation(FileNameInfo);
			}
		}
	}

	// FLT_PREOP_SUCCESS_NO_CALLBACK sinaliza que não para chamar as pos operações
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

VOID ProcCreateCallback(
	HANDLE hParent,
	HANDLE hProcess,
	BOOLEAN create)
{

	PEPROCESS proc;
	PUNICODE_STRING nome, nomeFilho, target;
	WCHAR* buffer = (WCHAR*)ExAllocatePool(NonPagedPoolExecute, sizeof(WCHAR) * 255);
	NTSTATUS status;


	if (buffer != NULL) {

		RtlZeroMemory(buffer, sizeof(WCHAR) * 255);

		wcscpy(buffer, ARQ_MONITORADO);

		//RtlInitUnicodeString(target, buffer, sizeof(buffer));

		PsLookupProcessByProcessId(hParent, &proc);
		SeLocateProcessImageName(proc, &nome);

		PsLookupProcessByProcessId(hProcess, &proc);
		SeLocateProcessImageName(proc, &nomeFilho);

		//KdPrint(("%wZ foi iniciado ou abriu um subprocesso\n", nome));

		if (wcsstr(nome->Buffer, buffer) != NULL) {
			KdPrint(("%wZ(PID: %ld)(fPID: %ld) foi iniciado ou abriu um subprocesso\n", nome, hParent, hProcess));
			if (create)
				KdPrint(("%wZ foi iniciou %wZ\n", nome, nomeFilho));
			else
				KdPrint(("%wZ foi encerrou %wZ\n", nome, nomeFilho));
		}

		ExFreePool(buffer);
	}
}

FLT_PREOP_CALLBACK_STATUS AmaterasuDefaultPreCallback(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext)
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
			irq = KeGetCurrentIrql();
			KdPrint(("%ws %c %d\n",irq,irq,irq));
			if (irq == PASSIVE_LEVEL)
				KdPrint(("nivel passivo\n"));
			if (irq == DISPATCH_LEVEL)
				KdPrint(("nivel de despacho\n"));
			fileInfo = FileInfoGet(NonPagedPool, FLT_FILE_NAME_QUERY_DEFAULT,Data);
			if (fileInfo != NULL) {
				KdPrint(("------------------------------------\n"));
				KdPrint(("PID: %lu %p\n", pid,pid));
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
				KdPrint(("ProcInfoGet failed\n"));
			Info = InfoGet(NonPagedPool, FLT_FILE_NAME_QUERY_DEFAULT,Data);
			if (Info != NULL) {
				KdPrint(("------------------------------------\n"));
				KdPrint(("PID: %lu %p\n", pid, pid));
				KdPrint(("Day: %hu\n", Info->TimeFields.Day));
				KdPrint(("Hour: %hu\n", Info->TimeFields.Hour));
				KdPrint(("Minute: %hu\n", Info->TimeFields.Minute));
				KdPrint(("Seconds: %hu\n",Info->TimeFields.Second));
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

NTSTATUS CloneFileInfo(PIRP Irp, PIO_STATCK_LOCATION IrpIoStack, PULONG InfoSize){
    
    

    return status;
}

NTSTATUS IoControl(PDEVICE_OBJECT Device, PIRP Irp){
    PIO_STATCK_LOCATION IrpIoStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG IoCtl, ClonedInfoSize;
    NTSTATUS status = NT_SUCCESS;

    if (IrpIoStack != NULL){
        IoCtl = IrpIoStack->Parameters.DeviceIoControl.IoControlCode;
        switch (IoCtl) {
            case IOCTL_FILE_LOG:
                status = CloneFileInfo(Irp, IrpIoStack, &ClonedInfoSize);
                break;
            case IOCTL_PROC_LOG:
                break;
            default:
                break;
        }
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = ClonedInfoSize;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	status = PsSetCreateProcessNotifyRoutine(ProcCreateCallback, FALSE);

	if (!NT_SUCCESS(status))
		KdPrint(("Não foi possivel registrar o callback de monitoramento de processo\n"));
	else 
		KdPrint(("Callback de monitoramento de processo registrado\n"));


	// Registrar driver no filter manager 
	status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);

	// se a regsitração teve sucesso começamos a filtragem
	if (NT_SUCCESS(status))
	{
		//notifica que o minidriver esta pronto para comecar a filtrar IO request
		status = FltStartFiltering(FilterHandle);
		if (!NT_SUCCESS(status))
		{
			FltUnregisterFilter(FilterHandle);
		}
	}
	return status;
}
