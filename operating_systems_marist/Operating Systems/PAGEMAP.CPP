// File Pagemap.cpp
// Scott Lurowist
//
// This file implements the memory class


#include "Pagemap.h"



long pagemap::page_to_frame(word ptr, page pagenumber, frame_addr& frame_base) {
// This routine goes to the page table in physical memory, and gets the base
// address in physical memory of the frame pointed to by the page number. It
// also determines whether a page fault has occured, and if so, what is the
// cause.

   // Mask off the page table base address. Since this is BCD, shifting
   // left 8 bits is multiplying by ten.
   ptr = ((ptr & 0x0000FFFF) << 8);

   // Return the frame number. Since the value returned from primary memory
   // is 32 bits, and frame_base is 16, mask off the lowest 2 bytes.
   word fr = pm->read_cell(ptr + pagenumber);
   frame_base = fr & 0x0000FFFF;

   // Check for the cases where there is a page fault. Determine whether the
   // fault is because the memory was not allocated, or the memory was
   // allocated, but the frame was swapped to disk.

   // Mask off the upper two bits
   fr = fr & 0xC0000000;

   if (fr ==  PAGE_INV_NOSWAP)
      return PAGE_INV_NOSWAP;
   else if (fr == PAGE_INV_SWAP)
      return PAGE_INV_SWAP;

   return PAGE_VALID;
}
