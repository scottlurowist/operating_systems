//  File    globals.h


#ifndef GLOBALS_H
#define GLOBALS_H

// Used for conditional compilation. This compilation is done to make the
// code portabal to Visual Age C++ on OS/2, or Borland C++ on MS-DOS.
// The portability is affected by the size of data types in the two
// environments

#define VACPP
//#define BCPP


typedef enum {FALSE, TRUE} boolean;

// MOS CPU data sizes
typedef char  one_byte;
typedef char  page;

#ifdef VACPP
   typedef short frame_addr;
   typedef short two_byte;
   typedef short address;

#endif

#ifdef BCPP
   typedef int frame_addr;
   typedef int two_byte;
   typedef int address;
#endif

typedef long   word;

const long MASTER = 0;
const long SLAVE = 1;

const long TIME_SLICE_VALUE = 10;

const  long    ZERO = 48;             // ASCII value for the character 0


//    Valid/Invalid bits for checking page table entries
const  long  PAGE_INV_NOSWAP = 0x40000000;  // Page fault & not swapped to disk
const  long  PAGE_INV_SWAP   = 0x80000000;  // Page fault & swapped to disk
const  long  PAGE_VALID      = 0xC0000000;  // Page in memory & not on disk


// MOS Interrupt values. The interrupt PROTECTION_INT is set in the PI
// register when a page is requested that is greater
const long INVALID_OPCODE_INT              = 1;   // Set in PI register
const long INVALID_OPERAND_NONNUMERIC_INT  = 2;   // Set in PI register
const long PAGE_FAULT_NOSWAP_INT           = 3;   // Set in PI
const long PAGE_FAULT_SWAP_INT             = 4;
const long INVALID_IC_OVERFLOW_INT         = 5;   // Set in PI register
const long GD_INT              = 1;   // Set in SI register
const long PD_INT              = 2;   // Set in SI register
const long HALT_INT            = 3;   // Set in SI register
const long PAGES_EXCEEDED_INT  = 4;
const long OUT_OF_DATA_INT     = 10;

// Note that channel interrupts are summed, since each channel can raise an
// interrrupt in parallel with another channnel. The interrupts are set when
// the respective channel becomes busy. Reset when the channel is not busy.
const long CHAN1_INT           = 1;
const long CHAN2_INT           = 2;
const long CHAN3_INT           = 4;
const long CHAN1_2_INT         = 3;
const long CHAN1_3_INT         = 5;
const long CHAN2_3_INT         = 6;
const long CHAN1_2_3_INT       = 7;


// This interrupt is set by the timer when a time slice has expired
const long TIMER_SLICE_INT     = 1;
const long TIMER_TOTAL_INT     = 2;
const long TIMER_BOTH_INT      = 3;

// This is for termination error report.
const long TIMER_TOTAL_INT_DUP     = 6;
const long TIMER_BOTH_INT_DUP      = 7;

// The number of columns on a card, the size of a disk sector, the number of
// bytes sent to the printer
const long  NUMOFCOLUMNS = 40;

//The types of pages that can be stored on the drum
enum pagetype {INPUT,OUTPUT,PROGRAM,SWAP,STAT};

//some queue error definitions
enum queue_error {UNDERFLOW,EMPTYQUEUE,NONEXISTANCE,NONEMPTYQUEUE};

typedef enum {IFB_MADE, NO_IFB_MADE, EOJ_IFB_MADE} ifb_type;
typedef enum {LOAD_PROGRAM_CARD, LOAD_DATA, PUT_DATA, PUT_SWAP, GET_SWAP,
	      PUT_GET_SWAP} service_type;
#endif
