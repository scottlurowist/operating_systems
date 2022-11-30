#include "globals.h"
#include "pcb.h"


void PCB::dec_IC() {
// This method decrements the two digit IC counter, which is BCD

   two_byte  ms_byte;    // The most significant byte
   two_byte  ls_byte;    // The least significant byte


   ms_byte = IC;            // Get the ms_byte
   ms_byte = ms_byte >> 8;

   ls_byte = IC & 0xFF;

   if ((ms_byte == 0) && (ls_byte == 0))
      IC = 0x0909;
   else if (ls_byte == 0)
      // Subtract one from the tens digit, and reset the ls_byte to nine
      IC = (IC - 0x0100) | 0x0009;
   else
      IC = IC - 1;
}
