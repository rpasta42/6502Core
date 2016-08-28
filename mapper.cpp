#include "mapper.h"

u8* mem = nullptr;
mapper_t globmapper;

void mapper_init(u16 mapper, u8 submapper, u8* prog_code, unsigned prg_rom_size, unsigned chr_rom_size)
{
    globmapper.mapper = mapper;
    globmapper.submapper = submapper;

    switch(mapper)
    {
        case 0:
        {
            globmapper.prg_rom = new u8[prg_rom_size];
            globmapper.chr_rom = new u8[chr_rom_size];
            memcpy(globmapper.prg_rom, prog_code, prg_rom_size);
            memcpy(globmapper.chr_rom, prog_code + prg_rom_size, chr_rom_size);
            globmapper.prg_rom_size = prg_rom_size;
            globmapper.chr_rom_size = chr_rom_size;

            globmapper.prg_rb = [&globmapper](u16 addr) -> u8
            {
                if(globmapper.prg_rom_size == 0x4000) return globmapper.prg_rom[addr & 0x3fff];
                else return globmapper.prg_rom[addr & 0x7fff];
            };
            globmapper.prg_wb = [](u16 addr, u8 val)
            {
                printf("NROM game tried to write to ROM! Game may not execute correctly\n");
            };
            break;
        }
        case 2:
        {
            globmapper.prg_rom = new u8[prg_rom_size];
            globmapper.chr_rom = new u8[chr_rom_size];
            memcpy(globmapper.prg_rom, prog_code, prg_rom_size);
            memcpy(globmapper.chr_rom, prog_code + prg_rom_size, chr_rom_size);
            globmapper.prg_rom_size = prg_rom_size;
            globmapper.chr_rom_size = chr_rom_size;

            globmapper.prg_rb = [&globmapper](u16 addr) -> u8
            {
                switch(globmapper.prg_rom_size)
                {
                    case 0x8000:
                    if(addr >= 0xc000) return globmapper.prg_rom[(addr & 0x3fff) + 0x4000];
                    else return globmapper.prg_rom[(addr & 0x3fff) + ((globmapper.prg_rom_bank_uxrom & 0x1) << 14)]; 
                    break;
                    case 0x10000:
                    if(addr >= 0xc000) return globmapper.prg_rom[(addr & 0x3fff) + 0xc000];
                    else return globmapper.prg_rom[(addr & 0x3fff) + ((globmapper.prg_rom_bank_uxrom & 0x3) << 14)]; 
                    break;
                    case 0x20000:
                    if(addr >= 0xc000) return globmapper.prg_rom[(addr & 0x3fff) + 0x1c000];
                    else return globmapper.prg_rom[(addr & 0x3fff) + ((globmapper.prg_rom_bank_uxrom & 0x7) << 14)]; 
                    break;
                    case 0x40000:
                    if(addr >= 0xc000) return globmapper.prg_rom[(addr & 0x3fff) + 0x3c000];
                    else return globmapper.prg_rom[(addr & 0x3fff) + ((globmapper.prg_rom_bank_uxrom & 0xf) << 14)]; 
                    break;
                }
            };
            globmapper.prg_wb = [](u16 addr, u8 val)
            {
                globmapper.prg_rom_bank_uxrom = val;
            };
            break;
        }
        default:
        {
            printf("Unknown mapper detected! Game may not run as expected!\n");
            break;
        }
    }
}

void mapper_quit()
{
    if(globmapper.prg_rom) delete[] globmapper.prg_rom;
}