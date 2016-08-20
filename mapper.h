#include "common.h"

extern u8* mem;

struct mapper_t
{
    u16 mapper;
    u8 submapper;

    u8* prg_rom;
    u8* chr_rom;

    u8* prg_ram_nbb; //non-battery-backed
    u8* prg_ram_bb;

    u8* chr_ram_nbb; //non-battery-backed
    u8* chr_ram_bb;
};

extern mapper_t globmapper;

void mapper_init(u16 mapper, u8 submapper, u8* prog_code, unsigned prg_rom_size, unsigned chr_rom_size);