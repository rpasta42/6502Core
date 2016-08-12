#include "ppu.h"

void ppu_t::tick()
{
   //The PPU is in a special powerup state for about 29,658 clocks after power on.
   if(ppustate == PPU_POWERUP)
   {
       ppucycles++;
       if(ppucycles == 3)
       {
           ppucycles = 0;
           cpucycles++;
           if(cpucycles == 29658)
           {
               cpucycles = 0;
               ppustate = PPU_NORMAL;
           }
       }
   }
}