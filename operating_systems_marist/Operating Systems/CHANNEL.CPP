
#include "channel.h"

//
// CardReader emulation.
//
ifb_type channelOneDevice::inputSpooling() {
// This routine takes cards from the card reader, and puts them in input full
// buffers. Channel 3 will later take these buffers, and write their contents
// to the drum.

   char tmp[41] = "";
   char num[5] = "";
   char* str_ptr = NULL;
   static long pid;
   static pagetype pin;
   //PCB pcbObj;
   long padBuffer = -1;
   ABuffer *addBuffer = NULL;

   // Read a card, only if there are cards left to read.
   if ( cardSet.reader_empty() == FALSE ) {
      strcpy( tmp , cardSet.get_card() );

      // Determine the type of card that has been read, and gather the
      // pertinent info from each card, based on the type.
      if( tmp[0] != '\0' ) {
         if( !strncmp( tmp , "$JOB", 4) ) {
            strcpy( num , "" );
            strncat( num, &tmp[4], 4);
            pid = atoi( num );
            pin = PROGRAM;

            // If there are available empty buffers, then get one from the
            // empty buffer pool.
            if( inBuffer.querybuffers( ) ){
               addBuffer = inBuffer.GetEmptyBuffer();
               addBuffer->setdata( tmp );
               addBuffer->setpid( pid );
               addBuffer->setpagetype( pin );
               inBuffer.PutInputBuffer( addBuffer );
               return IFB_MADE;   // we created an input full buffer
            }
         }
         else if( !strncmp( tmp, "$DTA", 4 )  ) {
            pin = INPUT;
            return NO_IFB_MADE;    // We did not create an input full buffer
         }
         else if( !strncmp( tmp, "$REM", 4 ) ){
             // Ignore comment cards
             return NO_IFB_MADE;    // We did not create an input full buffer
         }
         else if( !strncmp( tmp, "$EOJ", 4) ) {
            if( inBuffer.querybuffers( ) ) {
               addBuffer = inBuffer.GetEmptyBuffer();
               addBuffer->setdata( tmp );
               addBuffer->setpid( pid );
               addBuffer->setpagetype( pin );
               inBuffer.PutInputBuffer( addBuffer );
               return EOJ_IFB_MADE;   // we created an input full buffer
            }

         }
         else {
            // The card has either program code or data. Although the card
            // may be upto 40 characters, it doesn't have to be. If it is
            // less, than pad the missing characters with blanks.
            padBuffer = strlen( tmp );
             str_ptr = &tmp[padBuffer];

            for( long i = padBuffer; i <= 40; i++ )
               *str_ptr++ = ' ';

            tmp[40] = '\0';

            // If there are empty buffers, get one, and fill the info from
            // the card into the empty buffer. Put it on the input full buffer
            // queue.
            if( inBuffer.querybuffers( ) ){
               addBuffer = inBuffer.GetEmptyBuffer();

               if( addBuffer != NULL ){
                  addBuffer->setdata( tmp );
                  addBuffer->setpid( pid );
                  addBuffer->setpagetype( pin );
                  inBuffer.PutInputBuffer( addBuffer );
                  return IFB_MADE;   // we created an input full buffer
               }
            }
         }
      } // end if
   }

   return NO_IFB_MADE;
}


boolean channelOneDevice::channelOneTimerUp()
{
  if( channelOneTimer >= 4 )
    return TRUE;
  else
    return FALSE;
}


void channelTwoDevice::outputSpooling() {
// This routine takes an output full buffer, and transfers it's contents to
// the printer

   char  buffer[41];
   ABuffer *outputStr;


   if (lprinter.printer_ready()) {

      outputStr = outBuffer.GetOutputBuffer();

      // Ask the printer to print it.
      if( outputStr != NULL ){
         strcpy(buffer, outputStr->getBufStr());
	 lprinter.print( buffer );
	 //lprinter.print("\n");
	 //lprinter.print("\n");
	 outBuffer.PutEmptyBuffer( outputStr );

	 }
   }
}


boolean channelTwoDevice::channelTwoTimerUp()
{
  if( channelTwoTimer >= 4 )
    return TRUE;
  else
    return FALSE;
}

long channelThreeDevice::inputSpooling() {
// This routine takes an input full buufer, and puts it's data onto the drum.
// If the card is an EOJ, then all the cards are now on the drum; return the
// PID so that the PCB can be created. Otherwise, return zero.

   ABuffer *buf = bufPool.GetInputBuffer();

   // Only perform input spooling if there are input full buffers
   if( buf != NULL ) {

      // If the buffer contains an EOJ card, we are ready to create the PCB,
      // and put the job on the ready queue, Return the PID. All the necessary
      // cards are now on the drum.
      if( !strncmp( buf->getBufStr(), "$EOJ" , 4) ) {
         bufPool.PutEmptyBuffer( buf );
         return buf->getProcessId();
      }
      else {
         // Put the buffer's data on the drum
         long rc = fsys.storepage( buf->getBufStr(), buf->getPageType(),
                                   buf->getProcessId() );

         // If successful, return the input full buffer to the empty buffer
         // pool.
         if( rc == 0 ){
            bufPool.PutEmptyBuffer( buf );

            }
      }
   }

   return 0;
}

boolean channelThreeDevice::outputSpooling( long pid ) {
// This routine takes output data from the drum, and puts it into output full
// buffers. These buffers will then be used by channel two to write to the
// printer.

     char buffer[41];
     pagetype pt;
     long rc;
     boolean  retvalue;
     ABuffer *addOutBuffer;


     // If buffers are available, do output spooling
     if( bufPool.querybuffers( ) ) {

        rc = fsys.getpage( buffer, OUTPUT, pid );

        // Only do output spooling if there cards on the drum.
        if (!rc) {
           addOutBuffer = bufPool.GetEmptyBuffer();
           addOutBuffer->setdata( buffer );
           addOutBuffer->setpagetype( OUTPUT );
           addOutBuffer->setpid( pid );
           bufPool.PutOutputBuffer( addOutBuffer );
           retvalue = TRUE;
        }
        else
           // There are no more output cards n the drum. We are done output
           // spooling for this process.
           retvalue = FALSE;
     }

     return retvalue;
}


long channelThreeDevice::getDataService( char *outData, long pid ,
                                         pagetype pageT, long cardnumber ) {
//
// channelThreeDevice will be asked to get pages
// out of the filesystem object.
//  o page fault get program card.
//  o page fault get data card.
//  o page fault get swapped page
//   ( In this how or do we need to distinguish between page Types ? )

     strcpy( outData, "");

     switch(pageT) {
        case PROGRAM:
           return (fsys.getprogpage(outData, cardnumber, pid));
        case INPUT:
           return( fsys.getpage( outData, pageT, pid ) );
     }
}


long channelThreeDevice::putDataService( char *inData, long pid,
                                         pagetype pageT ) {
//
// channelThreeDevice will be asked to put the
// the following pages into the filesystem object
//  o output data
//  o swapped page's data.


   return( fsys.storepage( inData, pageT, pid ) );
}


long channelThreeDevice::getSwapPage( char *outData, long pagenum, long pid) {
//
// channelThreeDevice will be asked to get swap pages
// out of the filesystem object.

   return( fsys.getswappage( outData, pagenum, pid ) );
}


long channelThreeDevice::putSwapPage( char *inData, long pagenum, long pid) {
//
// channelThreeDevice will be asked to put swap
// pages into the filesystem object

   return( fsys.storeswappage( inData, pagenum, pid ) );
}


boolean channelThreeDevice::channelThreeTimerUp()
{
  if( channelThreeTimer >= 2 )
    return TRUE;
  else
    return FALSE;
}

