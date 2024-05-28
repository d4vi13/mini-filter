#include "amaterasu.h"

const FLT_OPERATION_REGISTRATION Callbacks[] = {
    {IRP_MJ_CREATE,0,AmaterasuDefaultPreCallback,NULL},
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


/*
 *  CreatePort() - Creates a communication port for inter-process
 *                 communication.
 *
 *  @sd: Security descriptor.
 *  @PortName: Name of the communication port.
 *  @Port: Pointer to a FLT_PORT structure that will receive the created
 *         port handle.
 *
 *  @ConnectCallback: Callback routine to be called when a client connects
 *                    to the port.
 *
 *  @DisconnectCallback: Callback routine to be called when a client
 *                       disconnects from the port.
 *
 *  @MessageCallback: Callback routine to be called when a message is
 *                    received on the port.
 *
 *  @MaxConnections: Maximum number of concurrent connections allowed on the
 *                   port.
 *
 *  Return:
 *    - 'STATUS_SUCCESS' on success;
 *    - The appropriate 'NTSTATUS' is returned to indicate failure.
 */
static NTSTATUS CreatePort(
    _In_ PSECURITY_DESCRIPTOR Sd,
    _In_ PCWSTR PortName,
    _In_ PFLT_PORT* Port,
    _In_ PFLT_CONNECT_NOTIFY ConnectCallback,
    _In_ PFLT_DISCONNECT_NOTIFY DisconnectCallback,
    _In_ PFLT_MESSAGE_NOTIFY MessageCallback,
    _In_ LONG MaxConnections
) {

    //NTSTATUS Status;
    //UNICODE_STRING UniString;
    //OBJECT_ATTRIBUTES ObjAttr;

    //RtlInitUnicodeString(&UniString, PortName);
    //InitializeObjectAttributes(
    //    &ObjAttr,
    //    &UniString,
    //    /*
    //     *  In Windows, object names are typically case insensitive, so
    //     *  'OBJ_CASE_INSENSITIVE' ensures consistency in how the port name is
    //     *  accessed and referenced.
    //     *
    //     *  'OBJ_KERNEL_HANDLE' specifies that the handle created for the
    //     *  communication port is intended for kernel-mode usage. This flag
    //     *  designates the handle for kernel-mode operations, ensuring it's
    //     *  managed appropriately within the kernel environment.
    //     */
    //    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
    //    NULL,
    //    sd
    //);

    //Status = FltCreateCommunicationPort(
    //    Amaterasu.FilterHandle,
    //    Port
    //    & ObjAttr,
    //    NULL,
    //    ConnectCallback,
    //    DisconnectCallback,
    //    MessageCallback,
    //    MaxConnections
    //);

    //return Status;
}

/*
 *  AmaterasuOpenIPCs() - Opens communication ports for the Amaterasu filter
 *                        driver.
 *
 *  'AmaterasuOpenIPCs()' opens the communication ports needed to communicate
 *  with user mode applications. More specifically, this function opens
 *  'Amaterasu.ServerPort'.
 *
 *  Besides that, 'AmaterasuOpenIPCs()' is essentially just a wrapper to
 *  'CreatePort()', serving as a unified interface for port creation,
 *  facilitating the expansion to open other communication ports if needed.
 *
 *  Return:
 *    - 'STATUS_SUCCESS' if the communication ports are successfully opened.
 *    - An appropriate 'NTSTATUS' error code if an error occurs.
 */
static NTSTATUS AmaterasuOpenIPCs(void) {

    NTSTATUS Status;
    PSECURITY_DESCRIPTOR Sd;

    /*
     *  Build a default security descriptor for the filter port with all access
     *  rights.
     *
     *  A security descriptor contains security information associated with a
     *  securable object, defining access control settings, such as permissions
     *  granted to users and groups, ownership information, etc.
     *
     *  The security descriptor, in this function, is used to create
     *  communication ports.
     */
    //Status = FltBuildDefaultSecurityDescriptor(&Sd, FLT_PORT_ALL_ACCESS);
    //if (!NT_SUCCESS(Status)) {
    //    return Status;
    //}

    //Status = CreateIPC(
    //    Sd,
    //    AMATERASU_SERVER_PORT,
    //    &Amaterasu.ServerPort,
    //    AmaterasuConnect,
    //    AmaterasuDisconnect,
    //    AmaterasuMessage,
    //    1
    //);

    //if (!NT_SUCCESS(Status)) {
    //    FltFreeSecurityDescriptor(sd);
    //    return Status;
    //}

    ///*
    // *  After all the communication ports have been created
    // *  the security descriptor is no longer needed.
    // */
    //FltFreeSecurityDescriptor(sd);

    //return Status;
}

/*
 *  AmaterasuInit() -
 *
 *  @DriverObject:
 *  @RegistryPath:
 *
 *  Return:
 *    -
 *    -
 */
static NTSTATUS AmaterasuInit(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

    NTSTATUS Status = STATUS_SUCCESS;

    Amaterasu.InfoList = InfoListGet(NonPagedPool, MAX_RECORDS);
    if (!Amaterasu.InfoList) {
        return STATUS_UNSUCCESSFUL;
    }

    //Status = OpenPorts();
    //if (!NT_SUCCESS(Status)) {
    //    return Status;
    //}

    Amaterasu.DriverObject = DriverObject;

    return Status;
}


NTSTATUS Create(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    return STATUS_SUCCESS;
}

NTSTATUS Close(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    return STATUS_SUCCESS;
}

NTSTATUS DeviceSetup(PDRIVER_OBJECT DriverObject) {
    NTSTATUS Status;
    PDEVICE_OBJECT device;
    UNICODE_STRING driverName, deviceSymLink;

    RtlInitUnicodeString(&driverName, DRIVER_NAME);
    RtlInitUnicodeString(&deviceSymLink, DEVICE_SYM_LINK);

    Status = IoCreateDevice(DriverObject, 0, &driverName,
        FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
        FALSE, &device);

    if (!NT_SUCCESS(Status))
        return Status;

    Status = IoCreateSymbolicLink(&deviceSymLink, &driverName);

    if (!NT_SUCCESS(Status))
        return Status;

    DriverObject->DeviceObject = device;

    return Status;
}

NTSTATUS DispatchRoutinesSetup(PDRIVER_OBJECT DriverObject){
    //Dispatch routines needed for driver handle creation
    DriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;

    //Dispatch routines to use ioctl communication
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

    return STATUS_SUCCESS;
}

/*
 *	DriverEntry() - Initializes the driver upon loading into memory.
 *
 *	@DriverObject: Pointer to the driver object created by the I/O manager.
 *	@RegistryPath: Path to the driver's registry key in the registry.
 *
 *  Return:
 *    - 'STATUS_SUCCESS' on success.
 *    - The appropriate 'NTSTATUS' is returned to indicate failure.
 */
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

    NTSTATUS Status;

    KdPrint(("driverentry\n"));

    /* Makes non-paged pools (kernel pools) allocations non-executable. */
    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    Status = FltRegisterFilter(DriverObject, &FilterRegistration, &Amaterasu.FilterHandle);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = AmaterasuInit(DriverObject, RegistryPath);
    if (!NT_SUCCESS(Status)) {
        FltUnregisterFilter(Amaterasu.FilterHandle);
        return Status;
    }
    
    Status = DeviceSetup(DriverObject);
    if (!NT_SUCCESS(Status)) {
        FltUnregisterFilter(Amaterasu.FilterHandle);
        return Status;
    }

    Status = DispatchRoutinesSetup(DriverObject);
    if (!NT_SUCCESS(Status)) {
        FltUnregisterFilter(Amaterasu.FilterHandle);
        return Status;
    }

    /*
     *  'FltStartFiltering()' notifies the Filter Manager that 'Amaterasu' is
     *  ready to begin attaching to volumes and filtering I/O requests.
     */
    Status = FltStartFiltering(Amaterasu.FilterHandle);
    if (!NT_SUCCESS(Status)) {
        //AmaterasuCleanup();
        FltUnregisterFilter(Amaterasu.FilterHandle);
    }

    return Status;
}