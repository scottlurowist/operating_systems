// File Frametab.cpp
// Scott Lurowist
//
// This file implements the memory class


#include "Frametab.h"


word frametable::dec_to_bcd(word addr) {
// Converts a decimal number to it's 2 digit BCD equivalent

   long  dec_equiv;   // converted number
   long  ms_byte;     // most significant byte
   long  ls_byte;     // least significant byte


   // To pick off each digit, divide by the digits placeholder. The quotient is
   // the digit, the remainder becomes the new dividend for the next quotient
   // remainder. Shift each digit to it's proper byte, then add. For example,
   // 234 /100 = 2 r 34. Two is the hundreds digit. Divide 34 by 10; 3 r 4.
   // Shift the 2 left 16 bits to put it in the most significant byte. Shift
   // the 3 8 bits. Now add each byte for the converted number.
   ms_byte = addr / 10;
   ls_byte = addr % 10;

   ms_byte = ms_byte << 8;

   dec_equiv = ms_byte + ls_byte;

   return dec_equiv;
}


word frametable::bcd_to_dec(word addr) {

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


frametable::frametable(long amount_of_frames, long framesize) {
   long    i;


   num_of_frames = amount_of_frames;

   // Allows the amount of frames to be specified at instantiation time.
   frames = new framedata[num_of_frames];

   // Set the number of free frames, and the free frame pointer equal to the
   // first frame.
   free_frames = num_of_frames;
   next_free_frame = 0;
}


boolean frametable::allocate_frame(long& frame_number, long pid) {
// This routine allocates a single frame of memory if available


   // If the number of free frames is zero, then all of physical memory is
   // allocated, otherwise allocate a frame.
   if (free_frames == 0)
      return FALSE;
   else {

      // The next_free_frame pointer points to the next frame to allocate. Go
      // to this frame data struct, get the number of the frame, and mark it
      // allocated and store the id of the process that it is allocated to.
      // Decrement the free frames counter, since one less free frame is
      // available for allocation.
      frame_number = dec_to_bcd(next_free_frame);
      frames[next_free_frame].processid = pid;
      frames[next_free_frame].allocated = TRUE;
      free_frames--;


      // If the number of free frames is zero, then memory is full. It doesn't
      // make sense to look for the next available free frame. If the number
      // of free frames is non-zero, then look for it. If the end of the array
      // is reached, then reset the pointer to the beginning of the array.
      if (free_frames != 0) {

	 do {
	    next_free_frame++;

	    if (next_free_frame == num_of_frames)
	       next_free_frame = -1;

	 } while ((frames[next_free_frame].allocated) ||
		  (next_free_frame == -1));
      }

      return TRUE;
   }
}


void frametable::deallocate_frame(long frame_number) {
// This routine deallocates a single frame of memory

   // Point the next free frame pointer to the frame just deallocated.
   next_free_frame = bcd_to_dec(frame_number);
   // Mark the frame of interest as FREE, and delete it's process identifier
   frames[next_free_frame].allocated = FALSE;
   frames[next_free_frame].processid = 0;

   // Increment the free frames counter since a frame is now available.
   free_frames++;
}


void frametable::deallocate_process(long pid) {
// This routine deallocates all the frames for a given process


   // Search the entire frame data table, and mark free any frame owned by
   // the process pid. Also delete it's processid, and increment the free
   // frames counter. Make next_free_frame point to the frame just
   // deallocated.
   for (long i = 0; i < num_of_frames; i++) {
      if (pid == frames[i].processid) {
	 frames[i].allocated = FALSE;
	 frames[i].processid = 0;

	 free_frames++;
	 next_free_frame = i;
      }
   }
}

