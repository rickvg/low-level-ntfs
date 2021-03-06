// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

int main()
{
	_TCHAR volumePath[7];
	_sntprintf_s(volumePath, 6, _T("\\\\.\\PhysicalDrive1"));
	//volumePath[6] = _T('\0');

	DWORD FilePos;

	// Open read and write volume to physical drive
	HANDLE hVolume;
	hVolume = CreateFile(_T("\\\\.\\PhysicalDrive0"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hVolume != INVALID_HANDLE_VALUE) {
		printf("Succeeded to open volume.\n\n");
		
		DWORD num;
		char out[4097] = { 0 };
		
		// Initialize position variables.
		LARGE_INTEGER frAddr;
		frAddr.QuadPart = 0x23700000; //512 = 200
		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		printf("Pos: %d\n\n", frAddr.LowPart);

		ReadFile(hVolume, &out, 1024, &num, NULL);
		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		int i;

		// Print initial bytes of physical drive
		for (i = 0; i < 1024; ++i)
			printf("%c", ((char *)out)[i]);
		
		// Bytes to original: \xEB R \x90 N

		out[0] = 'K';
		out[1] = 'R';
		out[2] = '\x90';
		out[3] = 'N';
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

		//If LOCK volume not possible: WriteFile also not possible, unless outside boot sector or outside filesystem space.
		bErrorFlag = WriteFile(hVolume, out, 1024, NULL, NULL);

		if (FALSE == bErrorFlag)
		{
			printf("\n\nUnable to write to file.\n");
		}

		frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

		printf("\n\n\n");

		ReadFile(hVolume, &out, 1024, &num, NULL);

		// Print possibly changed buffer.
		for (i = 0; i < 1024; ++i)
			printf("%c", ((char *)out)[i]);

	}
	else {
		printf("Failed");
	}

	std::cin.get();
    return 0;
}

