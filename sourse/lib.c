#include <windows.h>
#include <stdio.h>
#include <string.h>

#define VERSION "0.1"
#define APP_NAME "16coffee11"

__declspec(dllexport) const char* GetProgramVersion(void)
{
    return VERSION;
}

static BOOL Is16BitFile(const char* path)
{
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    WORD mz;
    DWORD read;
    BOOL result = FALSE;

    if (!ReadFile(hFile, &mz, sizeof(mz), &read, NULL) || read != sizeof(mz))
    {
        CloseHandle(hFile);
        return FALSE;
    }

    if (mz != IMAGE_DOS_SIGNATURE)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    DWORD e_lfanew;
    SetFilePointer(hFile, 0x3C, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, &e_lfanew, sizeof(e_lfanew), &read, NULL) || read != sizeof(e_lfanew))
    {
        CloseHandle(hFile);
        return TRUE;
    }

    if (e_lfanew == 0)
    {
        CloseHandle(hFile);
        return TRUE;
    }

    WORD sig;
    SetFilePointer(hFile, e_lfanew, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, &sig, sizeof(sig), &read, NULL) || read != sizeof(sig))
    {
        CloseHandle(hFile);
        return TRUE;
    }

    if (sig == 0x454E)
        result = TRUE;

    CloseHandle(hFile);
    return result;
}

static BOOL IsDOSExt(const char* path)
{
    const char* ext = strrchr(path, '.');
    if (ext && (stricmp(ext, ".com") == 0))
        return TRUE;
    return FALSE;
}

__declspec(dllexport) BOOL Is16BitExecutable(const char* path)
{
    if (IsDOSExt(path))
        return TRUE;
    return Is16BitFile(path);
}

__declspec(dllexport) BOOL IsAdministrator(void)
{
    BOOL elevated = FALSE;
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        TOKEN_ELEVATION te;
        DWORD size = sizeof(te);
        if (GetTokenInformation(hToken, TokenElevation, &te, size, &size))
            elevated = te.TokenIsElevated;
        CloseHandle(hToken);
    }

    return elevated;
}

__declspec(dllexport) int RunProgram(const char* path)
{
    if (!Is16BitExecutable(path))
    {
        printf("%s: '%s' is not a 16-bit executable.\n", APP_NAME, path);
        return 1;
    }

    HINSTANCE ret = ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOW);
    if ((INT_PTR)ret <= 32)
    {
        printf("%s: failed to launch '%s' (error code: %d).\n", APP_NAME, path, (INT_PTR)ret);
        return 1;
    }

    printf("%s: launched '%s'.\n", APP_NAME, path);
    return 0;
}

__declspec(dllexport) int RunProgramAsAdmin(const char* path)
{
    if (!Is16BitExecutable(path))
    {
        printf("%s: '%s' is not a 16-bit executable.\n", APP_NAME, path);
        return 1;
    }

    HINSTANCE ret = ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOW);
    if ((INT_PTR)ret <= 32)
    {
        printf("%s: failed to launch '%s' as administrator (error code: %d).\n",
               APP_NAME, path, (INT_PTR)ret);
        return 1;
    }

    printf("%s: launched '%s' as administrator.\n", APP_NAME, path);
    return 0;
}

__declspec(dllexport) int ViewInfo(const char* path)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fad))
    {
        printf("%s: cannot access '%s'.\n", APP_NAME, path);
        return 1;
    }

    printf("=== File Information ===\n");
    printf("  Path: %s\n", path);

    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("  Status: Cannot open for analysis\n");
        return 1;
    }

    if (IsDOSExt(path))
    {
        printf("  Executable: Yes (DOS .COM)\n");
        printf("  Subtype: DOS .COM (16-bit)\n");
        printf("  Can run with %s: Yes\n", APP_NAME);
        CloseHandle(hFile);
        return 0;
    }

    WORD mz;
    DWORD read;
    if (!ReadFile(hFile, &mz, sizeof(mz), &read, NULL) || read != sizeof(mz))
    {
        printf("  Executable: No (too small)\n");
        printf("  Can run with %s: No\n", APP_NAME);
        CloseHandle(hFile);
        return 1;
    }

    if (mz != IMAGE_DOS_SIGNATURE)
    {
        printf("  Executable: No (not an MZ executable)\n");
        printf("  Can run with %s: No\n", APP_NAME);
        CloseHandle(hFile);
        return 1;
    }

    DWORD e_lfanew;
    SetFilePointer(hFile, 0x3C, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, &e_lfanew, sizeof(e_lfanew), &read, NULL) || read != sizeof(e_lfanew))
    {
        printf("  Executable: Yes (DOS executable)\n");
        printf("  Subtype: Pure DOS (16-bit)\n");
        printf("  Can run with %s: Yes\n", APP_NAME);
        CloseHandle(hFile);
        return 0;
    }

    if (e_lfanew == 0)
    {
        printf("  Executable: Yes\n");
        printf("  Subtype: Pure DOS MZ (16-bit)\n");
        printf("  Can run with %s: Yes\n", APP_NAME);
        CloseHandle(hFile);
        return 0;
    }

    WORD sig;
    SetFilePointer(hFile, e_lfanew, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, &sig, sizeof(sig), &read, NULL) || read != sizeof(sig))
    {
        printf("  Executable: Yes (DOS executable)\n");
        printf("  Subtype: Pure DOS (16-bit)\n");
        printf("  Can run with %s: Yes\n", APP_NAME);
        CloseHandle(hFile);
        return 0;
    }

    if (sig == 0x454E)
    {
        printf("  Executable: Yes\n");
        printf("  Subtype: Windows NE (New Executable) - 16-bit\n");
        printf("  Can run with %s: Yes\n", APP_NAME);
    }
    else if (sig == 0x4550)
    {
        printf("  Executable: Yes\n");
        printf("  Subtype: Windows PE (Portable Executable) - 32/64-bit\n");
        printf("  Can run with %s: No (not a 16-bit executable)\n", APP_NAME);
    }
    else
    {
        printf("  Executable: Yes\n");
        printf("  Subtype: Unknown (signature: 0x%04X)\n", sig);
        printf("  Can run with %s: No\n", APP_NAME);
    }

    CloseHandle(hFile);

    return 0;
}
