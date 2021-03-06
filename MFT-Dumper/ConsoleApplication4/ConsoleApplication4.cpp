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

	// Open a read and write handle to the volume
	HANDLE hVolume;
	hVolume = CreateFile(_T("\\\\.\\C:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hVolume != INVALID_HANDLE_VALUE) {
		printf("Succeeded to open volume.\n\n");

		int j;

		HANDLE outFile;
		outFile = CreateFile(_T("\\\\.\\C:\\mft.out"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// For the whole MFT-size (retrieved with ntfsinfo - Cluster Size * Pos-value) read and write the result to a file
		for (j = 0; j < 1967915008; j = j + 8192) {

			DWORD num;
			DWORD num2;

			char out[8192] = { 0 };

			LARGE_INTEGER frAddr;
			frAddr.QuadPart = 0xC0000000 + j; //512 = 200
			frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

			//printf("Pos: %d\n\n", frAddr.LowPart);

			ReadFile(hVolume, &out, 8192, &num, NULL);
			frAddr.LowPart = SetFilePointer(hVolume, frAddr.LowPart, &frAddr.HighPart, FILE_BEGIN);

			WriteFile(outFile, &out, 8192, &num2, NULL);

			//int i;

			//for (i = 0; i < 8192; ++i)
			//	printf("%c", ((char *)out)[i]);
		}
	}
	else {
		printf("Failed");
	}
	printf("Done copying MFT");
	std::cin.get();
	return 0;
}

