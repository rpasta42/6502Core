#ifndef PPU_H_INCLUDE
#define PPU_H_INCLUDE

#include "common.h"

struct ppu_t {
   u8 regs[8];
   
   int cpucycles;
   int ppucycles;
   
   enum
   {
      PPU_POWERUP,
      PPU_RESET,
      PPU_NORMAL
   } ppustate;

   void init()
   {
       regs[0] = 0;
       regs[1] = 0;
       regs[2] = 0xa0; //1010 0000
       regs[3] = 0;
       //$2004 OAMDATA is random at reset, apparently.
       regs[5] = 0;
       regs[6] = 0;
       regs[7] = 0;
       
       cpucycles = 0;
       ppucycles = 0;
       
       ppustate = PPU_POWERUP;
   }

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
   
   void tick();

};

#endif //PPU_H_INCLUDE
