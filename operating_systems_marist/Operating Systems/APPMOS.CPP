#include "appmos.h"


AOperatingSystem::AOperatingSystem( long argc, char **argv ):
 traceFile(),
 mem(300),
 cpu( &mem ),
 pmap( &mem ),
 card(),
 lineprinter(),
 buffers(),
 fSys(),
 frameTab( 30, 10 ),
 readyQue(),
 terminateQue(),
 ioQue(),
 channel1( buffers, card, traceFile ),
 channel2( buffers, lineprinter, traceFile ),
 channel3( buffers, fSys, traceFile ) {
// The constructor implements the main loop of the simulation.

 char ioq[40];
 char rq[40];
 char tq[40];

   // Minimum requirements for command line parameters are program name,
   // input file (card set) and output file (printer file)
   if(argc < 3) {
      cout << "     Usage: Opsys cardset printerfile [tracefile] !!!";
      //throw "     Usage: Opsys cardset printerfile [tracefile] !!!";
   }

   // This is a requiste for our OS to work.
   // argv[1] contains the input file's name.
   if(card.load_cardset( argv[1] ) == FALSE) {
      cout << "     The cardset could not be opened !!!";
      //throw "     The cardset could not be opened !!!";
   }

   // this is a requiste for our OS to work.
   // argv[2] is the outfile name.
   if((lineprinter.load_paper( argv[2] ) == FALSE )) {
      cout << "     The printerfile could not be opened !!!";
      //throw "     The printerfile could not be opened !!!";
   }

   // This is a non fatal problem. Mos should be able to operate
   // without a tracefile.
   if (argv[3])
      traceFile.open( argv[3], ios::out );
   else {
      cout << "mosystem: trace file : name not provided." << endl;
      cout << "mosystem: trace file : couldn't open trace file." << endl;
      cout << "mosystem: no trace will be available." << endl;
   }

   // Kick start the simulation, by starting channel 1
   initialize();
   clockcount = 0;
   OPERAND_FAULT = FALSE;
   reserved_frame = -1;
   strcpy(emptystring, "                                        ");

   // Continue until there are no more cards on the card reader, there are
   // no jobs on the ready queue, and all jobs have been properly terminated.
   // Also wait until all three channels are not busy.
   while(simulation_complete() == FALSE){

      if(traceFile) {
	  strcpy(rq,readyQue.see());
	  strcpy(ioq,ioQue.see());
	  strcpy(tq,terminateQue.see());
	  traceFile << "\nCycle# " << clockcount << "  ";
	  traceFile << "RQ" << rq << " " << "IOQ";
	  traceFile << ioq << " " << "TQ" << tq << "\n";
      }
      // Some interrupts take several clock cycles to handle (i.e. page
      // faults). Only execute the next instruction if they have been handled.
      if( isSlaveModeTrue() )
	 slaveMode( );
      else
	 traceFile << "[SLAVE MODE - NOTHING TO DO]\n";

      simulateDevicesAndTimer();
      masterMode();

      clockcount++;

      cout << clockcount << "   " << frameTab.get_free_frames() << "  "
	   /*<< buffers.GetBufferCount()*/ << endl;
      /*cout << "Ready Queue:   ";
      readyQue.see();
      cout << "I/O Queue:   ";
      ioQue.see();
      cout << "Terminate Queue:   ";
      terminateQue.see();
      cout << "Stat Queue:   ";
      statQue.see(); */
   }
}


boolean AOperatingSystem::startChannelOneDevice() {
// This routine simulates the delay of channel one, and gets it started. If
// there are buffers available, channel one gets flagged as busy, and it's
// timer is reset. If buffers aren't available, then don't start the channel,
// since channel one needs input full buffers to store a card after it reads
// one.

    // if buffers are available and channel one is not busy.
    if( buffers.querybuffers() && cpu.get_CHST1() == 0 ){

         // Reserve an empty buffer. We don't want to waste 4 cycles if at the
         // end of 4 cycles, there are no empty buffers to make an input full
         // buffer.
         reserved_input_buffer = buffers.GetEmptyBuffer();

         if (reserved_input_buffer == NULL)
            return FALSE;
         else {
            cpu.set_CHST1( 1 ); // Set channel 1 register BUSY
            channel1.setChannelOneTimer( 0 );

            return TRUE;
         }
    }
    else
       return FALSE;
}


boolean AOperatingSystem::startChannelTwoDevice() {
// This routine simulates the delay of channel two, and gets it started. If
// there are buffers available, channel two gets flagged as busy, and it's
// timer is reset. If buffers aren't available, then don't start the channel,
// since channel two needs output full buffers to store data from a drum track
// after it reads the drum.


    // If buffers are available and channel two is not busy.
    if( buffers.query_out_buffers() && cpu.get_CHST2() == 0){
       cpu.set_CHST2( 1 );  // Set Channel 2 register BUSY.
       channel2.setChannelTwoTimer( 0 );

       return TRUE;
    }
    else
       return FALSE;
}


boolean AOperatingSystem::startChannelThreeDevice( ) {
// This routine simulates the delay of channel three, and gets it started. If
// there are buffers available, channel three gets flagged as busy, and it's
// timer is reset. Since channel three uses buffers allocated by channels one
// and two, and does not allocate it's own, there is no need to query whether
// buffers are available.


  if ( cpu.get_CHST3() == 0){
     cpu.set_CHST3( 1 ); // Set Channel 3 register BUSY
     channel3.setChannelThreeTimer( 0 );
     channel3.setCurrentService( None );

     return TRUE;
  }
  else
     return FALSE;
}


void AOperatingSystem::initialize() {
// This routine starts the simulation by getting input spooling rolling.
// Starting channel one begins the simulation of the channel delay. When it's
// time is up, it will throw the channel 1 interrupt. This will cause the
// channel one interrupt handler to begin call input spooling.
  startChannelOneDevice();
}


void AOperatingSystem::slaveMode() {

   frame_addr    frame;
   address ic_real_addr;
   char ir_real_value[5];
   long pid;
   PCB *id;

   if(traceFile) {
	 traceFile << "\n[Slave mode - executing process# ";
	 id = readyQue.front();
	 if(id==NULL) {
	    cout << "ERROR IN slaveMode()";
	    exit(1);
	 }
	 pid = id->getPID();
	 traceFile << pid << "]" << endl;
   }

   OPCODE_FAULT  = FALSE;
   OPERAND_FAULT = FALSE;

   // Perform a fetch/execute cycle.
   page_number = cpu.pre_fetch();
   long page_map_return = pmap.page_to_frame(cpu.get_PTR(), page_number,
					     frame);
   // Check for page faults
   if (page_map_return == PAGE_INV_NOSWAP) {
      cpu.set_PI(PAGE_FAULT_NOSWAP_INT);
      OPCODE_FAULT = TRUE;
      return;
   }
   else if (page_map_return == PAGE_INV_SWAP) {
      cpu.set_PI(PAGE_FAULT_SWAP_INT);
      OPCODE_FAULT = TRUE;
      return;
   }

   if (traceFile) {
      ic_real_addr = (cpu.bcd_to_dec((two_byte) frame) * 10) +
                     (cpu.get_IC() & 0xFF);
      traceFile << "   IC = " << ic_real_addr;
      traceFile << "/" << cpu.bcd_to_dec(cpu.get_IC());
      traceFile << " (real/logical)" << endl;
      traceFile << "   Instruction Fetch, ";
      traceFile << "   Incrementing IC" << endl;
    }
   page_number = cpu.fetch(frame);
   page_map_return = pmap.page_to_frame(cpu.get_PTR(), page_number, frame);

   // Look for a non-numeric operand. The IC wrap and invalid opcode are
   // handled in the CPU execute method.

   // Check for page faults. In this case, since the instruction ha been
   // fetched, and the operand needs to be resolved, the instruction must
   // be re-fetched, so decrement the instruction counter.
   if (page_map_return == PAGE_INV_NOSWAP) {
      OPERAND_FAULT = TRUE;
      if(traceFile)
	  traceFile << "   Setting PI" << endl;
      cpu.set_PI(PAGE_FAULT_NOSWAP_INT);
   }
   else if (page_map_return == PAGE_INV_SWAP) {
      if(traceFile)
	  traceFile << "   Setting PI" << endl;
      OPERAND_FAULT = TRUE;
      cpu.set_PI(PAGE_FAULT_SWAP_INT);
   }

   // Execute the instruction. Even though there may be a page fault on the
   // operand, the severity depends upon the instruction.
   if(traceFile) {
	traceFile << "    C = " << (int)cpu.get_C()   << " ";
	traceFile << "    R = " << (int)cpu.get_R()   << " ";
	traceFile << "    PI = " << (int)cpu.get_PI() <<  " ";
	traceFile << "    SI = " << (int)cpu.get_SI()   << endl;
	traceFile << "   ** Executing instruction - ";
	cpu.get_IR_string(ir_real_value);
	traceFile << ir_real_value << endl;

   }
   cpu.execute(frame);
   if(traceFile) {
	traceFile << "    C = " << (int)cpu.get_C() << " ";
	traceFile << "    R = " << (int)cpu.get_R() << " ";
	traceFile << "    PI = " << (int)cpu.get_PI() << " ";
	traceFile << "    SI = " << (int)cpu.get_SI()   << endl;
   }
}


void AOperatingSystem::masterMode()
{
PCB*    process = NULL;


 if(traceFile) {
     traceFile << "\n[Master mode]\n";
     traceFile << "   PI = " << (int)cpu.get_PI()   << " ";
     traceFile << "   SI = " << (int)cpu.get_SI()   << " ";
     traceFile << "   TI = " << (int)cpu.get_TI()   << " ";
     traceFile << "   IOI = " << (int)cpu.get_IOI() << endl;

 }
 serviceInterrupts();

 // Some interrupts take several clock cycles to handle. DO NOT enter slave
 // mode again until the interrupts are handled.
 if(!(readyQue.empty()) && (cpu.get_SI() == 0) && (cpu.get_PI() == 0)) {
    process = readyQue.front();

    if (process->get_blocked() == FALSE)
       cpu.set_MODE( SLAVE );
    else
       cpu.set_MODE( MASTER );
 }
 else
    cpu.set_MODE( MASTER );
}


boolean AOperatingSystem::simulation_complete() {
// When there are no more cards left in the card reader, and no more jobs are
// on the ready queue, and all the jobs on the terminate queue have been
// terminated, then the simulation is complete. It is possible that if only
// one job is running, that the above three conditions will be met during
// input spooling. Since the card reader will be empty, and the PCB has yet
// to be created (hence no job on the ready queue), and the job hasn't gotten
// to the terminate queue, the simulation would end prematurely. For this
// reason, check also that the channels are not busy.
PCB*  process = NULL;
char  statline1[41];
char  statline2[41];
long  pid;
long  pages_printed;


   if (card.reader_empty() && readyQue.empty() && terminateQue.empty() &&
      ioQue.empty() && (cpu.get_CHST1() == 0) && (cpu.get_CHST2() == 0) &&
      (cpu.get_CHST3() == 0)) {

      if(traceFile)
	traceFile << "***SIMULATION COMPLETE***" << endl;

      return TRUE;
   }
   else if (card.reader_empty() && readyQue.empty() && (!(terminateQue.empty()))
      && ioQue.empty() && (cpu.get_CHST1() == 0) && (cpu.get_CHST2() == 0) &&
      (cpu.get_CHST3() == 0)) {

      if (process = terminateQue.front()) {
	 // We can cheat and print the stat cards immediately, bypassing
	 // the channels. Only print them if they haven't yet been printed.
	 process->get_stats(statline1, statline2);
	 pages_printed = process->getPagesPrinted();
	 pid = process->getPID();

	 if (statline1[0] != '\0') {

	    lineprinter.print( statline1 );
	    lineprinter.print( statline2 );

	    process->store_stats("", "");   // Mark the stat cards printed.
	 }

	 // Check if there are any data cards to print on the drum before
	 // starting the channel.
	 if (fSys.outputexist(pid)) {

	    // Reserve an empty buffer. We don't want to waste 4 cycles if at
	    // the end of 4 cycles, there are no empty buffers to make an
	    // input full buffer.
	    reserved_output_buffer = buffers.GetEmptyBuffer();

	    if (reserved_output_buffer != NULL) {
	       startChannelThreeDevice();
	       channel3.setCurrentService( OutSpool );

	       return FALSE;
	    }

	 }
	 else {

	    if (!(buffers.query_out_buffers())) {
	       fSys.cleardrum(pid);
	       terminateQue.remove();

	       return FALSE;
	    }
	 }
      }
   }

   else
      return FALSE;
}


void AOperatingSystem::simulateDevicesAndTimer() {
// This routine checks each channel. If the channels simulated time delay has
// elapsed, then the appropriate interrupt is raised. Otherwise, the channel's
// timer is incremented. THe current active process is also checked to
// determine if it's two time limits have elapsed.

PCB *activeProcess=NULL;

  if(traceFile){
    traceFile << "\n[Simulation]\n";
    traceFile << "   TIMERS: Ch1=" << channel1.getChannelOneTimer();
    //<< endl;
    traceFile << " Ch2=" << channel2.getChannelTwoTimer();
    // << endl;
    traceFile << " Ch3=" << channel3.getChannelThreeTimer();
    //<< endl;
    traceFile << "/BUSY FLGS: Ch1=" << (long )cpu.get_CHST1();
    //<< endl;
    traceFile << " Ch2=" << (long )cpu.get_CHST2();
    //<< endl;
    traceFile << " Ch3=" << (long )cpu.get_CHST3();
    //<< endl;
    traceFile << "/IOI Reg=" << (long )cpu.get_IOI();
    //<< endl;
    traceFile << " TI Reg=" << (long )cpu.get_TI()
    << endl;
    }

  if( channel1.channelOneTimerUp() )
     cpu.set_IOI( CHAN1_INT );
  else if( cpu.get_CHST1() )
     channel1.incrementChannelOneTimer();

  if( channel2.channelTwoTimerUp() )
     cpu.set_IOI( CHAN2_INT );
  else if( cpu.get_CHST2() )
     channel2.incrementChannelTwoTimer();


 // Check if either the process's time slice has expired, it's total allowable
 // execution time, or both.
 if(readyQue.empty() == FALSE ){
     activeProcess = readyQue.front();
     if(activeProcess==NULL) {
	 cout << "ERROR IN SIMULATE DEVICICES AND TIMERS()";
	 exit(1);
     }

     if(traceFile) {
       traceFile << "   Active Process Status: \n";
       traceFile << "      Process Number : " << activeProcess->getPID()
       << endl;
       traceFile << "      TotalTime left : " << activeProcess->getTotalTime()
       << endl;
       traceFile << "      Time Slice left: " << activeProcess->getTimeSlice()
       << endl;
     }

     activeProcess->decrementTotalTimeCtr();
     activeProcess->decrementTimeSliceCtr();

     if( activeProcess->isTotalTimeUp() && activeProcess->isTimeSliceUp() )
	cpu.set_TI( TIMER_BOTH_INT );
     else if( activeProcess->isTotalTimeUp() )
	cpu.set_TI( TIMER_TOTAL_INT );
     else if( activeProcess->isTimeSliceUp() )
	cpu.set_TI( TIMER_SLICE_INT );

}

 if( channel3.channelThreeTimerUp() )
    cpu.set_IOI( CHAN3_INT );
 else if( cpu.get_CHST3() )
    channel3.incrementChannelThreeTimer();
}


void AOperatingSystem::serviceInterrupts() {
// This routine checks if any interrupts have been raised, and if so, it calls
// the appropriate handler routines.

   long SIInterrupt = 0;
   long TIInterrupt = 0;
   long PIInterrupt = 0;
   long IOIInterrupt = 0;


   IOIInterrupt = cpu.get_IOI();
   if( IOIInterrupt )
      serviceIOInterrupt( IOIInterrupt );

   SIInterrupt = cpu.get_SI();
   if( SIInterrupt )
     serviceSIInterrupt( SIInterrupt );

   PIInterrupt = cpu.get_PI();
   if( PIInterrupt )
     servicePIInterrupt( PIInterrupt );

   TIInterrupt = cpu.get_TI();
   if( TIInterrupt )
     serviceTIInterrupt( TIInterrupt );
}


void AOperatingSystem::serviceSIInterrupt( long intr ) {
// This is the interrupt handler fo supervisor interrupts

   long pid;
   long timeslice;
   long frame;
   frame_addr framenum;
   long rc;
   long PTR;
   long interrupt_type;
   PCB *process = NULL;
   page swap_page;
   char    tmp[41];
   boolean frame_available;
   long memcell;
   long victim_frame;


   switch( intr ) {
      case GD_INT:
	 // There are three conditions to consider. The first is
	 // PAGE_FAULT_NOSWAP_INT. If this case exists, set up the PCB for
	 // the io queue to get a data card from the drum. If PAGE_FAULT_SWAP
	 // _INT, then assume that the frame had been valid, the programmer
	 // wanted to overwrite it. Set up the PCB the same, except delete the
	 // swapped page from the drum. If PAGE_VALID, simply overwrite.
	 process = readyQue.remove();

	 if(process==NULL) {
	    cout << "ERROR IN SERVICE_SI_INTERUPT()";
	    exit(1);
	 }

	 // Set io service, and destination page.
	 process->set_IO_service(LOAD_DATA);
	 process->set_datapagenum(page_number);

	 // Save the CPU state
	 process->putTimeSlice(TIME_SLICE_VALUE);
	 process->putIC(cpu.get_IC());
	 process->putC(cpu.get_C());
	 process->putR(cpu.get_R());

	 pid = process->getPID();

	 // Insert onto the io queue, clear interrupts
	 ioQue.insert(process);
	 interrupt_type = cpu.get_PI();
	 cpu.set_SI(0);
	 cpu.set_PI(0);

	 switch(interrupt_type) {
	    case PAGE_FAULT_NOSWAP_INT:
	       process->set_allocate(TRUE);
	       break;
	    case PAGE_FAULT_SWAP_INT:
	       process->set_allocate(FALSE);

	       // If the page is swapped, and a GD, simply overwrite the page
	       // Remove the swapped page from the drum.
	       fSys.killswappage(page_number, pid);

	       break;
	    default:
	       // The page is valid; no interrupt was raised
	       process->set_allocate(FALSE);
	       break;
	 }

	 // If channel 3 is not busy, then start it, setting the service to
	 // load a program card. We are assuming that if channel 3 is idle,
	 // then there is nothing on the io queue, and we can load the
	 // current process.
	 if (cpu.get_CHST3() == 0) {
	    // Only allocate memory if the frame has not been allocated.
	    if ((interrupt_type == PAGE_FAULT_NOSWAP_INT) ||
		(interrupt_type == PAGE_FAULT_SWAP_INT)) {
	       frame_available = frameTab.allocate_frame(frame, pid );

	       if (frame_available)
		  page_to_mem(frame, emptystring);
	    }
	    else
	       frame_available = TRUE;

	    if (frame_available) {
	       process->set_reserved_frame(frame);
	       process->page_alloc->see();
	       process->insert_page((long) page_number);
	       process->page_alloc->see();
               startChannelThreeDevice();
	       channel3.setCurrentService( GetData );
	     }
	     else {
		// Set the page number that generated the fault
		process->set_datapagenum(page_number);

		// We must re-execute the GD instruction after it has memory
		// from the swap.
		cpu.dec_IC();
		process->putIC(cpu.get_IC());

		startChannelThreeDevice();
		channel3.setCurrentService( PutSwap );
	     }
	 }

	 // Schedule the next job
	 scheduler(TRUE);

	 break;
      case PD_INT:
	// Works similar to GD_INT
	process = readyQue.front();

	if(process==NULL) {
	     cout << "ERROR IN SERVICE_SI_INTERUPT()";
	     exit(1);
	 }

	pid = process->getPID();

	interrupt_type = cpu.get_PI();

	cpu.set_SI(0);
	cpu.set_PI(0);

	switch(interrupt_type) {
	   case PAGE_FAULT_NOSWAP_INT:
	      // From the programmer's perspective, all program cards are
	      // already loaded in user memory. If this fault occurrs, then
	      // check if a program card exists for that page. If it does,
	      // then copy the program card to the data part of the file
	      // system. Otherwise, if it doesn't exist, then we print an
	      // empty string. This is handled by the filesystem. No I/O is
	      // required. Must add 1 for the filesys.
	      if (fSys.prog2output(page_number + 1, pid)) {
		 // To save I/O, just let the fsys allocate a data track.
		 // Since the memory is not allocated, it is "garbage" data.
		 fSys.storepage("PD INVALID MEMORY REFERENCE", OUTPUT, pid);
	      }

	      return;
	   case PAGE_FAULT_SWAP_INT:
	      // The page has been swapped. Don't reload into memory, then
	      // write it back to drum. Simply let the filesystem allocate a
	      // new track, and copy the data from the swap track to the new
	      // track, which will be used for output.
	      fSys.swap2output(page_number, pid);

	      // The interrupt has just been handled. Since PutData won't be
	      // called, the PCB must be updated here
	      process->incPagesPrinted();


	      return;
	   default:
	      // The page to be printed is in memory. We want to do I/O.
	      process = readyQue.remove();
	      if(process==NULL) {
		   cout << "ERROR IN SERVICE_SI_INTERUPT()";
		   exit(1);
	      }

	      // Set io service, and destination page.
	      process->set_IO_service(PUT_DATA);
	      process->set_datapagenum(page_number);

	      // Save the CPU state
	      process->putTimeSlice(TIME_SLICE_VALUE);
	      process->putIC(cpu.get_IC());
	      process->putC(cpu.get_C());
	      process->putR(cpu.get_R());

	      // Insert onto the io queue
	      ioQue.insert(process);

	      break;
	}

	// If channel 3 is not busy, then start it, setting the service to
	// load a program card. We are assuming that if channel 3 is idle,
	// then there is nothing on the io queue, and we can load the
	// current process.
	if (cpu.get_CHST3() == 0) {
	   startChannelThreeDevice();
	   channel3.setCurrentService( PutData );
	}

	// Schedule the next job
	scheduler(TRUE);

	break;
      case HALT_INT:
	 // IT's obvious what to do
	 terminationService( intr, READYQ );
	 cpu.set_SI(0);
	 break;
      default:
	 break;
   }     // switch()
}


void AOperatingSystem::servicePIInterrupt( long intr ) {
// This is the interrupt handler for program interrupts

   long pid;
   long timeslice;
   long frame;
   long rc;
   page swap_page;
   long PTR;
   PCB *processFault = NULL;
   char   tmp[41];
   char   opcode[3];
   boolean frame_available = FALSE;
   long result;
   long memcell;

   switch( intr ){
      case INVALID_OPCODE_INT:
	// Terminate:
	terminationService( intr, READYQ );

	break;
     case INVALID_OPERAND_NONNUMERIC_INT:
	// Terminate:
	terminationService( intr, READYQ );

	break;
     case PAGE_FAULT_NOSWAP_INT:
	// If the fault is on the opcode, then it means that a page fault was
	// generated while resolving the instruction counter. Load a program
	// card.
	if (OPCODE_FAULT == TRUE) {
	   processFault = readyQue.remove();

	   if(processFault==NULL) {
	      cout << "ERROR IN SERVICE_PI_INTERUPT()";
	      exit(1);
	   }

	   // When doing I/O, we will need to know what to do. Put this info
	   // into the PCB. We also need to store which program card we want.
	   processFault->set_IO_service(LOAD_PROGRAM_CARD);
	   processFault->set_progpagenum(page_number);

	   // Save the CPU state
	   processFault->putTimeSlice(TIME_SLICE_VALUE);
	   processFault->putIC(cpu.get_IC());
	   processFault->putC(cpu.get_C());
	   processFault->putR(cpu.get_R());
	   pid = processFault->getPID();

	   // Insert PCB onto I/O queue to wait for I/O, and clear interrupt.
	   ioQue.insert(processFault);
	   cpu.set_PI(0);

	   // If channel 3 is not busy, then start it, setting the service to
	   // load a program card. We are assuming that if channel 3 is idle,
	   // then there is nothing on the io queue, and we can load the
	   // current process.
	   if (cpu.get_CHST3() == 0) {
	      frame_available = frameTab.allocate_frame(frame, pid );

	      if (frame_available) {
	         page_to_mem(frame, emptystring);
	         memcell = frame;
		 memcell = memcell << 8;
		 //mem.flush_cells(memcell, 10);
		 		 processFault->set_reserved_frame(frame);

		 processFault->page_alloc->see();
		 processFault->insert_page((long) page_number);
		 processFault->page_alloc->see();

		 startChannelThreeDevice();
		 channel3.setCurrentService( LoadService );
	      }
	      else {
		 // Set the page number that generated the fault
		 processFault->set_datapagenum(page_number);

		 startChannelThreeDevice();
		 channel3.setCurrentService(PutSwap);
	      }
	   }

	   // Schedule the next job
	   scheduler(TRUE);
	}
	else if (OPERAND_FAULT == TRUE) {
	   cpu.get_opcode(opcode);

	   cpu.set_PI(0);

	   // If the BT instruction, it will page fault on the opcode when
	   // execution resumes at the result of the branch.
	   if (strcmp(opcode, "BT")) {
	      // We are doing pure -demand paging. If the page fault is on a
	      // SR, CR, LR, we must check if a program card should be loaded
	      // there.

	      // From the programmer's point of view, all the program
	      // cards are in memory, however we are doing pure demand
	      // paging. Check if the operand should have a program card
	      // loaded at it's location. If so, load it; otherwise load
	      // the register with zero.
	      processFault = readyQue.front();
	      if(processFault==NULL) {
		    cout << "ERROR IN SERVICE_PI_INTERUPT()";
		    exit(1);
		 }

	      if (fSys.progexist(page_number + 1, processFault->getPID())) {
		 processFault = readyQue.remove();
		 if(processFault==NULL) {
		    cout << "ERROR IN SERVICE_PI_INTERUPT()";
		    exit(1);
		 }

		 // See load service above
		 processFault->set_IO_service(LOAD_PROGRAM_CARD);
		 processFault->set_progpagenum(page_number);

		 // Save the CPU state
		 processFault->putTimeSlice(TIME_SLICE_VALUE);
		 processFault->putC(cpu.get_C());
		 processFault->putR(cpu.get_R());

		 // Decrement the IC to re-execute the instruction
		 cpu.dec_IC();
		 processFault->putIC(cpu.get_IC());

		 pid = processFault->getPID();

		 ioQue.insert(processFault);

		 if (cpu.get_CHST3() == 0) {
		    frame_available = frameTab.allocate_frame(frame, pid );

		    if (frame_available) {
		       page_to_mem(frame, emptystring);
		       memcell = frame;
		       memcell = memcell << 8;
		       //mem.flush_cells(memcell, 10);		       
		       processFault->set_reserved_frame(frame);

		       processFault->page_alloc->see();
		       processFault->insert_page((long) page_number);
		       processFault->page_alloc->see();

                       startChannelThreeDevice();
		       channel3.setCurrentService( LoadService );
		    }
		    else {
		       // Set the page number that generated the fault
		       processFault->set_datapagenum(page_number);

		       startChannelThreeDevice();
		       channel3.setCurrentService(PutSwap);		    }
		 }

		 // Schedule the next job
		 scheduler(TRUE);
	      }
	      else {
		 // A program card does not exist at this page
		 // If LR, and memory not allocated, then load register with
		 // "garbage". IF CR, then assume a FALSE compare.
		 if (!strcmp(opcode, "LR"))
		    cpu.set_R(0);
		 else if (!strcmp(opcode, "CR"))
		    cpu.set_C(0);
		 else {
		    // Allocate a frame, if it is available, decrement the IC,
		    // and re-execute the instruction; otherwise we must swap.
		    pid = processFault->getPID();

		    frame_available = frameTab.allocate_frame(frame, pid );

		    if (frame_available) {
		       page_to_mem(frame, emptystring);
		       memcell = frame;
		       memcell = memcell << 8;
		       //mem.flush_cells(memcell, 10);		       
		       // Update page table
		       PTR = cpu.get_PTR();
		       PTR = PTR << 8;

		       mem.write_cell(PTR + page_number, frame | PAGE_VALID);

		       processFault->page_alloc->see();
		       processFault->insert_page((long) page_number);
		       processFault->page_alloc->see();

		       cpu.dec_IC();
		    }
		    else {
		       // We need to swap. Put on IO queue
		       processFault = readyQue.remove();
		       ioQue.insert(processFault);

		       processFault->set_datapagenum(page_number);

		       // We need to re-execute SR after the frame has been
		       // allocated
		       cpu.dec_IC();
		       processFault->putIC(cpu.get_IC());

		       if (cpu.get_CHST3() == 0) {
			  startChannelThreeDevice();
			  channel3.setCurrentService(PutSwap);
		       }
		       else {
			  processFault->set_IO_service(PUT_SWAP);

			  // Schedule the next job
			  scheduler(TRUE);
		       }		    }
		 }
	      }
	   }
	}

	break;
     case PAGE_FAULT_SWAP_INT:
	cpu.get_opcode(opcode);

	// If the instruction was BT, then let it branch to the instruction,
	// and let it page fault on the instruction counter
	if ((!strcmp(opcode, "BT")) && (OPERAND_FAULT == TRUE)) {
	   cpu.set_PI(0);
	   return;
	}

	processFault = readyQue.remove();

	if (processFault == NULL) {
	   cout << "ERROR IN SERVICE_PI_INTERRUPT()" << endl;
	   exit(1);
	}

	processFault->set_IO_service(GET_SWAP);
	processFault->set_datapagenum(page_number);

	// Save CPU state
	processFault->putTimeSlice(TIME_SLICE_VALUE);

	// If the fault was on the IC, then we don't need to dec the IC
	if (OPCODE_FAULT == TRUE)
	   processFault->putIC(cpu.get_IC());
	else {
	   cpu.dec_IC();
	   processFault->putIC(cpu.get_IC());
	}

	processFault->putC(cpu.get_C());
	processFault->putR(cpu.get_R());
	pid = processFault->getPID();

	// Insert PCB onto I/O queue to wait for I/O, and clear interrupt.
	ioQue.insert(processFault);
	cpu.set_PI(0);

	// If channel 3 is not busy, then start it, setting the service to
	// load a program card. We are assuming that if channel 3 is idle,
	// then there is nothing on the io queue, and we can load the
	// current process.
	if (cpu.get_CHST3() == 0) {
	   frame_available = frameTab.allocate_frame(frame, pid );

	   if (frame_available) {
	      page_to_mem(frame, emptystring);
	      memcell = frame;
	      memcell = memcell << 8;
	      //mem.flush_cells(memcell, 10);	      
	      processFault->set_reserved_frame(frame);

	      processFault->page_alloc->see();
	      processFault->insert_page((long) page_number);
	      processFault->page_alloc->see();

	      startChannelThreeDevice();
	      channel3.setCurrentService( GetSwap );
	   }
	   else {
	       // Set the page number that generated the fault
	       processFault->set_datapagenum(page_number);

	       startChannelThreeDevice();
	       channel3.setCurrentService(PutSwap);
	   }
	}

	scheduler(TRUE);
	break;

     case INVALID_IC_OVERFLOW_INT:
	// Terminate:
	terminationService( intr, READYQ );

	break;
     default:
       break;
     }
}


void AOperatingSystem::serviceTIInterrupt( long intr )
{
  switch( intr ){
      case TIMER_SLICE_INT:
	 // In the rare case where the program executes it's HALT instruction
	 // and the time_slice has expired, then terminate, rather than
	 // schedule the same process again.
	 if (cpu.get_SI() == HALT_INT)
	    terminationService( TIMER_TOTAL_INT_DUP, READYQ );
	 else
	    scheduler(FALSE);

	 break;
      case TIMER_TOTAL_INT:
	 // Terminate the process.
	 terminationService( TIMER_TOTAL_INT_DUP, READYQ );

	 break;
      case TIMER_BOTH_INT:
	 // Terminate the process.
	 terminationService( TIMER_BOTH_INT_DUP, READYQ );

	 break;
      default:
	 break;
  }
}


void AOperatingSystem::serviceIOInterrupt(long intr)  {
// This routine handles the IOI interrupts raised when a channel has finished
// its's delay simulation. Note that some of the interrupts are handled in the
// other handlers. This allows the priority of the channels to be overriden.
// For instance, it lets the GD_INT handler get immediate control of the
// channel when it is available.

   long rc;
   long PTR;
   boolean outspool;
   ifb_type  inspool_result;
   char   tmp[41];
   service_type st;
   frame_addr  frame;


   switch( intr ) {
      case CHAN1_INT: // Channel One has alone produced an interrupt.
	 channel1_IH();

	 break;   // Case CHAN1_INT
      case CHAN2_INT:
	 cpu.clear_IOI( CHAN2_INT );
	 cpu.set_CHST2(0);
	 channel2.setChannelTwoTimer( 0 );

	 channel2.outputSpooling();

	 // Channel 2 does all the work. If more buffers, then print them.
	 if (buffers.query_out_buffers())
	    startChannelTwoDevice();

	 break;  // Case CHAN2_INT
      case CHAN3_INT:

	 channel3_IH();

	 break;
      case CHAN1_2_INT:
	 channel1_IH();

	 // Now handle channel 2
	 cpu.clear_IOI( CHAN2_INT );
	 cpu.set_CHST2(0);
	 channel2.setChannelTwoTimer( 0 );

	 channel2.outputSpooling();

	 // Channel 2 does all the work. If more buffers, then print them.
	 if (buffers.query_out_buffers())
	    startChannelTwoDevice();

	 break;      // CHAN1_2_INT

      case CHAN1_3_INT:
	 channel1_IH();

	 // Now handle channel 3
	 channel3_IH();

	 break;   // Case CHAN1_3_INT

      case CHAN2_3_INT:
	 // Now handle channel 2
	 cpu.clear_IOI( CHAN2_INT );
	 cpu.set_CHST2(0);
	 channel2.setChannelTwoTimer( 0 );

	 channel2.outputSpooling();

	 // Channel 2 does all the work. If more buffers, then print them.
	 if (buffers.query_out_buffers())
	    startChannelTwoDevice();

	 // Now handle channel 3
	 channel3_IH();

	 break;   // Case CHAN2_3_INT

      case CHAN1_2_3_INT:
	 channel1_IH();

	 // Now handle channel 2
	 cpu.clear_IOI( CHAN2_INT );
	 cpu.set_CHST2(0);
	 channel2.setChannelTwoTimer( 0 );

	 channel2.outputSpooling();

	 // Channel 2 does all the work. If more buffers, then print them.
	 if (buffers.query_out_buffers())
	    startChannelTwoDevice();

	 // Now handle channel 3
	 channel3_IH();

	 break;   // Case CHAN1_2_3_INT

   }  // switch(intr)
}


void AOperatingSystem::channel1_IH() {
   long rc;
   long PTR;
   boolean frame_available = FALSE;
   boolean outspool;
   ifb_type  inspool_result;
   char   tmp[41];
   service_type st;
   frame_addr  frame;


   // Clear the interrupt. Channel one's delay has just been
   // simulated, so call channel 1 input spooling to read a card,
   // and put it into an input full buffer.
   cpu.clear_IOI( CHAN1_INT );
   cpu.set_CHST1(0);
   channel1.setChannelOneTimer( 0 );

   // Put back the reserved buffer. It was reserved so that it would be
   // available when channel 1's simulated delay was up. This ensures
   // that channel 1 won't be started needlesly.
   if(reserved_input_buffer==NULL) {
	cout << "ERROR IN CHANEL1_IH";
	exit(1);
   }
   buffers.PutEmptyBuffer(reserved_input_buffer);
   reserved_input_buffer = NULL;

   inspool_result = channel1.inputSpooling();

   if(traceFile)
      traceFile << "   Channel 1 task complete" << endl;

   // If channel 3 is not busy, start it to simulate it's delay, and
   // set the service to input spooling. When it's interrupt is
   // raised, it will take the input full buffer just created, and
   // put it on the drum. Only start if an input full buffer was
   // created.
   if(cpu.get_CHST3() == 0) {
      switch (inspool_result) {
	 case IFB_MADE:
	    startChannelThreeDevice();
	    channel3.setCurrentService( InSpool );
	    break;
	 case NO_IFB_MADE:
	    break;
	 case EOJ_IFB_MADE:
	    // An EOJ card was read. Reserve a frame of memory so that
	    // when channel 3 is done input spooling, a frame will be
	    // available to make the page table for the process. Use a
	    // dummy job number (-1).
	    frame_available = frameTab.allocate_frame(reserved_frame, -1);

	    // If there is not a frame available, don't start channel
	    // 3, since there probably won't be a frame available to
	    // make the page table.
	    if (frame_available) {
	       startChannelThreeDevice();
	       channel3.setCurrentService( InSpool );
	    }
      }
   }

   // Start channel one, if there are cards remaining in the card
   // reader, so that another card may be gotten.
   if (channel1.reader_empty() == FALSE)
      startChannelOneDevice();
}


void AOperatingSystem::channel3_IH() {
   long pid = 0;
   long rc;
   long PTR;
   PCB *process=NULL;
   boolean frame_available = FALSE;
   boolean outspool;
   boolean allocate = FALSE;
   ifb_type  inspool_result;
   char   tmp[41];
   service_type st;
   long  frame;
   long swappage;
   page  pagenumber;
   frame_addr put_frame;
   char   statline1[41];
   char   statline2[41];
   long   pages_printed;
   long   need_buffer = 0;
   long   memcell;
   ABuffer *ifbuffer = NULL;
   char    *strptr = NULL;


   cpu.clear_IOI( CHAN3_INT );
   cpu.set_CHST3(0);
   channel3.setChannelThreeTimer( 0 );

   switch (channel3.getCurrentService()) {
      case InSpool:
	 // Put back the reserved frame. This guarantees that we have
	 // at least one frame for the process to create a page table.
	 // Reset reserved_frame to an invalid frame number.
	 if (reserved_frame != -1) {
	    frameTab.deallocate_frame(reserved_frame);
	    reserved_frame = -1;
	 }

	 pid = channel3.inputSpooling();

	 if ( pid )
	    createProcessPageTable( pid );

	 break;

      case OutSpool:
	 // Put back the reserved buffer. It was reserved so that it
	 // would be available when channel 1's simulated delay was up.
	 // This ensures that channel 1 won't be started needlesly.
	 if (reserved_output_buffer != NULL) {
	    buffers.PutEmptyBuffer(reserved_output_buffer);
	    reserved_output_buffer = NULL;
	 }

	 // Perform output spooling. If outspool is FALSE, then there
	 // are no more cards on the drum. Everything for this process
	 // has been output spooled. REmove the process from the term
	 // queue, and free all the tracks on the drums.
	 process = terminateQue.front();
	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid     = process->getPID();

	 channel3.outputSpooling(pid);

	 // If channel 2 is not busy, start it to get the buffer.
	 if (cpu.get_CHST2() == 0)
	    startChannelTwoDevice();

	 break;
      case LoadService:
	 process = ioQue.front();
	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid = process->getPID();
	 frame = process->get_reserved_frame();

	 pagenumber = process->get_progpagenum();

	 rc = channel3.getDataService(tmp, pid, PROGRAM, pagenumber);

	 page_to_mem(frame, tmp);

	 // Update the page table
	 PTR = process->getPTR();
	 PTR = PTR << 8;

	 mem.write_cell(PTR + (pagenumber - 1), frame | PAGE_VALID);

	 process = ioQue.remove();
	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 // Put the PCB back on the ready queue. For the boundary
	 // condition where there ready queue was empty, we must call
	 // the scheduler.
	 if (readyQue.empty() == TRUE) {
	    readyQue.insert(process);
	    scheduler(TRUE);
	 }
	 else
	    readyQue.insert(process);

	 break;
      case GetData:
	 process = ioQue.front();
	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid = process->getPID();
	 allocate = process->get_allocate();

	 pagenumber = process->get_datapagenum();

	 rc = channel3.getDataService(tmp, pid, INPUT, pagenumber);

	 // If rc is non-zero, then the programmer has asked for more
	 // data cards than are available. This is a fatal error.
	 if (rc) {
	    terminationService(OUT_OF_DATA_INT, IOQ);
	    scheduler(FALSE);

	    break;//return;
	 }

	 if (allocate == TRUE) {
	    frame = process->get_reserved_frame();
	    page_to_mem(frame, tmp);
	 }
	 else {
	    pmap.page_to_frame(process->getPTR(), pagenumber, put_frame);
	    page_to_mem(put_frame, tmp);
	 }

	 // Update the page table, only if memory had to be allocated,
	 // otherwise, we are overwriting the contents of what is there.
	 // If the page is marked swapped, assume that the programmer
	 // wants to overwrite it.
	 if (allocate == TRUE) {
	    PTR = process->getPTR();
	    PTR = PTR << 8;

	    mem.write_cell(PTR + pagenumber, frame | PAGE_VALID);
	 }

	 process = ioQue.remove();
	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 // Put the PCB back on the ready queue. For the boundary
	 // condition where there ready queue was empty, we must call
	 // the scheduler.
	 if (readyQue.empty() == TRUE) {
	    readyQue.insert(process);
	    scheduler(TRUE);
	 }
	 else
	    readyQue.insert(process);
	 break;

      case GetSwap:
	 process = ioQue.remove();

	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid = process->getPID();

	 // Get the page number of the page that was swapped to disk
	 pagenumber = process->get_datapagenum();

	 // Get the page from the drun
	 rc = channel3.getSwapPage(tmp, pagenumber, pid);

	 // Load the swapped page into memory
	 frame = process->get_reserved_frame();
	 page_to_mem(frame, tmp);

	 // Update the page table
	 PTR = process->getPTR();
	 PTR = PTR << 8;

	 mem.write_cell(PTR + pagenumber, frame | PAGE_VALID);

	 // Insert the new allocated page into the PCB FIFO queue
	 //process->insert_page(pagenumber);

	 // Put the PCB back on the ready queue. For the boundary
	 // condition where there ready queue was empty, we must call
	 // the scheduler.
	 if (readyQue.empty() == TRUE) {
	    readyQue.insert(process);
	    scheduler(TRUE);
	 }
	 else
	    readyQue.insert(process);
	    break;

      case PutData:
	 process = ioQue.remove();

	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid = process->getPID();

	 // Get the data from memory, and write it to the drum. The
	 // page number must be mapped to a physical frame.
	 pmap.page_to_frame(process->getPTR(), process->get_datapagenum(),
			    put_frame);
	 mem_to_page(put_frame, tmp);

	 rc = channel3.putDataService(tmp, pid, OUTPUT);

	 // If there are no free tracks on the drum, put the PCB onto the
	 // back of the io queue to try again.
	 if (rc) {
	    ioQue.insert(process);
	    return;
	 }

	 process->incPagesPrinted();

	 // Put the PCB back on the ready queue. For the boundary
	 // condition where there ready queue was empty, we must call
	 // the scheduler.
	 if (readyQue.empty() == TRUE) {
	    readyQue.insert(process);
	    scheduler(TRUE);
	 }
	 else
	    readyQue.insert(process);

	 break;

      case PutSwap:
	 process = ioQue.remove();

	 if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }

	 pid = process->getPID();

	 process->page_alloc->see();

	 // Select the victim page from the FIFO queue in the PCB
	 swappage = process->remove_page();

	 // Get the page that generated the page fault
	 pagenumber = process->get_datapagenum();

	 // Get the data from memory, and write it to the drum. The
	 // page number must be mapped to a physical frame.
	 pmap.page_to_frame(process->getPTR(), swappage, put_frame);
	 mem_to_page(put_frame, tmp);

	 rc = channel3.putSwapPage(tmp, swappage, pid );

	 // If there are no free tracks on the drum, put the PCB onto the
	 // back of the io queue to try again.
	 if (rc) {
	    ioQue.insert(process);
	    return;
	 }

	 // Now update the page table to indicate that the victim page was
	 // swapped, and then update the page table to indicate that the page
	 // that was faulted on is now valid.
	 PTR = process->getPTR();
	 PTR = PTR << 8;

	 mem.write_cell(PTR + swappage, put_frame | PAGE_INV_SWAP);

	 frameTab.deallocate_frame(put_frame);

	 // Put the PCB back on the ready queue. For the boundary
	 // condition where there ready queue was empty, we must call
	 // the scheduler.
	 if (readyQue.empty() == TRUE) {
	    readyQue.insert(process);
	    scheduler(TRUE);
	 }
	 else
	    readyQue.insert(process);

	 break;
   }

   // Set channel 3's priority here. Give priority to output spooling,
   // then input spooling, then general I/O (Load Service, GD, PD,
   // etc.). Note when input spooling, if there are no free frames,
   // give priority to I/O over input spooling. This will prevent
   // possible thrashing, and prevent the I/O queue from starvation.
   if (process = terminateQue.front()) {
      // We can cheat and print the stat cards immediately, bypassing
      // the channels. Only print them if they haven't yet been printed.
      process->get_stats(statline1, statline2);
      pages_printed = process->getPagesPrinted();
      pid = process->getPID();

      if (statline1[0] != '\0') {

	 lineprinter.print( statline1 );
	 lineprinter.print( statline2 );

	 process->store_stats("", "");   // Mark the stat cards printed.
      }

      // Check if there are any data cards to print on the drum before
      // starting the channel.
      if (fSys.outputexist(pid)) {

	 // Reserve an empty buffer. We don't want to waste 4 cycles if at
	 // the end of 4 cycles, there are no empty buffers to make an
	 // input full buffer.
	 reserved_output_buffer = buffers.GetEmptyBuffer();

	 if (reserved_output_buffer != NULL) {
	    startChannelThreeDevice();
	    channel3.setCurrentService( OutSpool );

	    return;
	 }
	 else
	    need_buffer = 1;
      }
      else {
	 // All the output data has been spooled by channel 3 for the job.
	 // Clear all the tracks that are left on the drum, and then remove
	 // the PCB from the terminate queue. Wait until all the output buffers
	 // have been transferred to the printer before taking the job off of
	 // the terminate queue; otherwise if more than one job is on, then
	 // their output will overlap.
	 if (!(buffers.query_out_buffers())) {
	    fSys.cleardrum(pid);
	    terminateQue.remove();
	 }
      }
   }

   if (channel3.query_in_buffers()) {
      // If the next input full buffer is an $EOJ, we need a frame frame to
      // create the page table. Try to get a frame. If we can't, then yield
      // channel 3 priority to handling the I/O queue.
      ifbuffer = buffers.CheckInputBuffer();

      strcpy(tmp, ifbuffer->getBufStr());
      strptr = strstr(tmp, "$EOJ");

      if (strptr != NULL) {
	 frame_available = frameTab.allocate_frame(reserved_frame, -1);

	 if (frame_available) {
	    startChannelThreeDevice();
	    channel3.setCurrentService( InSpool );

	    return;
	 }
      }
      else {
	 // It is safe to do input spooling without getting a free frame
	 startChannelThreeDevice();
	 channel3.setCurrentService( InSpool );

	 return;
      }   }
Retry:   if (process = ioQue.front()) {
      // Determine the I/O to be performed
      if(process==NULL) {
	    cout << "ERROR IN CHANNEL3_IH()";
	    exit(1);
	 }
      st  = process->get_IO_service();
      pid = process->getPID();

      switch(st) {
	 case LOAD_PROGRAM_CARD:

	    // Allocate frame, since it will be used 2 cycles later.
	    frame_available = frameTab.allocate_frame(frame, pid );

	    if (frame_available) {
	       page_to_mem(frame, emptystring);
	       memcell = frame;
	       memcell = memcell << 8;
	       //mem.flush_cells(memcell, 10);	       
	       process->set_reserved_frame(frame);

	       process->page_alloc->see();
	       process->insert_page(process->get_datapagenum() -1);
	       process->page_alloc->see();

	       startChannelThreeDevice();
	       channel3.setCurrentService( LoadService );
	    }
	    else {
	       // Since switching from load service, the page number must be
	       // decremented by one since set_progpagenum adds one.

	       // Before we can swap, we must have something to swap with.
	       // If no pages have allocated, we cannot swap. Remove and put
	       // on back of IOqueue.
	       if (process->page_alloc->length() != 0) {
		  process->set_datapagenum(process->get_progpagenum() -1);
		  startChannelThreeDevice();
		  channel3.setCurrentService(PutSwap);
	       }
	       else {
		  process = ioQue.remove();
		  ioQue.insert(process);

		  channel3.setCurrentService(None);
		  goto Retry;
	       }

	    }

	    break;
	 case LOAD_DATA:
	    allocate = process->get_allocate();

	    // If allocate is TRUE, we need to get a frame to put
	    // the data card, otherwise, we will just be overwriting
	    // allocated memory.
	    if (allocate == TRUE) {
	       // Allocate frame, since it will be used 2 cycles later.
	       frame_available = frameTab.allocate_frame(frame, pid );

	       if (frame_available) {
	          page_to_mem(frame, emptystring);
	          memcell = frame;
		  memcell = memcell << 8;
		  //mem.flush_cells(memcell, 10);		  
		  process->set_reserved_frame(frame);

		  process->page_alloc->see();
		  process->insert_page(process->get_datapagenum());
		  process->page_alloc->see();

		  startChannelThreeDevice();
		  channel3.setCurrentService( GetData );
	       }
	       else {
		  startChannelThreeDevice();
		  channel3.setCurrentService(PutSwap);

		  // We want to re-execute the GD
		  process->dec_IC();
	       }
	    }
	    else {
	       startChannelThreeDevice();
	       channel3.setCurrentService( GetData );
	    }

	    break;
	 case GET_SWAP:
	       // Allocate frame, since it will be used 2 cycles later.
	       frame_available = frameTab.allocate_frame(frame, pid );

	       if (frame_available) {
	          page_to_mem(frame, emptystring);
	          memcell = frame;
		  memcell = memcell << 8;
		  //mem.flush_cells(memcell, 10);
		  process->set_reserved_frame(frame);

		  process->page_alloc->see();
		  process->insert_page(process->get_datapagenum());
		  process->page_alloc->see();

		  startChannelThreeDevice();
		  channel3.setCurrentService( GetSwap );
	       }
	       else {
		 // We wanted to get a swap card, but we couldn't allocate
		 // a frame of memory, so we must choose another page to swap.
		 // We must first check the FIFO replacement queue. If there is
		 // nothing on it, then there is nothing to swap with. In this
		 // case, we must put the PCB on the back of the ioQue, and
		 // hope that when it comes back to the front, that there will
		 // be an available frame for it.
		 if (process->page_alloc->length() != 0) {
		     startChannelThreeDevice();
		     channel3.setCurrentService(PutSwap);
		  }
		  else {
		     process = ioQue.remove();
		     ioQue.insert(process);

		     channel3.setCurrentService(None);
		     goto Retry;
		  }	       }

	    break;

	 case PUT_DATA:
	    startChannelThreeDevice();
	    channel3.setCurrentService( PutData );

	    break;

	 case PUT_SWAP:
	    startChannelThreeDevice();
	    channel3.setCurrentService( PutSwap );
	    break;
      }            // switch(st)
   }               // else if (process = ioQue.front()) {
   else  {
      if(need_buffer) {
	 //couldn't get a buffer for output spooling and there was nothing
	 //else for ch3 3 to do, so set the service back to output
	 //spooling and restart the channel
	 //startChannelThreeDevice();
	 //channel3.setCurrentService( OutSpool );
      }
      else
	 // There is nothing for channel 3 to do.
	 channel3.setCurrentService(None) ;
   }
}


void AOperatingSystem::scheduler(boolean TERMINATE) {
// THis routine implements round-robin process scheduling. Any scheduling
// algorithm could be inserted here.

  PCB *intrProcess=NULL;
  PCB *active=NULL;

  cpu.set_TI( 0 );


  if(readyQue.empty() == FALSE ){

     // If TERMINATE = TRUE, the next node in the ready Q (the one that
     // followed the active process that was terminated) will be removed
     // from the front of the ready Queue, and placed on the rear. It will
     // be passed over. Therefore, if TERMINATE is TRUE, don't remove the
     // front node, since it was done by the terminate service, and put on
     // the terminate queue.

     if (TERMINATE) {
	active = readyQue.front();
	active->set_blocked(FALSE);

	if(active==NULL) {
	    cout << "ERROR IN SCEDULER - 1";
	    exit(1);
	}

	// Reset the TERMINATE flag
	TERMINATE = FALSE;
     }
     else {
	intrProcess = readyQue.remove();
	intrProcess->set_blocked(FALSE);

	if(intrProcess==NULL) {
	    cout << "ERROR IN SCEDULER - 2";
	    exit(1);
	}

	if(traceFile) {
	     traceFile << "   Process #" << intrProcess->getPID();
	     traceFile << " Moved to back of ready queue" << endl;
	}
	// Save the current state.
	// Do not resave the PTR, sonce it's value is constant for the life
	// of the process.
	intrProcess->putTimeSlice( TIME_SLICE_VALUE );
	intrProcess->putC( cpu.get_C() );
	intrProcess->putIC( cpu.get_IC());
	intrProcess->putR(cpu.get_R());

	readyQue.insert( intrProcess );

	// Round robin takes the first PCB on the ready queue
	active = readyQue.front();
	active->set_blocked(FALSE);

	if(active==NULL) {
	    cout << "ERROR IN SCEDULER - 3";
	    exit(1);
	}

     }

     // If a newly created process has not yet run, and there is no available
     // memory, then we must wait unitl there is at least one free frame, so
     // that local page replacement can occur.
     while (active->executionTime() == 0) {
	if ((readyQue.length() == 1) && (frameTab.get_free_frames() < 1)) {
	   active->set_blocked(TRUE);
	   break;
	}

	if (frameTab.get_free_frames() < 1) {
	   active = readyQue.remove();
	   if(active==NULL) {
	      cout << "ERROR IN SCEDULER - 4";
	      exit(1);
	   }
	   readyQue.insert(active);
	   active = readyQue.front();
	   if(active==NULL) {
	     cout << "ERROR IN SCEDULER - 5";
	     exit(1);
	   }
	}
	else
	   break;
     }

     // update the cpu register with the new process
     // state.
     cpu.set_C(active->getC());
     cpu.set_IC( active->getIC() );
     cpu.set_R( active->getR() );
     cpu.set_PI(0);
     cpu.set_SI(0);
     cpu.set_TI(0);
     cpu.set_PTR( active->getPTR() );
  }
  else {
     // There is the boundary condition that a job has just terminated, but
     // the next job has not yet created it's PCB and placed it onto the
     // ready queue (it may still be input spooling). Therefore, reset the CPU
     // for the process dependent registers, so that when the next process
     // begins executing, the CPU will be initialized correctly.
     cpu.set_IC(0);
     cpu.set_TI(0);
     cpu.set_SI(0);
     cpu.set_PI(0);
     cpu.set_C(0);
  }
}


void AOperatingSystem::createProcessPageTable( long pid ) {
// This routine is called after channel 3 reads an EOJ buffer. It creates the
// PCB for the process, creates a page table and initializes all the entries
// to invalid, and inserts the PCB onto the ready queue.

    word base;
    long frame_number;
    long rc = -1;
    long pgLimit = -1;
    long totTime = -1;
    char tmp[41];
    char convert[5];
    PCB *pcbObj = NULL;

    // Get the $JOB card from the drum, so that total time and printline info
    // can be found. Since the program cards need to be read from the drum in a
    // random access fashion, specify program card 0 (for $JOB card).
    rc = fSys.getprogpage( tmp, 0, pid );

    if( !rc ) {
       // Get total time and print limits
       strncpy( convert, &tmp[8], 4);
       totTime = atoi( convert );

       strcpy( convert, "" );
       strncpy( convert, &tmp[12], 4);
       pgLimit = atoi( convert );

       pcbObj = new PCB;

       if(pcbObj==NULL) {
	   cout << "ERROR IN CREATPROCESSPAGETABLE()";
	   exit(1);
       }


       pcbObj->putPID( pid );
       pcbObj->putTotalTime( totTime );
       pcbObj->putPageLimit( pgLimit );
       pcbObj->putTimeSlice( TIME_SLICE_VALUE );
       pcbObj->putC(0);
       pcbObj->putIC(0);
       pcbObj->putIC(0);

       // Allocate memory for the page table., and update the PCB.
       if (frameTab.allocate_frame(frame_number, pid))
	  pcbObj->putPTR(frame_number);
       else {
	  cout << "PROCESS CREATE MEM FAIL" << endl;
	  exit(1);
       }

       frame_number = frame_number << 8;

       // Mark the page table entries as invalid, since we are implementing
       // pure demand paging.
       for (long i = 0; i < 10; i++)
	   mem.write_cell(frame_number + i, PAGE_INV_NOSWAP);

       // If the ready queue is empty, then schedule the newly created process.
       // To force the scheduler to get the value of the PTR from the PCB, set
       // TERMINATE = TRUE, otherwise, it will use the value of PTR from the
       // previous process. Only schedule the job the job if there are 1 or
       // more free frame. Swapping cannot occur unless there is at least one
       // free page allocated.
       if((readyQue.empty() == TRUE) && (frameTab.get_free_frames()) > 0) {
	  readyQue.insert( pcbObj );
	  scheduler(TRUE);
       }
       else if ((readyQue.empty() == TRUE) &&
		(frameTab.get_free_frames()) == 0) {
	  readyQue.insert( pcbObj );
	  pcbObj->set_blocked(TRUE);
       }       else
	  readyQue.insert( pcbObj );
    }
}


void AOperatingSystem::terminationService( long intr, queue_type q ) {

   PCB *terminate=NULL;
   char statline1[41];
   char statline2[41];
   char stringbuff[7];
   char statData[41];

   // Set the flag to TRUE so that the scheduler knows the proper course of
   // action to perform.
   TERMINATE = TRUE;


   // All the resources held by the procces is refurbished by the operating
   // system, and put on terminate Que. The terminating job is either on the
   // ready q, or the IO q.
   if(q == READYQ )
      terminate = readyQue.remove();
   else
      terminate = ioQue.remove();

   if(terminate==NULL) {
	cout << "ERROR IN TERMINATION SERVICE";
	exit(1);
   }

   if(traceFile) {
     traceFile << "  Moving process# " << terminate->getPID();
     traceFile << "  to the terminate queue " << endl;
   }

   // Deallocate all physical memory held by process.
   frameTab.deallocate_process( terminate->getPID() );

   // Clean up the interrupts (except for IOI ints, since they are process
   // independent), since they make no sense to be raised, because
   // the process is terminating.
   cpu.set_PI(0);
   cpu.set_SI(0);
   cpu.set_TI(0);

   switch( intr ){
      case INVALID_OPCODE_INT:
	 strcpy( statData, "OpCode InValid");

	 break;
      case INVALID_OPERAND_NONNUMERIC_INT:
	 strcpy( statData, "Operand Non Numeric" );

	 break;
      case INVALID_IC_OVERFLOW_INT:
	 strcpy( statData, "IC Overflow" );

	 break;
      case HALT_INT:
	 strcpy( statData, "Terminated Normally");

	 break;
      case TIMER_TOTAL_INT_DUP:
      case TIMER_BOTH_INT_DUP:
	 strcpy( statData, "Total Time Expired");

	 break;
      case PAGES_EXCEEDED_INT:
	  strcpy( statData,"Max Lines Exceeded");
	  break;

      case OUT_OF_DATA_INT:
	 strcpy( statData, "Out of Data");
	 break;

      default:
	 strcpy( statData, "Unknown Error");
	 break;
   }

   sprintf( statline1, "\n\nID# %d      ", terminate->getPID() );
   strcat(statline1,statData);

   sprintf(statline2,"IC=%d IR=",cpu.bcd_to_dec( cpu.get_IC() ));
   cpu.get_IR_string(stringbuff);
   strcat(statline2,stringbuff);
   strcat(statline2," ");
   int temp = terminate->executionTime();
   char t[4];
   itoa(temp,t,10);
   strcat(statline2,"EXE Time=");
   strcat(statline2,t);
   strcat(statline2," Pgs prnt=");
   temp = terminate->getPagesPrinted();
   itoa(temp,t,10);
   strcat(statline2,t);
   strcat(statline2,"\n\n");
   statline2[40] = '\0';

   // Store the stat cards in the PCB to be written later.
   terminate->store_stats(statline1, statline2);

   // If the terminate queue is empty, then no output spooling is occuring.
   // If the process currently being processed for termination has no pages
   // to print, then write the stat cards directly to the printer, and do
   // not insert onto the term queue. Otherwise, insert the PCB onto the
   // terminate queue. When channel 2 int handler is called, let it print
   // the stat cards.
   //if (terminate->getPagesPrinted() == 0)
   //	statQue.insert(terminate);
   //else {
      //output spooling needs to be fucking done
      terminateQue.insert( terminate );

      //if(cpu.get_CHST3()==0) {
	// Reserve an empty buffer. We don't want to waste 4 cycles if at
	// the end of 4 cycles, there are no empty buffers to make an
	// input full buffer.
      //	reserved_output_buffer = buffers.GetEmptyBuffer();

      //	if (reserved_output_buffer != NULL) {
      //	  startChannelThreeDevice();
      //	  channel3.setCurrentService( OutSpool );
      //	}
     // }


   // Since the process is terminating, schedule the next process to run.
   scheduler(TRUE);
}


void AOperatingSystem::page_to_mem(long frame, char* data) {
// This routine takes a page worth of data, and writes it to a frame in
// primary memory.

  long word_value;
  long byte_ctr = 0;
  long word_ctr = 0;


  // Add a zero to the frame number (i.e. frame 11 becomes frame 110,
  // a physical address)
  frame = frame << 8;

  while( byte_ctr < 40 ){
     // Get a byte from the buffer, shift left one byte, then
     // add the next byte, shift etc., until 4 bytes hasve been extracted from
     // the buffer.
     word_value = data[byte_ctr++];
     word_value = word_value << 8;
     word_value = word_value + data[byte_ctr++];
     word_value = word_value << 8;
     word_value = word_value + data[byte_ctr++];
     word_value = word_value << 8;
     word_value = word_value + data[byte_ctr++];

     mem.write_cell( frame + word_ctr++, word_value);
  }
}


void AOperatingSystem::mem_to_page(long frame, char* data) {
// This routine takes a page worth of data from memory, and writes it to
// a buffer, so that it may be written to disk.

  long  word_value;
  long *long_ptr;
  char *str_ptr;
  char  temp[5];
  char *temp_ptr;
  long  word_ctr = 0;


   // Add a zero to the frame number (i.e. frame 11 becomes frame 110,
   // a physical address)
   frame = frame << 8;

   temp[4] = '\0';
   str_ptr = data;
   long_ptr = &word_value;

   // Convert 10 long values into one string value
   while( word_ctr < 10 ){

      word_value = mem.read_cell( frame + word_ctr++);

      memcpy(temp, (void*) long_ptr, sizeof(long));

      // After the memcpy, the data is in reverse order from what we want, so
      // reverse it.
      temp_ptr = strrev(temp);
      memcpy(str_ptr, temp_ptr, sizeof(long));

      str_ptr = str_ptr + sizeof(long);
   }
   data[40] = '\0';
}
