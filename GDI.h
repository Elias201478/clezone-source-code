#pragma once
#include <Windows.h>


DWORD WINAPI tHSL(LPVOID lpThread);
DWORD WINAPI warp(LPVOID lpThread);
DWORD WINAPI GDI_One(LPVOID lpThread);
DWORD WINAPI GDI_Two(LPVOID lpThread);
DWORD WINAPI GDI_Three(LPVOID lpThread);
DWORD WINAPI GDI_Four(LPVOID lpThread);
DWORD WINAPI GDI_Five(LPVOID lpThread);
DWORD WINAPI ShiftXW(LPVOID lpThread);
DWORD WINAPI GDI_Seven(LPVOID lpThread);