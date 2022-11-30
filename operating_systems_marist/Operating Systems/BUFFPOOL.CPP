//BUFFPOOL.CPP
//Manages pool of buffers for the channels

#include "buffpool.h"

bufferpool::bufferpool()
{
     inputfullbuffers = new poolqueue;
     outputfullbuffers = new poolqueue;
     emptybuffers = new poolqueue;
     ABuffer *b;


     for(long i=0; i<NUMBEROFBUFFERS; i++) {
          b = new ABuffer;
          emptybuffers->insert(b);
    }
}

bufferpool::~bufferpool()
{
     delete inputfullbuffers;
     delete outputfullbuffers;
     delete emptybuffers;
}

boolean bufferpool::querybuffers(void)
{
     if(emptybuffers->front() == NULL)
        return(FALSE);
     else
        return(TRUE);
}

boolean bufferpool::query_in_buffers(void) {
// This routine returns TRUE if there are input full buffers

   if(inputfullbuffers->front() == NULL)
      return(FALSE);
   else
      return(TRUE);
};


boolean bufferpool::query_out_buffers(void) {
// This routine returns TRUE if there are output full buffers

   if(outputfullbuffers->front() == NULL)
      return(FALSE);
   else
      return(TRUE);

};


ABuffer* bufferpool::GetEmptyBuffer(void)
{
     ABuffer *b;

     if(emptybuffers->front() == NULL)
        return(NULL);

     b = emptybuffers->remove();
     return(b);
}


ABuffer* bufferpool::GetInputBuffer(void)
{
     ABuffer *b;

     if(inputfullbuffers->front() == NULL)
        return(NULL);

     b = inputfullbuffers->remove();
     return(b);
}

ABuffer* bufferpool::CheckInputBuffer(void)
{
     ABuffer *b;

     if(inputfullbuffers->front() == NULL)
        return(NULL);

     b = inputfullbuffers->front();
     return(b);
}

ABuffer*  bufferpool::GetOutputBuffer(void)
{
     ABuffer *b;

     if(outputfullbuffers->front() == NULL)
        return(NULL);
     b = outputfullbuffers->remove();
     return(b);
}

void bufferpool::PutInputBuffer(ABuffer *b)
{
      inputfullbuffers->insert(b);
}

void bufferpool::PutOutputBuffer(ABuffer *b)
{
      outputfullbuffers->insert(b);
}

void bufferpool::PutEmptyBuffer(ABuffer *b)
{
      emptybuffers->insert(b);
}
