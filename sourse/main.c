#include <windows.h>
#include <stdio.h>
#include <string.h>

#define APP_NAME "16coffee11"
#define AUTHOR   "ExEintel"

__declspec(dllimport) const char* GetProgramVersion(void);
__declspec(dllimport) BOOL       Is16BitExecutable(const char*);
__declspec(dllimport) int        RunProgram(const char*);
__declspec(dllimport) int        RunProgramAsAdmin(const char*);
__declspec(dllimport) int        ViewInfo(const char*);

static void ShowHelp(void)
{
    printf("%s v%s - 16-bit Executable Launcher for Windows 11\n", APP_NAME, GetProgramVersion());
    printf("Developed by %s\n\n", AUTHOR);
    printf("Usage:\n");
    printf("  %s {option} [file path]\n\n", APP_NAME);
    printf("Options:\n");
    printf("  --start    <file>   Launch a 16-bit executable\n");
    printf("  --startas  <file>   Launch a 16-bit executable as administrator\n");
    printf("  --view     <file>   Display information about the executable\n");
    printf("  --help              Show this help message\n");
    printf("  --version           Show version information\n");
}

static void ShowVersion(void)
{
    printf("%s v%s\n", APP_NAME, GetProgramVersion());
    printf("Author: %s\n", AUTHOR);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        ShowHelp();
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        ShowHelp();
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0)
    {
        ShowVersion();
        return 0;
    }

    if (argc < 3)
    {
        printf("%s: missing file path. Use --help for usage.\n", APP_NAME);
        return 1;
    }

    const char* filePath = argv[2];

    if (strcmp(argv[1], "--start") == 0)
        return RunProgram(filePath);

    if (strcmp(argv[1], "--startas") == 0)
        return RunProgramAsAdmin(filePath);

    if (strcmp(argv[1], "--view") == 0)
        return ViewInfo(filePath);

    printf("%s: unknown option '%s'. Use --help for usage.\n", APP_NAME, argv[1]);
    return 1;
}
