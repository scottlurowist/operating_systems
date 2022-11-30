//PCB.H
//Process Control Block object
#ifndef PCB_H
#define PCB_H

#include "cpu.h"
#include "queue.h"


class PCB {
   private:
      // MOS CPU Registers
      long        pid;
      one_byte    C;
      address     IC;
      word        R;
      one_byte    PI;
      one_byte    SI;
      one_byte    IOI;
      one_byte    TI;
      word        PTR;
      //timing data for the process
      long saveTotTime;
      long totaltime;
      long timeslice;
      // page Limit
      long pagelimit;
      long pages_printed;
      //statistics
      char stat1[41];
      char stat2[41];
      //indicates if a frame needs to be allocated for a GD
      boolean allocate;

      boolean BLOCKED;

      //indicates that a page fault occured that requires a
      //page to be swapped out of memory, and another page to
      //be swapped into memory
      int swapflag;
      // A queue of the pages allocated to this process. Needed for FIFO page
      // replacement

      // The type of IO needed by this process when on the IO queue
      service_type service;
      // The page number that needs to be swapped in on a page fault
      long progpagenum;
      //The frame number of a reserved frame of memory for IO
      frame_addr reserved_frame;

      //  The offs
   public:
     queue*  page_alloc;
     PCB():timeslice(TIME_SLICE_VALUE ) {page_alloc = new queue;
					 pages_printed = 0;
					 BLOCKED = FALSE;};
     ~PCB() {delete page_alloc;};
     one_byte getC(void) { return C; }
     void putR(word r) {R = r;};
     word getR(void) {return R;};
     void putC(one_byte c) { C = c; }
     address getIC(void) { return IC; }
     void putIC(address ic) { IC = ic; }
     void dec_IC();
     one_byte getPI(void) { return PI; }
     void putPI(one_byte pi) { PI = pi; }
     one_byte getSI(void) { return SI; }
     void putSI(one_byte si) { SI = si; }
     one_byte getIOI(void) { return IOI; }
     void putIOI(one_byte ioi) { IOI = ioi; }
     one_byte getTI(void) { return TI; }
     void putTI(one_byte ti) { TI = ti; }
     word getPTR(void) { return PTR; }
     void putPTR(word ptr) { PTR = ptr; }
     void putPID( long process ) { pid = process; };
     long getPID(){ return pid; }
     void putTotalTime( long inT ){ totaltime = inT; saveTotTime = inT;}
     long getTotalTime(){ return totaltime; }
     void putTimeSlice(long inTS ){ timeslice = inTS; }
     long getTimeSlice( ){ return timeslice; };
     void putPageLimit( long pgnum ){ pagelimit = pgnum; }
     long getPageLimit(){ return pagelimit; }
     void putPagesPrinted( long pgnum ){ pages_printed = pgnum; }
     long getPagesPrinted(){ return pages_printed; }
     void incPagesPrinted() {pages_printed++;};
     void decrementTotalTimeCtr(){ totaltime--; }
     void decrementTimeSliceCtr(){ timeslice--;}
     boolean isTotalTimeUp(){ if( totaltime == 0 )
				 return TRUE;
			      else
				 return FALSE; };
    boolean isTimeSliceUp(){ if( timeslice == 0 )
				return TRUE;
			     else
				return FALSE;};

   long executionTime(){ return ( saveTotTime - totaltime ); };
   void insert_page(long pagenum) {page_alloc->insert(pagenum);};
   long remove_page() {return page_alloc->remove();};
   long next_page() {return page_alloc->front();};
   void set_IO_service(service_type s) { service = s; };
   service_type get_IO_service() { return service; };
   void set_progpagenum(long p) { progpagenum = p+1; };
   void set_datapagenum(long p) { progpagenum = p; };
   long get_progpagenum() { return progpagenum; };
   long get_datapagenum() { return progpagenum; };
   void store_stats(char *a, char *b) { strcpy(stat1,a); strcpy(stat2,b); };
   void get_stats(char *a, char *b) { strcpy(a,stat1); strcpy(b,stat2); };
   boolean get_allocate(void) { return allocate; };
   void set_allocate(boolean a) { allocate = a; };
   void set_reserved_frame(frame_addr f) { reserved_frame = f; };
   frame_addr get_reserved_frame() { return reserved_frame; };
   void setSwapflag(int f) { swapflag = f; }
   int  getSwapflag(void) { return swapflag; }
   void set_blocked(boolean b) {BLOCKED = b;};
   boolean get_blocked(void) {return BLOCKED;};


};

#endif
