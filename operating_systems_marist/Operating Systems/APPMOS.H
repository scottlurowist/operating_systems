// File appmos.hpp

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include "channel.h"
#include "primem.h"
#include "frametab.h"
#include "filesys.h"
#include "cpu.h"
#include "pagemap.h"
#include "pcbqueue.h"

#ifndef APPMOS_HPP
#define APPMOS_HPP


typedef enum {READYQ, IOQ} queue_type;

class AOperatingSystem{
   private:
      fstream traceFile;
      primarymemory mem;
      moscpu cpu;
      pagemap pmap;
      cardreader card;
      printer lineprinter;
      bufferpool buffers;
      filesystem fSys;
      frametable frameTab;
      PCBqueue readyQue;
      PCBqueue terminateQue;
      PCBqueue ioQue;
      char     emptystring[41];

      // will have a copy of both inputBufferQue, card
      channelOneDevice channel1;

      // will have a copy of both outputBufferQue, printer
      channelTwoDevice channel2;
      channelThreeDevice channel3;

      page  page_number;
      boolean  TERMINATE;
      boolean  OPERAND_FAULT;
      boolean  OPCODE_FAULT;
      boolean  READY_TO_RUN;
      long clockcount;

      // Used in input and output spooling to reserve buffers and frames
      ABuffer *reserved_input_buffer;
      ABuffer *reserved_output_buffer;
      long    reserved_frame;

      boolean isSlaveModeTrue(){ return (boolean )cpu.get_MODE(); };
      boolean startChannelOneDevice();
      boolean startChannelTwoDevice();
      boolean startChannelThreeDevice();

      void initialize();
      void slaveMode();
      void masterMode();
      void simulateDevicesAndTimer();
      boolean simulation_complete();

      void serviceInterrupts();
      void servicePIInterrupt( long intr );
      void serviceIOInterrupt( long intr );
      void serviceTIInterrupt( long intr );
      void serviceSIInterrupt( long intr );
      void channel1_IH();
      void channel3_IH();

      void scheduler(boolean TERMINATE);
      void createProcessPageTable(long pid );
      void pageFaultSwapService();
      void terminationService( long intr, queue_type q );

      void page_to_mem(long frame, char* data);
      void mem_to_page(long frame, char* data);

   public:
     AOperatingSystem( long argc, char **argv );
     ~AOperatingSystem(){ if( traceFile ) traceFile.close(); };
};

#endif
