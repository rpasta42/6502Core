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

typedef struct nes_header_t {
   u8 rpg_rom_size;
   u8 chr_rom_size;
   union {
      u8 flags6_b;
   }
   union {
      u8 flags7_b;
   }
   u8 rpg_ram_size;
   u8 flags9;
   u8 flags10;
};


