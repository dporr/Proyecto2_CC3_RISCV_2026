#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitExtender(unsigned int field, unsigned int size) {
  //Creo que esto aplicaria para inmediatos y offsets, normalmente son de 12 bits
  //TODO: Extender para fields que no sean de 12 bits
  u_int32_t mask = 0x800; // 1000_0000_0000, por alguna razon el binario no funciona...
  int imm = 0;
  switch(size){
    case 12:
      imm = (int32_t)(field ^ mask) - mask;
      break;
    case 20:
      mask = 0x100000 ; //0001 0000 0000 0000 0000 0000
      imm = (int32_t)(field ^ mask) - mask;
      break;
    default:
      break;
  } 
  return imm;
}


/* Distances in BYTES */

int get_branch_distance(Instruction instruction) {
  /* YOUR CODE HERE */
  return 0;
}


int get_jump_distance(Instruction instruction) {
  //Yo crep que aca se calcula el imm del JAL...
  //BUG: Recalcular mascaras....
  int operando = instruction.jtype.imm;
  operando = (((operando >> 0x13) & 0x1) << 0x14 )| //Agarro solo bit 20: quito 0x13(19) del principio, hago AND parara preservar 1 y lo regreso a la pos 20.
    ((operando >> 0x9) & 0x3FF) << 0x1|  //Agarro bits 1:10:  con mascara 0011 1111 1111
    ((operando >> 0x8) & 0x1) << 0xB | //Agarro 11
    ((operando & 0xFF) << 0xC);  //Agarro 19:12, corro 11 >, mascara  11111 11111 (solo 8 bits)
  return bitExtender(operando, 20);
}

/* Offset in BYTES */
/* Used both in load and store */

int get_memory_offset(Instruction instruction) {
  /* YOUR CODE HERE */
  return 0;
}


void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}


void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}


void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}

//Pretty print  el payload en Bin y Hexa
void print_raw(Instruction instruction){
  printf("b");
  for (int i = 31; i >= 0; i--) {
    putchar(((instruction.bits >> i) & 1) ? '1' : '0');
    if (i == 16) putchar('_');
  }
  printf(" 0x%08x\n", instruction.bits);
}
