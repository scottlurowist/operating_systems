// File Memory.cpp
// Scott Lurowist
//
// This file implements the memory class

#include <iostream.h>
#include "primem.h"


word primarymemory::bcd_to_dec(word addr) {

   long  dec_equiv;   // converted number
   long  ms_byte;     // most significant byte
   long  mid_byte;    // middle byte
   long  ls_byte;     // least significant byte

   ms_byte = addr;
   ms_byte = ms_byte >> 16;   // Get the MSB

   mid_byte = addr;
   mid_byte = mid_byte & 0x0000FF00;    // Mask off the middle byte
   mid_byte = mid_byte >> 8;            // Shift it

   ls_byte = addr & 0x000000FF;         // Mask off the LSB

   dec_equiv = (100 * ms_byte) + (10 * mid_byte) + ls_byte;

   return dec_equiv;
}


primarymemory::primarymemory(long sizeofmem) {
   size = sizeofmem;

   // Allows the amount of memory to be specified at instantiation time. The
   // amount of memory is specified by sizeofmem
   cells = new memorycell[sizeofmem];
}


void primarymemory::write_cell(long address, long data) {

   address = bcd_to_dec(address);

   // Ignore invalid addresses
   if ((address < 0) || (address > (size - 1))) {
      cout << "Invalid memory write address " << address << endl;
      return;
   }
   else
      cells[address].write(data);
}


long primarymemory::read_cell(long address) {

   address = bcd_to_dec(address);

   // Ignore invalid addresses
   if ((address < 0) || (address > (size - 1))) {
      cout << "Invalid memory read address  " << address << endl;
      return 0;
   }
   else
      return cells[address].read();
}


void primarymemory::flush_cells(long frame_number, long amount) {
// This routine clears (flushes) amount cells

   long address;

   address = bcd_to_dec(frame_number);

   // Ignore invalid addresses
   if ((address < 0) || (address > (size - 1))) {
      cout << "Invalid memory write address " << address << endl;
      return;
   }
   for (long i = 0; i < amount; i++)
      cells[address + 1].flush();
}


void primarymemory::flush_memory() {
   delete [] cells;
   cells = new memorycell[size];
}


