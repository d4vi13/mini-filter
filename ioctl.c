#include "amaterasu.h"


NTSTATUS CloneFileInfo(PIRP Irp, PIO_STACK_LOCATION IrpIoStack, PULONG InfoSize) {
	NTSTATUS status = STATUS_SUCCESS;
	PINFO Info;
	PINFO_PACK InfoBuffer;
	ULONG InfoBufferLen = IrpIoStack->Parameters.DeviceIoControl.OutputBufferLength;

	KdPrint(("CloneFileInfo\n"));

	if (IsListEmpty(&Amaterasu.InfoList->Head))
		return STATUS_SUCCESS;

	Info = InfoListRemoveTail(Amaterasu.InfoList);

	InfoBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (!InfoBuffer) {
		if (sizeof *Info <= InfoBufferLen) {

			//CopyFileInfo(&InfoBuffer->Info.FileInfo, Info->Info.FileInfo);
			RtlCopyMemory(&InfoBuffer->MjFunc, &Info->MjFunc, sizeof(Info->MjFunc));
			RtlCopyMemory(&InfoBuffer->TimeFields, &Info->TimeFields, sizeof(Info->TimeFields));
			RtlCopyMemory(&InfoBuffer->ProcInfo, &Info->ProcInfo, sizeof(Info->ProcInfo));
			*InfoSize = InfoBufferLen;
		}
	}
	else
		KdPrint(("Null associated buffer\n"));

	return status;
}



NTSTATUS IoControl(PDEVICE_OBJECT Device, PIRP Irp) {
	PIO_STACK_LOCATION IrpIoStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG IoCtl, ClonedInfoSize = 0;
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("IoControl\n"));
	if (IrpIoStack != NULL) {
		IoCtl = IrpIoStack->Parameters.DeviceIoControl.IoControlCode;
		switch (IoCtl) {
		case IOCTL_FILE_LOG:
			status = CloneFileInfo(Irp, IrpIoStack, &ClonedInfoSize);
			break;
		case IOCTL_PROC_LOG:
			break;
		default:
			KdPrint(("default io control operation!!!\n"));
			break;
		}
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = ClonedInfoSize;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}