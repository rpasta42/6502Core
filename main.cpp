#include <iostream>
#include <fstream>
#include <cstring>
#include "m6502.h"
#include "main.h"

int main() {
   m6502* m = new m6502;
   m->init(false);

   u8* mem = init_machine_mem(m);

   /*mem[0] = 0xa9;
   mem[1] = 0x00;
   mem[2] = 0x20;
   mem[3] = 0x10;
   mem[0] = 0xea;
   mem[1] = 0xea;*/
   mem[42] = 0xea;
   mem[43] = 0xea;
   mem[0xfffc] = 42;

   /*print_machine(m);
   m->tick();
   print_machine(m);
   m->tick();
   print_machine(m);*/

   string prog_path = "../mario/SuperMarioBros.nes";
   int load_stat = load_nes(prog_path, mem);
   if (load_stat != 0) {
      cout << "failed to load program: " << load_stat << endl;
      exit(1);
   }

   /*for (int i = 0; i < 15; i++) {
      m->tick();
      print_machine(m);
      print_mem(mem);
   }*/
   int i = 0;
   for (; i < 100000; i++) {
      if (m->reset && i > 7) break;

      m->tick();
      if (i % 50 == 0) {
         print_machine(m);
         print_mem(mem);
      }
   }

   if (i >= 99998)
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
//0 = success, 1 = bad magic, 2 = header 11-15 not zero filled
int load_nes(string path, u8* mem, u8 address) {
   int length = 0;
   u8 *prog_code, *p;
   prog_code = p = read_file(path, &length);

   u8 n = 0;

   //last one is DOS new-line
   if (p[n++] != 'N' || p[n++] != 'E' || p[n++] != 'S' || p[n++] != 0x1a)
      return 1;

   u8 rpg_rom_size = p[n++]; //p[4]
   u8 chr_rom_size = p[n++];
   u8 flags6 = p[n++];
   u8 flags7 = p[n++];
   u8 rpg_ram_size = p[n++]; //p[8]
   u8 flags9 = p[n++];
   u8 flags10 = p[n++]; //p[10]

   while (n <= 15) {
      if (p[n++] != 0)
         return 2;
   }

   memcpy(mem, prog_code + 3, length - 3);
   delete prog_code;

   return 0;
}

void print_mem(u8* mem, uint start, uint end) {
   for (int i = start; i < end; i++)
      cout << (int)mem[i];
   cout << endl;
}

u8* init_machine_mem(m6502* m) {
   u8* mem = new u8[MEMORY_SIZE];

   memset(mem, 0, MEMORY_SIZE);

   auto rb = [mem](u16 addr) -> u8 {
      cout << "rb (" << (int)mem[addr]
           << ") at " << addr << endl;
      return mem[addr];
   };
   auto wb = [mem](u16 addr, u8 val) {
      cout << "wb (" << (int)val << ") at "
           << addr << endl;
      mem[addr] = val;
   };
   m->rb = rb;
   m->wb = wb;

   return mem;
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

