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
#include<stdio.h>
#include<stdlib.h>




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

const FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_CREATE,0,MiniPreCreate,MiniPostCreate},
	{IRP_MJ_WRITE,0,MiniPreWrite,NULL},
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
	KdPrint(("Rotina de pos operação foi chamada"));
	return FLT_POSTOP_FINISHED_PROCESSING;
}

//A minifilter driver's PFLT_PRE_OPERATION_CALLBACK routine 
//performs pre-operation processing for I/O operations
FLT_PREOP_CALLBACK_STATUS MiniPreCreate(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext) 
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	
	
	//WCHAR é um typedef em cima de wchar_t
	//wchar_t : big chungus char, char só 256 possibilidades, wchar 65536 possibilidades
	
	WCHAR nomeDoArquivo[260] = {0};

	// Data: estrtura de dados para operção de I/O
	// Segundo Parametro: especifica o formato da informação a ser retornada e metodo de query
	// terceiro parametro: ponteiro para uma estrutura(alocada pelo sistema) que contem as informações do nome do arquivo
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);


	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(FileNameInfo);
		if (NT_SUCCESS(status)) {
			// checa se cabe no buffer
			if (FileNameInfo->Name.MaximumLength < 260) {
				// copia um bloco de memoria(indicado pelo segundo parametro) para outro local(primeiro parametro)
				// ultimo parametro define o numero de bytes a ser copiado
				RtlCopyMemory(nomeDoArquivo, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
				KdPrint(("Um arquivo foi criado: %ws", nomeDoArquivo));
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}

	// esse retorno sinaliza pro filter manager que é para chamar as rotina 
	// de pos operação, para finaalizar o processamento do IO request
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS MiniPreWrite(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext)
{
	PFLT_FILE_NAME_INFORMATION FileNameInfo;
	NTSTATUS status;
	WCHAR nomeDoArquivo[260] = { 0 };

	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);

	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(FileNameInfo);
		if (NT_SUCCESS(status)) {
			if (FileNameInfo->Name.MaximumLength < 260) {
				RtlCopyMemory(nomeDoArquivo, FileNameInfo->Name.Buffer, FileNameInfo->Name.MaximumLength);
				KdPrint(("Um arquivo foi escrito: %ws", nomeDoArquivo));
			}
		}
		FltReleaseFileNameInformation(FileNameInfo);
	}

	// FLT_PREOP_SUCCESS_NO_CALLBACK sinaliza que não para chamar as pos operações
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

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
