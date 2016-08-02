#include <iostream>
#include "m6502.h"

using namespace std;

#define tabs "   "
u8* init_machine_mem(m6502* m);
void print_regs(m6502* m);
void print_flags(m6502* m);
void print_machine(m6502* m);
void print_mem(u8* mem);

void step(m6502* m) {
   m->tick();
}


int main() {
   m6502* m = new m6502;
   m->init(false);

   u8* mem = init_machine_mem(m);

   /*mem[0] = 0xa9;
   mem[1] = 0x00;
   mem[2] = 0x20;
   mem[3] = 0x10;*/
   //mem[0] = 0xea;
   //mem[1] = 0xea;
   mem[42] = 0xea;
   mem[43] = 0xea;
   mem[0xfffc] = 42;

   /*print_machine(m);
   m->tick();
   print_machine(m);
   m->tick();
   print_machine(m);*/

   for (int i = 0; i < 15; i++) {
      m->tick();
      print_machine(m);
      print_mem(mem);
   }

   delete[] mem;
   delete m;
}

void print_mem(u8* mem) {
   for (int i = 0; i < 100; i++)
      cout << (int)mem[i];
   cout << endl;
}

u8* init_machine_mem(m6502* m) {
   u8* mem = new u8[64*1000];

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

void print_flags(m6502* m) {
   cout << "flags:" << endl;

   cout << tabs << "flags: " << m->flags <<
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

