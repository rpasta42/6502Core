#include <iostream>
#include "m6502.h"

using namespace std;

#define tabs "   "

void print_regs(m6502* m) {
   cout << "registers:" << endl;

   cout << tabs << "a: " << m->a <<
           tabs << "x: " << m->x <<
           tabs << "y: " << m->y <<
           tabs << "s: " << m->s << endl;
}

void print_flags(m6502* m) {
   cout << "flags:" << endl;

   cout << tabs << "reset: " << m->reset <<
           tabs << "sync: " << m->sync << endl <<
           tabs << "irq: " << m->irq << endl <<
           tabs << "nmi: " << m->nmi << endl <<
           tabs << "rdy: " << m->rdy << endl <<
           tabs << "so: " << m->so << endl <<
           tabs << "bcd: " << m->bcd << endl <<
           tabs << "execing: " << m->execing << endl <<
           tabs << "phi2: " << m->phi2 << endl;
}


void print_machine(m6502* m) {
   print_flags(m);
   print_regs(m);

   cout << "cycle: " << m->cycle << endl
        << "cycle type: "
        << (m->cycletype == Read ? "read" : "write")
        << endl << endl;
}


int main() {
   m6502* m = new m6502;
   m->init(true);

   u8* mem = new u8[64*1000];

   auto rb = [mem](u16 addr) -> u8 {
      return mem[addr];
   };
   auto wb = [mem](u16 addr, u8 val) {
      mem[addr] = val;
   };
   m->rb = rb;
   m->wb = wb;

   print_machine(m);
   m->tick();
   print_machine(m);
   m->tick();
   print_machine(m);

   delete[] mem;
   delete m;
}

