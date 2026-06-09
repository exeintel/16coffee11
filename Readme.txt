================================================================================
  16coffee11 v0.1 - 16-bit Executable Launcher for Windows 11
  Developer: ExEintel
================================================================================

DESCRIPTION
-----------
16coffee11 is a command-line utility for Windows 11 that detects, inspects,
and launches 16-bit executables (NE format or DOS programs). The tool consists
of a CLI frontend (16coffee11.exe) and a dynamic-link library (16coffee11.dll)
that provides the core detection and launch logic.

SYSTEM REQUIREMENTS
-------------------
- Operating System: Windows 10 / 11 (64-bit)
- Compiler: MinGW-w64 (gcc) with Windows SDK headers
- Runtime: None (statically linked CRT recommended)

Note: Windows 11 64-bit does not include NTVDM. To actually run 16-bit
programs, you may need a third-party emulator such as:
  - otvdm (WineVDM)  - https://github.com/otya128/winevdm
  - DOSBox           - https://www.dosbox.com

16coffee11 performs detection and launch delegation; runtime compatibility
depends on the presence of a 16-bit subsystem or emulator.

BUILD INSTRUCTIONS
------------------
Prerequisites: MinGW-w64 (gcc) must be in your PATH.

  1. Open a command prompt in the project root directory.
  2. Run the build script:

         build.bat

     This will compile:
       - 16coffee11.dll  (shared library)
       - 16coffee11.exe  (command-line launcher)

Manual build steps:

  Step 1: Build DLL
    gcc -shared -o 16coffee11.dll sourse\lib.c ^
        -luser32 -lshell32 ^
        -Wl,--out-implib,lib16coffee11.a

  Step 2: Build EXE
    gcc -o 16coffee11.exe sourse\main.c -L. -l16coffee11

USAGE
-----
  16coffee11.exe {option} [file path]

  Options:

    --start <file>
        Detects whether <file> is a 16-bit executable and attempts to launch it.

    --startas <file>
        Same as --start, but requests elevation (administrator privileges)
        via the UAC prompt.

    --view <file>
        Analyzes <file> and prints its executable type, subsystem, and
        compatibility information.

    --help
        Displays the help message with all available options and usage.

    --version
        Displays the program name, version number, and author.

EXAMPLES
--------
  16coffee11.exe --version
  16coffee11.exe --help
  16coffee11.exe --start C:\DOS\APP.EXE
  16coffee11.exe --startas game.exe
  16coffee11.exe --view unknown.exe

EXIT CODES
----------
  0   Success
  1   Error (file not found, not a 16-bit executable, or launch failure)

TECHNICAL DETAILS
-----------------
16-bit executable detection algorithm:

  1. Open the file and read the first 2 bytes.
  2. If the signature is "MZ" (0x5A4D), it is a DOS/Windows executable.
  3. Read the 4-byte e_lfanew field at file offset 0x3C. This points to the
     extended header.
  4. At the extended header offset, read the 2-byte signature:
       - "NE" (0x454E)  -> 16-bit Windows New Executable
       - "PE" (0x4550)  -> 32/64-bit Portable Executable
       - "LE" / "LX"    -> VxD / OS/2 (not supported)
  5. If no extended header exists, the file is a pure DOS MZ executable
     (16-bit real mode).
  6. Files with the ".COM" extension are also recognized as 16-bit DOS
     programs.

The DLL exports the following functions:

  const char* GetProgramVersion(void);
  BOOL        Is16BitExecutable(const char* path);
  BOOL        IsAdministrator(void);
  int         RunProgram(const char* path);
  int         RunProgramAsAdmin(const char* path);
  int         ViewInfo(const char* path);

These functions can be consumed by any program that links against
16coffee11.dll or loads it at runtime.

FILES
-----
  16coffee11.exe     Command-line launcher
  16coffee11.dll     Core library (executable analysis and launch)
  sourse\            Source code directory
  sourse\main.c      CLI front-end source
  sourse\lib.c       Library source
  Readme.txt         This file

VERSION HISTORY
---------------
  0.1  - Initial release
         - 16-bit executable detection (MZ/NE/DOS/COM)
         - Launch via ShellExecute
         - Elevated launch (runas)
         - File inspection and reporting

================================================================================
