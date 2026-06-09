#include <stdio.h>
#include <stdint.h>
#include <windows.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t e_magic;     // MZ
    uint16_t e_cblp;      // bytes on last page
    uint16_t e_cp;        // pages
    uint16_t e_crlc;      // relocations
    uint16_t e_cparhdr;   // header size in paragraphs
    uint16_t e_minalloc;  // min extra paras
    uint16_t e_maxalloc;  // max extra paras
    uint16_t e_ss;        // stack seg
    uint16_t e_sp;        // stack ptr
    uint16_t e_csum;      // checksum
    uint16_t e_ip;        // ip
    uint16_t e_cs;        // cs
    uint16_t e_lfarlc;    // reloc offset
    uint16_t e_ovno;      // overlay
    uint16_t e_res[4];    // reserved
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;    // offset to new header
} IMAGE_DOS_HEADER_16;
#pragma pack(pop)

int main()
{
    // --- Create minimal .COM file (RET instruction) ---
    FILE* f = fopen("test_16bit.com", "wb");
    if (f) {
        unsigned char com[] = { 0xC3 }; // RET
        fwrite(com, 1, sizeof(com), f);
        fclose(f);
        printf("Created: test_16bit.com (1 byte, DOS .COM)\n");
    }

    // --- Create minimal MZ .EXE (pure DOS, no NE header) ---
    f = fopen("test_16bit.exe", "wb");
    if (f) {
        IMAGE_DOS_HEADER_16 hdr = { 0 };
        hdr.e_magic   = 0x5A4D;       // "MZ"
        hdr.e_cblp    = 0x0090;
        hdr.e_cp      = 0x0001;
        hdr.e_crlc    = 0x0000;
        hdr.e_cparhdr = 0x0004;       // 64 bytes
        hdr.e_minalloc= 0x0010;
        hdr.e_maxalloc= 0xFFFF;
        hdr.e_ss      = 0x0000;
        hdr.e_sp      = 0x00FE;
        hdr.e_csum    = 0x0000;
        hdr.e_ip      = 0x0000;
        hdr.e_cs      = 0x0000;
        hdr.e_lfarlc  = 0x001C;
        hdr.e_ovno    = 0x0000;
        hdr.e_lfanew  = 0x0000;       // 0 = no extended header -> pure DOS

        fwrite(&hdr, 1, sizeof(hdr), f);
        // Small stub that just returns
        unsigned char stub[] = { 0xB4, 0x4C, 0xCD, 0x21 }; // MOV AH, 0x4C; INT 0x21
        fwrite(stub, 1, sizeof(stub), f);
        fclose(f);
        printf("Created: test_16bit.exe (MZ DOS executable, no NE header)\n");
    }

    // --- Create fake MZ with NE header marker ---
    f = fopen("test_ne.exe", "wb");
    if (f) {
        IMAGE_DOS_HEADER_16 hdr = { 0 };
        hdr.e_magic   = 0x5A4D;       // "MZ"
        hdr.e_cblp    = 0x0090;
        hdr.e_cp      = 0x0001;
        hdr.e_cparhdr = 0x0004;
        hdr.e_lfanew  = 0x0040;       // offset to extended header

        fwrite(&hdr, 1, sizeof(hdr), f);
        // Pad to offset 0x40
        fseek(f, 0x40, SEEK_SET);
        unsigned char ne_sig[] = { 0x4E, 0x45, 0x00, 0x00 }; // "NE\0\0"
        fwrite(ne_sig, 1, sizeof(ne_sig), f);
        fclose(f);
        printf("Created: test_ne.exe (MZ + NE header, 16-bit Windows)\n");
    }

    return 0;
}
