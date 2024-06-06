#include "amaterasu.h"

static PINFO InfoListRemoveTail(_Inout_ PINFO_LIST InfoList) {

	PINFO Info;
	KIRQL OldIrql;
	PLIST_ENTRY Entry;

	Info = NULL;
	KeAcquireSpinLock(&InfoList->Lock, &OldIrql);

	if (InfoList->RecordsAllocated) {
		InfoList->RecordsAllocated--;

		Entry = RemoveTailList(&InfoList->Head);
		Info = CONTAINING_RECORD(Entry, INFO, ListEntry);
	}

	KeReleaseSpinLock(&InfoList->Lock, OldIrql);

	return Info;
}


static inline NTSTATUS CloneSetup(
	PIRP Irp,
	PIO_STACK_LOCATION IrpIoStack,
	PINFO* Info,
	PINFO_STATIC* InfoBuffer,
	PULONG InfoBufferLen
) {

	if (IsListEmpty(&Amaterasu.InfoList->Head))
		return STATUS_UNSUCCESSFUL;

	*Info = InfoListRemoveTail(Amaterasu.InfoList);
	*InfoBuffer = SystemBuffer(Irp);
	*InfoBufferLen = OutputBufferLength(IrpIoStack);

	return STATUS_SUCCESS;
}

NTSTATUS InfoClone(PIRP Irp, PIO_STACK_LOCATION IrpIoStack, PULONG InfoSize) {
	NTSTATUS status = STATUS_SUCCESS;
	PINFO Info;
	PINFO_STATIC InfoBuffer;
	ULONG InfoBufferLen;

	CloneSetup(Irp,IrpIoStack, &Info, &InfoBuffer, &InfoBufferLen);

	if ((InfoBuffer != NULL) && (sizeof(Info) <= InfoBufferLen)) {
		InfoCopy(InfoBuffer, Info);
		*InfoSize = InfoBufferLen;
	}else {
		*InfoSize = 0;
		KdPrint(("Null associated buffer\n"));
	}

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
		case IOCTL_GET_INFO:
			status = InfoClone(Irp, IrpIoStack, &ClonedInfoSize);
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