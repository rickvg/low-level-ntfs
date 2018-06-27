#include <ntddk.h>
#include <wdf.h>
#include <stdio.h>


DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfDriver1EvtDeviceAdd;

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT     DriverObject,
	_In_ PUNICODE_STRING    RegistryPath
)
{
	// NTSTATUS variable to record success or failure
	NTSTATUS status = STATUS_SUCCESS;

	// Allocate the driver configuration object
	WDF_DRIVER_CONFIG config;

	// Print "Hello World" for DriverEntry
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: DriverEntry\n"));

	// Initialize the driver configuration object to register the
	// entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
	WDF_DRIVER_CONFIG_INIT(&config,
		KmdfDriver1EvtDeviceAdd
	);

	// Finally, create the driver object
	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		WDF_NO_OBJECT_ATTRIBUTES,
		&config,
		WDF_NO_HANDLE
	);
	return status;
}

NTSTATUS
KmdfDriver1EvtDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{
	// We're not using the driver object,
	// so we need to mark it as unreferenced
	UNREFERENCED_PARAMETER(Driver);

	NTSTATUS status;

	// Allocate the device object
	WDFDEVICE hDevice;

	// Print "Hello World"
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));

	// Create the device object
	status = WdfDeviceCreate(&DeviceInit,
		WDF_NO_OBJECT_ATTRIBUTES,
		&hDevice
	);


	// Start of read and write code
	UNICODE_STRING     uniName;
	OBJECT_ATTRIBUTES  objAttr;

	RtlInitUnicodeString(&uniName, L"\\Device\\Harddisk0\\Partition2"); 
	InitializeObjectAttributes(&objAttr, &uniName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	HANDLE   handle;
	NTSTATUS ntstatus;
	NTSTATUS ntstatus3;
	IO_STATUS_BLOCK    ioStatusBlock;

	// Driver must be in PASSIVE LEVEL, otherwise IRPs must be constructed.
	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
		return STATUS_INVALID_DEVICE_STATE;

	#define  BUFFER_SIZE 0x200
	CHAR     buffer[BUFFER_SIZE];

	// Open volume and read contents
	ntstatus3 = ZwOpenFile
	(&handle,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatusBlock,
		FILE_SHARE_VALID_FLAGS,
		FILE_SYNCHRONOUS_IO_NONALERT);


	FILE_POSITION_INFORMATION position;
	position.CurrentByteOffset.QuadPart = 0xC0000000;
	status = ZwSetInformationFile(handle,
		&ioStatusBlock,
		&position,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation);

	LARGE_INTEGER frAddr;
	frAddr.QuadPart = 0xC0000000; //512 = 200

	ntstatus = ZwReadFile(handle, 0, 0, 0, &ioStatusBlock, buffer, sizeof(buffer), &frAddr, 0);
	ZwClose(handle);


	// ---------------------------------------------------------------------------
	// Print read contents to file.
	UNICODE_STRING     uniName2;
	OBJECT_ATTRIBUTES  objAttr2;

	RtlInitUnicodeString(&uniName2, L"\\DosDevices\\C:\\output.txt");  // or L"\\SystemRoot\\example.txt"
	InitializeObjectAttributes(&objAttr2, &uniName2,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	HANDLE   handle2;
	NTSTATUS ntstatus2;
	IO_STATUS_BLOCK    ioStatusBlock2;

	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
		return STATUS_INVALID_DEVICE_STATE;

	ntstatus2 = ZwCreateFile(&handle2,
		GENERIC_WRITE,
		&objAttr2, &ioStatusBlock2, NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);

#define  BUFFER_SIZE2 0x200
	CHAR     buffer2[BUFFER_SIZE2];
	sprintf(buffer2, "hello%d", ntstatus);

	ntstatus = ZwWriteFile(handle2, NULL, NULL, NULL, &ioStatusBlock2,
		buffer, BUFFER_SIZE2, NULL, NULL);
	ZwClose(handle2);


	// ----------------------------------------------------------------------------

	return status;
}