#include <iostream>
#include <fstream>
#include <cstring>
#include "m6502.h"

using namespace std;

#define tabs "   "
#define MEMORY_SIZE (64*1000)

u8* init_machine_mem(m6502* m);
void print_regs(m6502* m);
void print_flags(m6502* m);
void print_machine(m6502* m);
void print_mem(u8* mem, uint start = 0, uint end = 100);
int load_nes(string path, u8* mem, u8 address = 42);

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
   if (load_nes(prog_path, mem) != 0)
      cout << "failed to load program";

   /*for (int i = 0; i < 15; i++) {
      m->tick();
      print_machine(m);
      print_mem(mem);
   }*/

   for (int i = 0; i < 100000; i++) {
      //if (i > 10 && !m->execing) break;
      cout << "kkkkkkkkkk:" << m->reset;
      if (i > 100) break;
      m->tick();
      if (i % 50 == 0) {
         print_machine(m);
         print_mem(mem);
      }
   }

   print_machine(m);
   print_mem(mem);

   delete[] mem;
   delete m;
}

u8* read_file(string path, int* f_length) {
   ifstream f(path, fstream::binary);

   f.seekg(0, ios::end);
   int length = f.tellg();
   f.seekg(0, ios::beg);

   char* buff = new char[length];
   f.read(buff, length);
   f.close();

   *f_length = length;

   return (u8*)buff;
}

//TODO: check for errors in read_file and other places
int load_nes(string path, u8* mem, u8 address) {
   int length = 0;
   u8 *prog_code, *p;
   prog_code = p = read_file(path, &length);

   if (p[0] != 'N' || p[1] != 'E' || p[2] != 'S')
      return 1;

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
      uint x = (n & (1 << i)) >> i;
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

