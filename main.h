#ifndef MAIN_H_INCLUDE
#define MAIN_H_INCLUDE

#include "common.h"

#define pack_ __attribute__((packed))

#define tabs "   "
#define MEMORY_SIZE 0x10000 //(64*1000)
#define MAX_CYCLES 1000000//10000//1000//100 //90000 //1000000
#define EXTRACT_BIT(n, i) ((n & (1 << i)) >> i)
//number of ticks to dump memory, registers and flags
#define FREQ_DUMP 50


//CHECK_MEM STUFF
//This stuff is used in check_mem for debugging
//print when accessing first 16KB of ROM
#define PRINT_ROM_ACCESS_START 1
#define PAUSE_ROM_ACCESS_START 0

#define PRINT_ROM_ACCESS_END 1
#define PAUSE_ROM_ACCESS_END 0//1

#define PRINT_PRG_RAM_ACCESS 1
#define PAUSE_PRG_RAM_ACCESS 0

#define PRINT_PPU_RAM_ACCESS 0//1
#define PAUSE_PPU_RAM_ACCESS 0//1

#define PRINT_SOUND_ACCESS 1
#define PAUSE_SOUND_ACCESS 1
//END CHECK_MEM STUFF

void init_machine_mem(m6502* m, u8* mem);
void print_regs(m6502* m);
void print_flags(m6502* m);
string or_flags_to_str(uint n, uint len);
void print_machine(m6502* m);
void print_mem(u8* mem, uint start = 0, uint end = 100);
int load_nes(string path, u8* mem, u8 address = 42);

//http://wiki.nesdev.com/w/index.php/INES
typedef struct nes_header_t {
   u8 magic[4]; //should be "NES", but in struct empty
   u8 prg_rom_size; //12 bits used
   u8 chr_rom_size; //12 bits used
   union {
      u8 flags6_b;
      struct {
         bool mirroring : 1;
         bool has_battery : 1;
         bool have_trainer : 1;
         bool four_screen : 1;
         u8 nybble0_mapper_num : 4;
      } pack_ flags6;
   };
   union {
      u8 flags7_b;
      struct {
         //if is_nes2 == 2, then flags 8-15 are in NES 2.0 format
         u8 is_nes2 : 2;
         //8kb of hint screen data stored after CHR data
         bool play_choice10 : 1;
         bool vs_unisystem : 1;
         u8 nybble1_mapper_num : 4;
      } pack_ flags7;
   };
   union {
      u8 flags8_b;
      struct {
         u8 nybble2_mapper_num : 4;
         u8 submapper_num : 4;
      } pack_ flags8;
   };
   union {
      u8 flags9_b;
      struct {
         u8 high_prg_rom_size : 4;
         u8 high_chr_rom_size : 4;
      } pack_ flags9;
   };
   union {
      u8 flags10_b;
      struct {
         u8 non_battery_prg_ram_size : 4;
         u8 battery_prg_ram_size : 4;
      } pack_ flags10;
   };
   union {
      u8 flags11_b;
      struct {
         u8 non_battery_chr_ram_size : 4;
         u8 battery_chr_ram_size : 4;
      } pack_ flags11;
   };
   union {
      u8 flags12_b;
      struct {
         u8 pal_mode : 1;
         u8 multi_region : 1;
      } pack_ flags12;
   };
   u8 zeros[3];
} pack_ nes_header_t;


#endif //MAIN_H_INCLUDE
