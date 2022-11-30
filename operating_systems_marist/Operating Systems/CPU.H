// File cpu.h
// Scott Lurowist
//
// This file contains declarations for a cpu object that would
// be used in the MOS simulation.

#ifndef MOSCPU_H
#define MOSCPU_H

#include "globals.h"
#include "primem.h"

// Used for conditional compilation in the CPU for debugging
#define  DEBUG_CPU


// CPU instruction set. The values are the ascii values of each character.
// For example ascii(L) = 0x4C; ascii(R) = 0x52; therefore LR = 0x4C52.
const  word   LR = 0x4C52;
const  word   SR = 0x5352;
const  word   CR = 0x4352;
const  word   BT = 0x4254;
const  word   GD = 0x4744;
const  word   PD = 0x5044;
const  word   H  = 0x48;


class moscpu {

   protected:
      primarymemory*  primem;  // Pointer to primary memory

      // MOS CPU Registers
      one_byte    C;      //
      word        R;
      word        IR;
      address     IC;
      one_byte    PI;
      one_byte    SI;
      one_byte    IOI;
      one_byte    TI;
      word        PTR;
      one_byte    CHST1;
      one_byte    CHST2;
      one_byte    CHST3;
      one_byte    MODE;

      address   IC_real;
      word      opcode;
      word      operand;

      // Restore the state of the CPU after a BT
      address   pre_BT_IC;
      one_byte  pre_BT_C;

      // The mnemonic form of the opcode
      char    mnemonic[3];

      // Holds the displacement portion of an effective address
      word      effective_address;
      two_byte  displacement;

      // The value of the operand and effective address, converted from ascii
      // encoded BCD. Needed for the trace output.
      word  operand_decimal;
      word  effective_address_converted;

      boolean  IC_RESET;

   public:
      moscpu(primarymemory* pm);
      ~moscpu() {};

      // convert a 2 and 3 digit BCD to it's decimal equivalent
      two_byte bcd_to_dec(two_byte bcd);
      word bcd_to_dec(word addr);

      // Increments the IC counter, since it's BCD
      void inc_IC();

      // Decrements the IC
      void dec_IC();

      // If there is not enough time to service a page fault, reset IC to
      // address of the branch instruction to re-execute it.
      void set_IC_pre_BT() {IC = pre_BT_IC;};
      void set_C_pre_BT() {C = pre_BT_C;};

      // These methods are used for the fetch/execute cycle
      page pre_fetch();
      page fetch(frame_addr frame_base);
      page execute(frame_addr frame_base);

      void get_opcode(char* oc) const {strcpy(oc, mnemonic);};
      word get_OPND() {return operand_decimal;};
      word get_effective_address() {return effective_address;};
      word get_effective_address_converted()
         {return effective_address_converted;};
      two_byte get_displacement() {return displacement;};

      // Register methods
      void set_C(one_byte value) {C = value;};
      one_byte get_C() const {return C;};

      void set_R(word value) {R = value;};
      word get_R() const {return R;};

      void set_IR(word value) {IR = value;};
      word get_IR() const {return IR;};
      void get_IR_string(char* value);

      void set_IC(address counter) {IC = counter;};
      address get_IC() const {return IC;};
      address get_IC_real() const{return IC_real;};

      void set_PI(one_byte value) {PI = value;};
      one_byte get_PI() const {return PI;};

      void set_SI(one_byte value) {SI = value;};
      one_byte get_SI() const {return SI;};

      void set_IOI(one_byte value) {IOI = IOI | value;};
      one_byte get_IOI() const {return IOI;};
      void clear_IOI(long bitmask) {IOI = IOI ^ bitmask;};


      void set_TI(one_byte value) {TI = value;};
      one_byte get_TI() const {return TI;};

      void  set_PTR(word counter) {PTR = counter;};
      word get_PTR() const {return PTR;};

      void set_CHST1(one_byte value) {CHST1 = value;};
      one_byte get_CHST1() const {return CHST1;};

      void set_CHST2(one_byte value) {CHST2 = value;};
      one_byte get_CHST2() const {return CHST2;};

      void set_CHST3(one_byte value) {CHST3 = value;};
      one_byte get_CHST3() const {return CHST3;};

      void set_MODE(one_byte value) {MODE = value;};
      one_byte get_MODE() const {return MODE;};

      long get_opcode() const {return opcode;};
};


#endif
