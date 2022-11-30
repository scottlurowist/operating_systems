// File Frametab.h
// Scott Lurowist
//
// This file contains declarations for a frame table class that is used
// by a MOS for managing physical memory.


#ifndef FRAMETAB_H
#define FRAMETAB_H


#include <string.h>
#include "globals.h"


class framedata {
   friend class frametable;

   private:
      long      processid;
      boolean   allocated;
   public:
      framedata() {allocated = FALSE;};
};


class frametable {

   protected:
      long         num_of_frames;   // The number of frames
      long         free_frames;     // The number of freee frames available
      long         next_free_frame; // Pointer to next free frame to be
				   // allocated
      framedata*  frames;          // An array of frame data structs

   public:
      // Convert a 3 digit BCD number to it's decimal equivalent
      word dec_to_bcd(word addr);
      word bcd_to_dec(word addr);

   public:
      // Constructor takes the amount of memory the user desires
      frametable(long amount_of_frames, long framesize);
      ~frametable() {delete [] frames;};
      boolean allocate_frame(long &frame_number, long pid);
      long get_free_frames() const {return free_frames;};
      void deallocate_frame(long frame_number);
      void deallocate_process(long pid);
};


#endif
