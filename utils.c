#include "utils.h"
#include <stdio.h>
#include <stdlib.h>


//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitExtender(unsigned int field, unsigned int size) {
  /* YOUR CODE HERE */
  return 0;
}


/* Distances in BYTES */

int get_branch_distance(Instruction instruction) {
  /* YOUR CODE HERE */
  return 0;
}


int get_jump_distance(Instruction instruction) {
  /* YOUR CODE HERE */
  return 0;
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
