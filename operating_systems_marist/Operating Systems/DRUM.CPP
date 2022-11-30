//FILE DRUM.CPP
//Dominic Bruen

#include <string.h>
#include "drum.h"

int drum::WriteTrack(int tracknumber, byte *data)
{
     if(tracknumber-1 < 0) return(INVALIDTRACKNUMBER);       //check for invalid track number
     if(tracknumber-1 > NUMBEROFTRACKS) return(INVALIDTRACKNUMBER);
     if(tracks[tracknumber-1].isallocated()==FALSE) return(TRACKNOTALLOCATED);

     tracks[tracknumber-1].Write(data);
     return(0);
}

int drum::ReadTrack(int tracknumber, byte* trackdata)
{

     if( (tracknumber-1 < 0) || (tracknumber-1 > NUMBEROFTRACKS))  //check for invaid ot un-allocated track
        return(INVALIDTRACKNUMBER);
     if(tracks[tracknumber-1].isallocated()==FALSE)
            return(TRACKNOTALLOCATED);

     tracks[tracknumber-1].Read(trackdata);
     return(0);
}

int drum::AllocateTrack(void)
{
   for(int i=0; i<NUMBEROFTRACKS; i++)
      if(tracks[i].isallocated()==FALSE) {
         tracks[i].allocate();
         return(i+1);
      }

   return(OUTOFTRACKS); //no tracks are available
}

int drum::DeallocateTrack(int tracknumber)
{
     if(tracknumber-1 < 0) return(INVALIDTRACKNUMBER);       //check for invalid track number
     if(tracknumber-1 > NUMBEROFTRACKS) return(INVALIDTRACKNUMBER);
     if(tracks[tracknumber-1].isallocated()==FALSE) return(TRACKNOTALLOCATED);

     //deallocate the specified track
     tracks[tracknumber-1].deallocate();
     return(0);
}

int drum::QueryTracks(void)
{
   int count=0;

   for(int i=0; i<NUMBEROFTRACKS; i++)  //iterate through the tracks and count
      if(tracks[i].isallocated()==FALSE)    //up the ones that aren't allocated
         count++;

   return(count);                      //number of free tracks
}
