// File Channel.hpp

#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "buffpool.h"
#include "primem.h"
#include "frametab.h"
#include "filesys.h"
#include "cardread.h"
#include "printer.h"
#include "pcbqueue.h"

#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

class channelOneDevice{
      private:
        bufferpool &inBuffer;
        cardreader &cardSet;
        long channelOneTimer;
        fstream &traceFile;
      public:
         channelOneDevice( bufferpool &inputBuffer , cardreader &card,
                                  fstream &tracker):
         inBuffer( inputBuffer ),
         cardSet( card ),
         channelOneTimer( 0 ),
         traceFile(tracker){};
         ~channelOneDevice(){};
         void setChannelOneTimer( long inT ){ channelOneTimer = inT; };
         long getChannelOneTimer(){ return channelOneTimer; };
         void incrementChannelOneTimer(){ channelOneTimer++; };
         boolean reader_empty() { return cardSet.reader_empty(); };
         boolean channelOneTimerUp();
         ifb_type inputSpooling();
};
class channelTwoDevice{
     private:
        bufferpool &outBuffer;
        printer &lprinter;
        long channelTwoTimer;
        fstream &traceFile;
     public:
         channelTwoDevice( bufferpool &outputBuffer, printer &prnt,
                           fstream &tracker):
         outBuffer( outputBuffer ),
         lprinter( prnt ),
         channelTwoTimer( 0 ),
         traceFile(tracker){};
         ~channelTwoDevice(){ };
         void setChannelTwoTimer( long inT ){ channelTwoTimer = inT; };
         long getChannelTwoTimer(){ return channelTwoTimer; };
         void incrementChannelTwoTimer(){ channelTwoTimer++; };
         boolean channelTwoTimerUp();
         void outputSpooling();

         boolean query_out_buffers(void)
            { return outBuffer.query_out_buffers();};
};

enum serviceType{ None, InSpool, OutSpool, GetData, PutData, LoadService,
		  GetSwap, PutSwap, PutGetSwap  };

class channelThreeDevice{
    private:
      bufferpool &bufPool;
      filesystem &fsys;
      long channelThreeTimer;
      serviceType currServRequest;
      fstream &traceFile;
    public:
      channelThreeDevice( bufferpool &allBuffer, filesystem &filesys,
                          fstream &tracker):
      bufPool( allBuffer ),
      fsys( filesys ),
      channelThreeTimer( 0 ),
      currServRequest( None ),
      traceFile(tracker){};
      ~channelThreeDevice(){ };
      long getDataService( char *outData, long pid, pagetype pageT,
                           long carnumber);
      long putDataService( char *inData,  long pid, pagetype pageT );
      long getSwapPage( char *outData, long pagenum, long pid);
      long putSwapPage( char *outData, long pagenum, long pid);
      long inputSpooling();
      boolean outputSpooling( long pid );

      void setChannelThreeTimer( long inT ){ channelThreeTimer = inT; };
      long getChannelThreeTimer(){ return channelThreeTimer; };
      void incrementChannelThreeTimer(){ channelThreeTimer++; };
      boolean channelThreeTimerUp();
      serviceType getCurrentService(){ return currServRequest; };
      void setCurrentService( serviceType serv ){ currServRequest = serv; };

      boolean query_in_buffers(void) { return bufPool.query_in_buffers();};
};
#endif
