//rest of project is public domain right now,
//the main.cpp and main.h are under GPLv2+ until we
//decide what to do with it

#include "m6502.h"
#include "main.h"
#include "mapper.h"
#include "ppu.h"

ppu_t* ppu = nullptr;

int main(int ac, char** av) {
   m6502* m = new m6502;
   m->init(false);
   
   ppu = new ppu_t;
   ppu->init();

   mem = new u8[MEMORY_SIZE];
   memset(mem, 0, MEMORY_SIZE);

   //0x8000
   if (ac < 2) {
      printf("usage: %s rom-path\n", av[0]);
      return -1;
   }

   string prog_path = av[1];
   int load_stat = load_nes(prog_path, mem);
   if (load_stat != 0) {
      cout << "failed to load program: " << load_stat << endl;
      exit(1);
   }

   init_machine_mem(m, mem);

   int i = 0;
   for (; i <= MAX_CYCLES; i++) {
      if (m->reset && i > 7) break;

      m->tick();
      for(int j = 0; j <= 3; j++)
      {
            ppu->tick();
      }
      if (i % FREQ_DUMP == 0) {
         print_machine(m);
         print_mem(mem);
      }
   }

   if (i >= MAX_CYCLES)
      cout << "Number of CPU cycles reached the limit!";
   if (m->reset)
      cout << "reset!!";

   cout << " Terminating execution at " << i
        << "==============\n";

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

   n = 0;
   nes_header_t* nes_header = (nes_header_t*)prog_code;
   n += sizeof(nes_header_t);

   if (nes_header->flags6.have_trainer) {
      n += 512;
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

   u16 mapper = lower | (upper << 4);
   u8 submapper = 0;

   u16 prg_rom_size_num = nes_header->prg_rom_size;
   u16 chr_rom_size_num = nes_header->chr_rom_size;
   if (nes_header->flags7.is_nes2 == 2) {
      prg_rom_size_num |= (nes_header->flags9.high_prg_rom_size << 8);
      chr_rom_size_num |=  (nes_header->flags9.high_chr_rom_size << 8);
   }

   if(nes_header->flags7.is_nes2 == 2)
   {
      mapper |= nes_header->flags8.nybble2_mapper_num << 8;
      submapper = nes_header->flags8.submapper_num;
   }

   cout << "mapper num combined: " << (uint)mapper << endl;

   uint prg_rom_size = 16384 * prg_rom_size_num;
   if (prg_rom_size == 0)
      prg_rom_size = 8192;
   uint chr_rom_size = 8192 * chr_rom_size_num;
   mapper_init(mapper, submapper, prog_code + n, prg_rom_size, chr_rom_size);

   if (nes_header->flags7.play_choice10)
      return 4; //TODO: read INST-ROM and PROM

   cout << "size of nes_header_t (should be 16): "
      << sizeof(nes_header_t) << endl;

   cout << "PRG rom size: "
      << (int)nes_header->prg_rom_size << endl
      << "CHR rom size: " << (int)nes_header->chr_rom_size << endl;

   return 0;
}

void print_mem(u8* mem, uint start, uint end) {
   for (int i = start; i < end; i++)
      cout << (int)mem[i];
   cout << endl;
}

void check_mem(u16 addr, u8 val, bool is_write) {
   //used for looking at how the prg ram and rom being used
   //and stepping through every one of them

   const char* action = is_write ? "write" : "read";
   bool print_end = false;
   if (addr >= 0x6000 && addr <= 0x7fff) {
      if (PRINT_PRG_RAM_ACCESS)
         cout << "!!!!!!!!!!!!!!! PRG RAM RAM";
      if (PAUSE_PRG_RAM_ACCESS)
         print_end = true;
   }
   /*if (addr >= 0x8000 && addr <= 0xbfff) {
      if (PRINT_ROM_ACCESS_START)
         cout << "!!!!!!!!!!!!!! first 16KB ROM";
      if (PAUSE_ROM_ACCESS_START)
         print_end = true;
   }
   if (addr >= 0xc000 && addr <= 0xffff) {
      if (PRINT_ROM_ACCESS_END)
         cout << "!!!!!!!!!! last 16kb of ROM" << endl;
      if (PAUSE_ROM_ACCESS_END)
         print_end = true;
   }*/
   if (addr >= 0x2000 && addr <= 0x3fff) {
      if (PRINT_PPU_RAM_ACCESS) {
         printf("!!!!!!!!! gpu register %s: (addr: %i) (addr hex: %x) (val: %i)",
                action, addr, addr, val);
      }
      if (PAUSE_PPU_RAM_ACCESS && ppu->ppustate == PPU_NORMAL)
         print_end = true;
   }
   if (addr >= 0x4000 && addr <= 0x4017) { //0x400f
      if (PRINT_SOUND_ACCESS)
         printf("!!!!! sound and/or input\n");
      if (PAUSE_SOUND_ACCESS)
         print_end = true;
   }
   if (print_end) {
      string i; cin >> i;
   }
   cout << endl;

}

void init_machine_mem(m6502* m, u8* mem) {

   auto rb = [mem, ppu](u16 addr) -> u8 {
      cout << "rb (" << (int)mem[addr]
           << ") at " << std::hex << addr << endl;
      check_mem(addr, mem[addr], false);

      if(addr < 0x2000)
         return mem[addr & 0x7ff];
      else if (addr >= 0x2000 && addr <= 0x3fff)
         return ppu->rb(addr);
      else if(addr >= 0x5000)
         return globmapper.prg_rb(addr);
   };
   auto wb = [mem, ppu](u16 addr, u8 val) {
      cout << "wb (" << (int)val << ") at "
           << std::hex << addr << endl;
      check_mem(addr, val, true);
      if(addr < 0x2000)
         mem[addr & 0x7ff] = val;
      else if (addr >= 0x2000 && addr <= 0x3fff)
         ppu->wb(addr, val);
      else if(addr >= 0x5000)
         globmapper.prg_wb(addr,val);
   };
   m->rb = rb;
   m->wb = wb;
}

void print_regs(m6502* m) {
   cout << "registers:" << endl;

   printf("\ta: %02x\tx: %02x\ty: %02x\ts: %02x\n\tpc: %04x\n", m->a, m->x, m->y, m->s, m->pc);
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

