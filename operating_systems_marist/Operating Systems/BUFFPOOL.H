//BUFFERPOOL.H
//A class to handle management of the 10 buffers for spooling

//All management of the buffers are handled by this class.
//It is up to the user to return the buffers to the empty buffer
//queue by making calls to PutEmptyBuffer() after they
//is no longer needed for spooling. Also, if a call to GetInputBuffer(),
//GetOutputBuffer(), or GetEmptyBuffer() is made, and no buffers
//are available, NULL is returned, otherwise a pointer to the
//buffer is returned. Buffers are stored in the queues in FIFO order.

#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include "poolque.h"
#include "buffer.h"

#define NUMBEROFBUFFERS 30

class bufferpool {
      private:
         poolqueue *inputfullbuffers;
         poolqueue *outputfullbuffers;
         poolqueue *emptybuffers;

      public:
          bufferpool();
          ~bufferpool();
          boolean querybuffers(void);     //Use to check for an avail. buffer
          boolean query_in_buffers(void);
          boolean query_out_buffers(void);
          void PutInputBuffer(ABuffer *b); //Use to store an IFB  (ch1)
          void PutOutputBuffer(ABuffer *b);//Use to store an OFB  (ch2)
          void PutEmptyBuffer(ABuffer *b); //Use to store an EB  (ch1,ch2)
          ABuffer*  GetEmptyBuffer(void); //Use to get an empty buffer(ch1,ch2)
          ABuffer*  GetInputBuffer(void);  //Use to get the next IFB (ch3)
          ABuffer*  GetOutputBuffer(void); //Use to get the next OFB (ch3)
          ABuffer*  CheckInputBuffer(void);  //Use to check the next IFB (ch3)
};
#endif

// * TO LINK INTO YOUR PROJECT YOU NEED:  *

// LIBRARY FILE: BUFFPOOL.LIB

// HEADER FILES: BUFFPOOL.H
//               BUFFER.H
//               POOLQUE.H
//               GLOBALS.H





