// File cpu.cpp
// Scott Lurowist
//
// This file implements the cpu class


#include "cpu.h"
#include <iostream.h>
#include <stdlib.h>
#include <string.h>



moscpu::moscpu(primarymemory* pm) {

   primem = pm;

   // Initialize all the CPU registers
   C     = 0;
   R     = 0;
   IR    = 0;
   IC    = 0;
   PI    = 0;
   SI    = 0;
   IOI   = 0;
   TI    = 0;
   PTR   = 0;
   CHST1 = 0;
   CHST2 = 0;
   CHST3 = 0;
   MODE  = 0;

   IC_RESET = FALSE;
}


two_byte moscpu::bcd_to_dec(two_byte bcd) {
// Convert a 2 digit BCD number to it's decimal equivalent

   two_byte   dec_equiv;

   dec_equiv = bcd;
   dec_equiv = (dec_equiv >> 8) * 10;       // Mult most-sig-byte by 10
   dec_equiv = dec_equiv + (bcd & 0xFF);    // Mask LSB, and add

   return dec_equiv;
}


word moscpu::bcd_to_dec(word addr) {

   long  dec_equiv;   // converted number
   long  ms_byte;     // most significant byte
   long  mid_byte;    // middle byte
   long  ls_byte;     // least significant byte

   ms_byte = addr;
   ms_byte = ms_byte >> 16;   // Get the MSB

   mid_byte = addr;
   mid_byte = mid_byte & 0x0000FF00;    // Mask off the middle byte
   mid_byte = mid_byte >> 8;            // Shift it

   ls_byte = addr & 0x000000FF;         // Mask off the LSB

   dec_equiv = (100 * ms_byte) + (10 * mid_byte) + ls_byte;

   return dec_equiv;
}


void moscpu::inc_IC() {
// This method increments the two digit IC counter, which is BCD

   two_byte  ms_byte;    // The most significant byte
   two_byte  ls_byte;    // The least significant byte


   ms_byte = IC;            // Get the ms_byte
   ms_byte = ms_byte >> 8;

   ls_byte = IC & 0xFF;

   IC_RESET = FALSE;

   // Check if the IC counter will attempt to count past 99
   if ((ms_byte == 9) && (ls_byte == 9)) {
      IC = 0;
      IC_RESET = TRUE;
   }
   else if (ls_byte == 9)
      // Carry the one, and reset the ls_byte to zero
      IC = (IC + 0x0100) & 0xFF00;
   else
      IC = IC + 1;
}


void moscpu::dec_IC() {
// This method decrements the two digit IC counter, which is BCD

   two_byte  ms_byte;    // The most significant byte
   two_byte  ls_byte;    // The least significant byte


   ms_byte = IC;            // Get the ms_byte
   ms_byte = ms_byte >> 8;

   ls_byte = IC & 0xFF;

   if ((ms_byte == 0) && (ls_byte == 0))
      IC = 0x0909;
   else if (ls_byte == 0)
      // Subtract one from the tens digit, and reset the ls_byte to nine
      IC = (IC - 0x0100) | 0x0009;
   else
      IC = IC - 1;
}


page moscpu::pre_fetch() {
// This method gets the contents of the instruction counter. It then returns
// the page portion of the address, so that the paging hardware can resolve
// the page for the instruction pointed to by the IC. The value stored in the
// IC is BCD, but IS NOT ascii encoded.

   page     page_number;
   address  temp = IC;

   page_number = temp >> 8;

   return page_number;
}


page moscpu::fetch(frame_addr frame_base) {
// This method gets the machine instruction from primary memory. The
// operand is a logical address, so the page portion of the address is
// returned so that the paging hardware can resolve it. The address passed in
// is the base address of the frame where the instruction resides.

   page page_number;
   long operand_MSB;
   long operand_LSB;


   // Get the frame displacement for the instruction. It is the LSB in the
   // IC register. The page is the MSB.
   displacement = IC & 0xFF;

   // The effective address is the page + displacement. It is BCD
   effective_address = frame_base;
   effective_address = (effective_address << 8) + displacement;
   IC_real = (address) ((bcd_to_dec((two_byte)frame_base) * 10) +
                         displacement);

   // Store the complete instruction (opcode & operand) in the instruction
   // register
   IR = primem->read_cell(effective_address);

   // Decode the instruction. The opcode is in the first two bytes, the
   // operand is in the last two bytes of the four byte word.
   opcode = IR;
   opcode = opcode >> 16;
   operand = IR;
   operand = operand & 0xFFFF;
   operand_LSB = (operand & 0xFF) - 0x30;
   operand_MSB = ((operand >> 8) & 0xFF) - 0x30;
   operand_decimal = bcd_to_dec(operand -  0x3030);

   // Check that the operand is valid (that it is not non-numeric)
   if ((operand_LSB < 0) || (operand_LSB > 9) || (operand_MSB < 0) ||
       (operand_MSB > 9)) {
      PI = INVALID_OPERAND_NONNUMERIC_INT;
      return 0;
   }

   // Get the page number and displacement from the operand. The page number
   // is the most sig byte of the operand; the displacement is the LSB. Both
   // BCD digits are stored as their ascii values. For example, the BCD digit
   // 1 is stored as the value 49 in primary memory. Convet to it's normal
   // BCD value so that it is compatable with the primary memory's addressing
   // format.
   displacement = (operand & 0xFF) - ZERO;
   page_number = (operand >> 8) - ZERO;

   // Increment the instruction counter
   inc_IC();

   // The operand is an address that must be resolved by the paging system
   return page_number;
}


page moscpu::execute(frame_addr frame_base) {
// This method executes a MOS cpu instruction. This routine simulates the
// microcode of the CPU. The address passed in is the base address of the
// frame where the operand of the instruction resides. The

    // The effective address is the page + displacement. It is BCD
    effective_address = frame_base;
    effective_address = (effective_address << 8) + displacement;
    effective_address_converted = bcd_to_dec(effective_address);


    // If DEBUG CPU is defined, print out to the screen the logical memory
    // location, the opcode, and the operand for debug purposes

    // If the IC counter reset(tried to count past 99), and the opcode is a
    // BT, then program execution WILL NOT continue with an instruction at
    // memory location 100 (an obvious error). If the opcode IS NOT a BT,
    // then there is an error condition, since logical memory is 0-99.
    if ((opcode != BT) && (IC_RESET)) {
       PI = INVALID_IC_OVERFLOW_INT;
       return 0;
    }

    switch (opcode) {
        // Load the contents of location physical_address into the R Register
       case LR:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  LR  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          if (PI == 0)
             R = primem->read_cell(effective_address);

          strcpy(mnemonic, "LR");

          break;
       // Store the contents of the R register into location physical_address
       case SR:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  SR  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          if (PI == 0)
             primem->write_cell(effective_address, R);

          strcpy(mnemonic, "SR");

          break;
       // Read the contents of location physical_location, and compare to the
       // R register. If equal, set the C register
       case CR:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  CR  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          if (PI == 0) {
             if (R == primem->read_cell(effective_address))
                C = 1;            // Set the compare register to TRUE
          }

          strcpy(mnemonic, "CR");

          break;
       // If the C register is TRUE, then the CR instruction resulted in a
       // TRUE comparison. Load the instruction register with the operand, as
       // this location is the destination of the branch, and will be the
       // next instruction fetched.
       case BT:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  BT  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          // These are used if a BT is executed, and the time slice is up, to
          // restore the CPU to the state before the branch
          pre_BT_IC = IC;
          pre_BT_C  = 0;

          if (C == 1) {
             // The operand is ascii-encoded BCD. Convert BCD to decimal,
             // then subtract ascii 0 from both bytes.
             IC = operand -0x3030;
             pre_BT_C = 1;
             C = 0;         // Reset the toggle register
          }
          else {
             // If the IC counter reset, and the branch is false, then
             // execution would want to continue with instruction 100 (an
             // error).
             if (IC_RESET == TRUE) {
                PI = INVALID_IC_OVERFLOW_INT;
                return 0;
             }
          }

          strcpy(mnemonic, "BT");

          break;
       // Print the data to the printer
       case PD:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  PD  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          SI= PD_INT;

          strcpy(mnemonic, "PD");

          break;

       // Get data from the drum
       case GD:
          #ifdef DEBUG_CPU
             cout << bcd_to_dec(IC) - 1 << "  GD  " <<
                     bcd_to_dec(operand -  0x3030) << endl;
          #endif

          SI = GD_INT;

          // If the PI is set, it means that the operand page faulted. Since
          // this is a GD, it SHOULD do that. Therefore, reset it to zero,
          // since it is meaningless.
          //PI = 0;

          strcpy(mnemonic, "GD");

          break;
       // Invalid opcode
       default:
          // The HALT instruction is unique in that it is one character. Check
          // if the MSB of the opcode is 0x64; otherwise, the opcode is
          // invalid.
          // The user wants to end the program
          long HALT = opcode >> 8;

          if (HALT == H) {
             #ifdef DEBUG_CPU
                cout << bcd_to_dec(IC) << "  H" << endl;
             #endif

             SI = HALT_INT;
             PI = 0;             /// HALT will raise non-numeric interrupt

             strcpy(mnemonic, "H");
          }
          else {
             #ifdef DEBUG_CPU
                cout << "INVALID" << endl;
             #endif

             PI = INVALID_OPCODE_INT;

             strcpy(mnemonic, "xx");
          }
    }

    operand = (operand >> 8) - ZERO;

    return operand;
}


void moscpu::get_IR_string(char* value) {
   char buffer[5];
   long result;

   if ((result = strcmp("H", mnemonic)) != 0) {
      strcpy(value, mnemonic);
      strcat(value, itoa(operand_decimal, buffer, 10));
   }
   else
      strcpy(value, mnemonic);
}
