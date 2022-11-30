#ifndef DRUM_H
#define DRUM_H

#include "globals.h"
#include "string.h"

#define TRACKSIZE 41
#define NUMBEROFTRACKS 100
#define INVALIDTRACKNUMBER -1
#define TRACKNOTALLOCATED -2
#define OUTOFTRACKS -3

typedef char  byte;

class track {
    private:
      byte trackdata[TRACKSIZE];
      boolean allocated;
    public:
      track() { allocated=FALSE; strcpy(trackdata,""); }
      int isallocated(void) { if(allocated==TRUE) return(TRUE);
                              else return(FALSE); }
      void allocate(void) { allocated=TRUE; }
      void deallocate(void) { allocated=FALSE; }
      void Write(byte *data) { strcpy(trackdata,data);};
      void Read(byte *data) { strcpy(data,trackdata);}
};

class drum {
     private:
       track tracks[NUMBEROFTRACKS];
     public:
       drum() {};
       int WriteTrack(int tracknumber, byte *data);
       int ReadTrack(int tracknumber, byte* data);
       int AllocateTrack(void);
       int DeallocateTrack(int tracknumber);
       int QueryTracks(void);
};
#endif
