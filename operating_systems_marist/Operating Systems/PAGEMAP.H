// File Pagemap.h
// Scott Lurowist
//
// This file contains declarations for a frame table class that is used
// by a MOS for managing physical memory.


#ifndef PAGEMAP_H
#define PAGEMAP_H


#include <string.h>
#include "globals.h"
#include "primem.h"


class pagemap {

   protected:
      primarymemory*  pm;          // Pointer to primary memory

   public:
      // Convert a 3 digit BCD number to it's decimal equivalent
      word dec_to_bcd(word addr);

   public:
      // Constructor takes the amount of memory the user desires
      pagemap(primarymemory* primem) {pm = primem;};
      ~pagemap() {};
      long page_to_frame(word ptr, page pagenumber, frame_addr& frame_base);
};


#endif
