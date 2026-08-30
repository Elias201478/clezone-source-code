#pragma once
#include <Windows.h>
typedef unsigned char (*BytebeatFunc)(unsigned int t);

void CALLBACK WaveCallback(HWAVEOUT hwo, UINT msg, DWORD_PTR inst, DWORD_PTR param1, DWORD_PTR param2);
void PlayBytebeat(BytebeatFunc func, int hz);
void StopBytebeat();