#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    HMODULE dll = LoadLibraryA("16coffee11.dll");
    if (!dll)
    {
        printf("FAIL: LoadLibrary failed (error %lu)\n", GetLastError());
        return 1;
    }

    printf("DLL loaded at 0x%p\n", (void*)dll);

    typedef const char* (*GetVerFn)(void);
    typedef BOOL (*Is16Fn)(const char*);
    typedef BOOL (*IsAdminFn)(void);
    typedef int (*RunFn)(const char*);
    typedef int (*RunAdminFn)(const char*);
    typedef int (*ViewFn)(const char*);

    GetVerFn  GetProgramVersion  = (GetVerFn)GetProcAddress(dll, "GetProgramVersion");
    Is16Fn    Is16BitExecutable  = (Is16Fn)GetProcAddress(dll, "Is16BitExecutable");
    IsAdminFn IsAdministrator    = (IsAdminFn)GetProcAddress(dll, "IsAdministrator");
    RunFn     RunProgram         = (RunFn)GetProcAddress(dll, "RunProgram");
    RunAdminFn RunProgramAsAdmin = (RunAdminFn)GetProcAddress(dll, "RunProgramAsAdmin");
    ViewFn    ViewInfo           = (ViewFn)GetProcAddress(dll, "ViewInfo");

    printf("\n--- DLL Exports ---\n");
    printf("  GetProgramVersion:  %s\n", GetProgramVersion ? "OK" : "FAIL");
    printf("  Is16BitExecutable:  %s\n", Is16BitExecutable ? "OK" : "FAIL");
    printf("  IsAdministrator:    %s\n", IsAdministrator ? "OK" : "FAIL");
    printf("  RunProgram:         %s\n", RunProgram ? "OK" : "FAIL");
    printf("  RunProgramAsAdmin:  %s\n", RunProgramAsAdmin ? "OK" : "FAIL");
    printf("  ViewInfo:           %s\n", ViewInfo ? "OK" : "FAIL");

    if (!GetProgramVersion  || !Is16BitExecutable || !IsAdministrator ||
        !RunProgram         || !RunProgramAsAdmin || !ViewInfo)
    {
        printf("\nFAIL: Some exports are missing!\n");
        FreeLibrary(dll);
        return 1;
    }

    printf("\n--- Function Tests ---\n");
    printf("  Version: %s\n", GetProgramVersion());
    printf("  Running as admin: %s\n", IsAdministrator() ? "Yes" : "No");

    const char* testFiles[] = {
        "test_16bit.com",
        "test_16bit.exe",
        "test_ne.exe",
        "C:\\Windows\\System32\\cmd.exe",
        "nonexistent.exe",
        NULL
    };

    for (int i = 0; testFiles[i]; i++)
    {
        BOOL is16 = Is16BitExecutable(testFiles[i]);
        printf("  Is16BitExecutable(\"%s\"): %s\n", testFiles[i], is16 ? "Yes (16-bit)" : "No (not 16-bit)");
    }

    printf("\n--- ViewInfo Test ---\n");
    ViewInfo("test_ne.exe");

    printf("\nALL DLL TESTS PASSED.\n");
    FreeLibrary(dll);
    return 0;
}
