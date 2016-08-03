#define pack_ __attribute__((packed))

#define tabs "   "
#define MEMORY_SIZE (64*1000)
#define EXTRACT_BIT(n, i) ((n & (1 << i)) >> i)

u8* init_machine_mem(m6502* m);
void print_regs(m6502* m);
void print_flags(m6502* m);
void print_machine(m6502* m);
void print_mem(u8* mem, uint start = 0, uint end = 100);
int load_nes(string path, u8* mem, u8 address = 42);

//http://wiki.nesdev.com/w/index.php/INES
typedef struct nes_header_t {
   u8 prg_rom_size;
   u8 chr_rom_size;
   union {
      u8 flags6_b;
      struct {
         bool arrange_type1 : 1;
         bool have_trainer : 1;
         bool has_persist : 1;
         u8 lower_nybble_mapper_num : 4;
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
         u8 upper_nybble_mapper_num : 4;
      } pack_ flags7;
   };
   u8 rpg_ram_size;
   union {
      u8 flags9_b;
      struct {} pack_ flags9;
   };
   union {
      u8 flags10_b;
      struct {} pack_ flags10;
   };
} pack_ nes_header_t;


