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

            if(prg_rom_size == 0x4000)
            {
                memcpy(mem+0x8000, globmapper.prg_rom, 0x4000);
                memcpy(mem+0xc000, globmapper.prg_rom, 0x4000);
            }
            else if(prg_rom_size == 0x8000)
                memcpy(mem+0x8000, globmapper.prg_rom, 0x8000);
            break;
        }    
        default:
        {
            printf("Unknown mapper detected! Game may not run as expected!");
            break;
        }
    }
}

void mapper_quit()
{
    if(globmapper.prg_rom) delete[] globmapper.prg_rom;
}