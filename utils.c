#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitExtender(unsigned int field, unsigned int size) {
  //Creo que esto aplicaria para inmediatos y offsets, normalmente son de 12 bits
  //TODO: Extender para fields que no sean de 12 bits
  u_int32_t mask = 0; // 1000_0000_0000, por alguna razon el binario no funciona...
  int imm = 0;
  switch(size){
    case 5:
      mask = 0x10;//mascara 1_0000 0x10
      break;
    case 12:
      mask = 0x800; // 1000_0000_0000, por alguna razon el binario no funciona...
      break;
    //TODO: Por qué unos fiels se extienden bien con 12 y otros con 13?
    //en teoria si el field es de 12, no deberia ser 12 el signo siempre?
    case 13:
      mask = 0x1000; //1 0000 0000 0000  bit 13 es signo;
      break;
    case 21:
      mask = 0x100000 ; //0001 0000 0000 0000 0000 0000
      break;
    //Sign extension para operandos que van a registros Byte, half, word.
    case 8:
      mask = 0x80; // 1000_0000 0x80, por alguna razon el binario no funciona...
      break;
    case 16:
      mask = 0x8000; // 1000_0000_0000_0000 0x8000, por alguna razon el binario no funciona...
      break;
    case 32:
      mask = 0x80000000; // 1000_0000_0000_0000_0000_0000_0000_0000 0x80000000, por alguna razon el binario no funciona...
      break;
    default:
      break;
  } 
  imm = (int32_t)(field ^ mask) - mask;
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
  return bitExtender(operando, 21);
}

/* Offset in BYTES */
/* Used both in load and store */

  int get_memory_offset(Instruction instruction) {
    int upper = instruction.stype.imm7;
    int lower = instruction.stype.imm5;
    switch(instruction.stype.opcode){
      case 0x23:
        //Esto sirve para los s-type, pero no para los b-type
        upper &= 0x7F; //mascara 0111 1111 0x7F
        upper <<=  5;
        lower &= 0x1F; //mascara 0001 1111 0x1F
        return bitExtender(upper|lower, 12);
        break;
      case 0x63:
        int result = 0; //r_X para indicar posiciones dentro de r, que es el offset calculado
        //imm[12|10:5] imm[4:1|11] opcode B-type
        result = ((upper >> 0x6) & 0x1) << 0xC;// Saco bit imm7_7, colooc pos r_12
        result |= (upper & 0x3F) << 0x5;  // Saco bits imm_0-6 0011 1111, muevo a pos r_5 para llenar hasta r_11
        result |= (lower & 0x1E); // Saco bits imm5_1-4 0001 1110 0x1E, muevo a pos r_1 hasta r_r4
        result |= (lower & 0x1) << 0xB; //saco imm5_0 coloco r_11
        return bitExtender(result, 13);
        break;  
    }
    return -1;
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
