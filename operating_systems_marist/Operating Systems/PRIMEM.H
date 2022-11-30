// File Memory.h
// Scott Lurowist
//
// This file contains declarations for a memory object that would
// be used in a simple computing system.

#ifndef MEMORY_H
#define MEMORY_H


#include <string.h>
#include "globals.h"


class memorycell {

   private:
      long  value;

   public:
      memorycell() {value = 0;};
      void write(long data) {value = data;};
      long read() {return value;};
      void flush() {value = 0;};
};


class primarymemory {

   protected:
      long      size;     // Amount of memory cells
      memorycell*  cells;    // An array of individual memory cells

      // Convert a 3 digit BCD number to it's decimal equivalent
      word bcd_to_dec(word addr);

   public:
      // Constructor takes the amount of memory the user desires
      primarymemory(long sizeofmem);
      ~primarymemory() {delete [] cells;};
      void write_cell(long address, long data);
      long read_cell(long address);

      // Returns the amount of memory in the object
      long memory_size() const {return size;};
      void flush_cells(long frame_number, long amount);
      void flush_memory();
};


#endif
