#ifndef PPU_H_INCLUDE
#define PPU_H_INCLUDE

#include "common.h"

struct ppu_t {
   u8 regs[8];

   u8 rb(u16 addr) {

      printf("******ppu rb at %x\n", addr);
      string s; cin >> s;

      //if (addr == 0x2002) return 255;

      addr &= 0x7;

      return regs[addr];
   }

   void wb(u16 addr, u8 val) {
      printf("******ppu wb (at %x): %i\n", addr, val);
      string s; cin >> s;
      
      addr &= 0x7;
      
      regs[addr] = val;
   }

};

#endif //PPU_H_INCLUDE
