#include "common.h"
#ifndef IOCTL_H
#define IOCTL_H

#define SystemBuffer(Irp) Irp->AssociatedIrp.SystemBuffer
#define OutputBufferLength(IrpIoStack) IrpIoStack->Parameters.DeviceIoControl.OutputBufferLength

#define IOCTL_GET_INFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

extern NTSTATUS InfoClone(
	PIRP Irp,
	PIO_STACK_LOCATION IrpIoStack,
	PULONG InfoSize
);

NTSTATUS IoControl(PDEVICE_OBJECT Device, PIRP Irp);

#endif