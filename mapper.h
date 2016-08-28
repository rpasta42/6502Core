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

    unsigned prg_rom_size;
    unsigned chr_rom_size;

    function<u8(u16)> prg_rb;
    function<void(u16,u8)> prg_wb;
};

extern mapper_t globmapper;

void mapper_init(u16 mapper, u8 submapper, u8* prog_code, unsigned prg_rom_size, unsigned chr_rom_size);