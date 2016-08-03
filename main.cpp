//rest of project is public domain right now,
//the main.cpp and main.h are under GPLv2+ until we
//decide what to do with it

#include <iostream>
#include <cstring>
#include "m6502.h"
#include "main.h"

int main() {
   m6502* m = new m6502;
   m->init(false);

   u8* mem = new u8[MEMORY_SIZE];
   memset(mem, 0, MEMORY_SIZE);

   string prog_path = "../mario/SuperMarioBros.nes";
   int load_stat = load_nes(prog_path, mem);
   if (load_stat != 0) {
      cout << "failed to load program: " << load_stat << endl;
      exit(1);
   }

   init_machine_mem(m, mem);

   int i = 0;
   for (; i < 1000000; i++) {
      if (m->reset && i > 7) break;

      m->tick();
      if (i % FREQ_DUMP == 0) {
         print_machine(m);
         print_mem(mem);
      }
   }

   if (i >= 999998)
      cout << "Number of CPU cycles reached the limit!";
   if (m->reset)
      cout << "reset!!";

   cout << " Terminating execution at " << i << "==============\n";

   print_machine(m);
   print_mem(mem);

   delete[] mem;
   delete m;
}


//TODO: check for errors in read_file and other places
//TODO: add print_nes_header() function
//0 = success, 1 = bad magic, 2 = header 11-15 not zero filled
//3 = trainer specified, but we don't support it
//4 = PlayChoice 10 specified, but we don't support it
int load_nes(string path, u8* mem, u8 address) {
   int length = 0;
   u8 *prog_code, *p;
   prog_code = p = read_file(path, &length);
   u8 n = 0;

   //last one is DOS new-line
   if (p[n++] != 'N' || p[n++] != 'E' || p[n++] != 'S' || p[n++] != 0x1a)
      return 1;

   n = 10;
   while (n <= 15) {
      if (p[n++] != 0)
         return 2;
   }

   p += 4;
   nes_header_t* nes_header = (nes_header_t*)prog_code;
   p += sizeof(nes_header_t);

   if (nes_header->flags6.have_trainer) {
      p += 512;
      return 3;
   }

   u8 lower = nes_header->flags6.nybble0_mapper_num;
   u8 upper = nes_header->flags7.nybble1_mapper_num;
   cout
      << "is NES 2.0: " << (uint)nes_header->flags7.is_nes2 << endl
      << "lower nybble mapper num: " << (uint)lower << endl
      << "upper nybble mapper num: " << (uint)upper << endl
      << "flags6 combined: "
      << or_flags_to_str(nes_header->flags6_b, 8) << endl
      << "flags7 combined: "
      << or_flags_to_str(nes_header->flags7_b, 8) << endl;

   u16 mapper = upper | (lower << 4);
   u8 submapper = 0;

   u16 prg_rom_size_num = (nes_header->flags7.is_nes2 == 2) ? (nes_header->prg_rom_size | (nes_header->flags9.high_prg_rom_size << 8)) : nes_header->prg_rom_size;
   u16 chr_rom_size_num = (nes_header->flags7.is_nes2 == 2) ? (nes_header->chr_rom_size | (nes_header->flags9.high_chr_rom_size << 8)) : nes_header->chr_rom_size;

   if(nes_header->flags7.is_nes2 == 2)
   {
      mapper |= nes_header->flags8.nybble2_mapper_num << 8;
      submapper = nes_header->flags8.submapper_num;
   }

   cout << "mapper num combined: " << (uint)mapper << endl;

   uint prg_rom_size = 16384 * prg_rom_size_num;
   if (prg_rom_size == 0)
      prg_rom_size = 8192;
   u8* prg_rom = new u8[prg_rom_size];
   memcpy(prg_rom, p, prg_rom_size);
   //This function will be implemented eventually to handle mapper-specific ROM banking and mirroring.
   //mapper_init(mapper, submapper, mem, p, prg_rom_size);
   memcpy(mem + 0x8000, p, prg_rom_size);
   p += prg_rom_size;

   uint chr_rom_size = 8192 * nes_header->chr_rom_size;
   u8* chr_rom = new u8[chr_rom_size];
   memcpy(chr_rom, p, chr_rom_size);
   p += chr_rom_size;

   if (nes_header->flags7.play_choice10)
      return 4; //TODO: read INST-ROM and PROM

   char* title_c_str = new char[128];
   memcpy(title_c_str, p + (length - 1 - 127), 127);
   title_c_str[127] = '\0';

   string title = string(title_c_str);

   cout << "size of nes_header_t (should be 16): "
      << sizeof(nes_header_t) << endl;

   cout << "PRG rom size:"
      << (int)nes_header->prg_rom_size << endl
      << "CHR rom size" << (int)nes_header->chr_rom_size
      << endl  << "title: " << title << endl;

   //return 7;
   return 0;
}

void print_mem(u8* mem, uint start, uint end) {
   for (int i = start; i < end; i++)
      cout << (int)mem[i];
   cout << endl;
}

void check_mem(u16 addr, bool is_write) {
   //used for looking at how the rpg ram and rom being used
   //and stepping through every one of them

   bool print_end = false;
   if (addr >= 0x6000 && addr <= 0x7fff) {
      cout << "!!!!!!!!!!!!!!! PRG RAM RAM";
      print_end = true;
   }
   if (addr >= 0x8000 && addr <= 0xbfff) {
      cout << "!!!!!!!!!!!!!! first 16KB ROM";
      print_end = true;
   }
   if (addr >= 0xc000 && addr <= 0xffff) {
      cout << "!!!!!!!!!! last 16kb of ROM" << endl;
   }
   if (addr >= 0x2000 && addr <= 0x3fff) {
      printf("!!!!!!!!! gpu register\n");
      print_end = true;
   }
   if (addr >= 0x4000 && addr <= 0x400f) {
      printf("!!!!! sound\n");
      print_end = true;
   }
   if (print_end) {
      cout << endl;
      string i; cin >> i;
   }
}

u16 translate_addr(u16 addr) {
   //uncomment if rpg rom size == 2
   //if (addr >= 0xc000 && addr <= 0xffff) return addr - 0xc000 + 0x8000;
   return addr;
}

void init_machine_mem(m6502* m, u8* mem) {
   auto rb = [mem](u16 addr) -> u8 {
      cout << "rb (" << (int)mem[addr]
           << ") at " << std::hex << addr << endl;
      check_mem(addr, false);
      return mem[translate_addr(addr)];
   };
   auto wb = [mem](u16 addr, u8 val) {
      cout << "wb (" << (int)val << ") at "
           << std::hex << addr << endl;
      check_mem(addr, true);
      mem[translate_addr(addr)] = val;
   };
   m->rb = rb;
   m->wb = wb;
}

void print_regs(m6502* m) {
   cout << "registers:" << endl;

   cout << tabs << "a: " << m->a <<
           tabs << "x: " << m->x <<
           tabs << "y: " << m->y <<
           tabs << "s: " << m->s << endl <<
           tabs << "pc: " << m->pc << endl;
}

string or_flags_to_str(uint n, uint len) {
   string s = "";
   for (uint i = 0; i < len; i++) {
      uint x = EXTRACT_BIT(n, i); //(n & (1 << i)) >> i;
      s += to_string(x);
   }
   return s;
}

void print_flags(m6502* m) {
   cout << "flags:" << endl;

   cout << tabs << "flags: " << or_flags_to_str(m->flags, 8) <<
           tabs << "reset: " << m->reset <<
           tabs << "sync: " << m->sync <<
           tabs << "irq: " << m->irq <<
           tabs << "nmi: " << m->nmi <<
           tabs << "rdy: " << m->rdy <<
           tabs << "so: " << m->so << endl <<
           tabs << "bcd: " << m->bcd << endl <<
           tabs << "execing: " << m->execing << endl <<
           tabs << "phi2: " << m->phi2 << endl;
}

void print_machine(m6502* m) {
   cout << "---------------------\n";
   print_flags(m);
   print_regs(m);

   cout << "misc:" << endl
        << tabs << "cycle: " << m->cycle << endl
        << tabs << "cycle type: "
        << (m->cycletype == Read ? "read" : "write")
        << endl << endl;
}

