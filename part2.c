#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"


// forward declarations
void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jalr(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_auipc(Instruction, Processor *);
void execute_lui(Instruction, Processor *);


void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {
  printf("Me pasaron: %.8x\n que decofica: ", instruction.bits);
  decode_instruction(instruction);
  // exit(666);
  int opcode = instruction.opcode;
  switch(opcode) { // What do we switch on?
    case 0x33:
      execute_rtype(instruction, processor);
      break;
    case 0x13:
      execute_itype_except_load(instruction, processor);
      break;
    case 0x03:
      execute_load(instruction, processor, memory);
      break;
    case 0x73:
      execute_ecall(processor, memory);
      break;
    case 0x63: //beq, bne
      execute_branch(instruction, processor);
      break;
    case 0x37:
      execute_lui(instruction, processor);
      break;
    case 0x6F:
      execute_jal(instruction, processor);
      break;
    case 0x23:
      execute_store(instruction, processor, memory);
      break;
    case 0x17:
      execute_auipc(instruction, processor);
      break;
    case 0x67:
      execute_jalr(instruction, processor);
      break;
    default: // undefined opcode
      handle_invalid_instruction(instruction);
      exit(1);
      break;
  }
  processor->PC = processor->PC + 4; //Incremento PC Siempre/solo cuando logré ejecutar
}


void execute_rtype(Instruction instruction, Processor *processor) {
  int func3 = instruction.rtype.funct3;
  int func7 = instruction.rtype.funct7;
  int rd = instruction.rtype.rd;
  int rs1 = instruction.rtype.rs1;
  int rs2 = instruction.rtype.rs2;
  switch(func3) { 
    case 0x0: // add, mul, sub
        //Feo switch nesteado xD
       switch (func7) {
        case 0x0: //add
            //R[rd]<-R[rs1] + R[rs2]
            processor->R[rd] = processor->R[rs1] + processor->R[rs2];
        break;
        case 0x01: //mul
            //R[rd]<-(R[rs1] * R[rs2]) [31:0]
            processor->R[rd] = (processor->R[rs1] * processor->R[rs2]) & 0xFFFFFFFF; // Potencial BUG: hay que hacer bit extension?
        case 0x20: //sub
            //R[rd]<-R[rs1] - R[rs2]
            processor->R[rd] = processor->R[rs1] - processor->R[rs2];
            break;
        //Termino switch nesteado para Rtype
       
       default:
        handle_invalid_instruction(instruction);
        exit(-1);
        break;
       }
        break;
    case 0x1: //sll, mulh
        switch(func7) {
            case 0x0: //sll
              //R[rd]<-R[rs1] << R[rs2]
              processor->R[rd] = processor->R[rs1] << processor->R[rs2];
              break;
            case 0x01: //mulh
              processor->R[rd] = (processor->R[rs1] * processor->R[rs2]) >> 0x1F; //Descarto los primeros 32 bits;
              //R[rd]<-(R[rs1] * R[rs2]) [63:32]
              break;
        }
        break;
    case 0x2: //slt
        printf(RTYPE_FORMAT, "slt",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
            );
        break;
    // case 0x3: 
    //     //Creo que este  func3 no existe.
    //     handle_invalid_instruction(instruction);
    //     break;
    case 0x4: //xor, div
        switch(func7) {
            case 0x0:
                printf(RTYPE_FORMAT, "xor",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                );
                break;
            case 0x01:
                printf(RTYPE_FORMAT, "div",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                 );
                break;
        }
        break;
    case 0x5: //srl, sra
        switch(func7) {
            case 0x0:
                printf(RTYPE_FORMAT, "srl",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                );
                break;
            case 0x20:
                printf(RTYPE_FORMAT, "sra",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                 );
                break;
        }
        break;
    case 0x6: //or, rem
        switch(func7) {
            case 0x0:
                printf(RTYPE_FORMAT, "or",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                );
                break;
            case 0x01:
                printf(RTYPE_FORMAT, "rem",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                 );
                break;
        }
        break;
    case 0x7: //and
        printf(RTYPE_FORMAT, "and",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
        );
        break;
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void execute_itype_except_load(Instruction instruction, Processor *processor) {
  int func3 = instruction.itype.funct3;
  int rd = instruction.itype.rd;
  int rs1 = instruction.itype.rs1;
  int imm = instruction.itype.imm;
  switch(func3) { // What do we switch on?
    case 0x0: //addi
      //R[rd]<- R[rs1] + imm 
      processor->R[rd] = processor->R[rs1] + imm;
      break;
    case 0x07:
      printf(ITYPE_FORMAT,
      "andi",
      instruction.itype.rd,
      instruction.itype.rs1,
      bitExtender(instruction.itype.imm,12)
      );
      break;
    case 0x06:
      printf(ITYPE_FORMAT,
      "ori",
      instruction.itype.rd,
      instruction.itype.rs1,
      bitExtender(instruction.itype.imm, 12)
      );
      break;
    case 0x02:
      printf(ITYPE_FORMAT,
      "slti",
      instruction.itype.rd,
      instruction.itype.rs1,
      bitExtender(instruction.itype.imm, 12)
      );
      break;
      case 0x01:
      printf(ITYPE_FORMAT,
      "slli",
      instruction.itype.rd,
      instruction.itype.rs1,
      instruction.itype.imm
      );
      break;
      case 0x4:
      printf(ITYPE_FORMAT,
        "xori",
        instruction.itype.rd,
        instruction.itype.rs1,
        bitExtender(instruction.itype.imm, 12)
        );
        break;
      case 0x5:
        int imm = instruction.itype.imm;
        imm >>= 5;
        imm &= 0x7F; //Mascara 0111 1111
        switch (imm)
        {
        case 0x00:
          printf(ITYPE_FORMAT,
          "srli",
          instruction.itype.rd,
          instruction.itype.rs1,
          // bitExtender(instruction.itype.imm & 0x1F, 5) //mascara 0001_1111
          //Sin sing extension
          instruction.itype.imm & 0x1F
          );
          break;
        case 0x20:
          printf(ITYPE_FORMAT,
          "srai",
          instruction.itype.rd,
          instruction.itype.rs1,
          instruction.itype.imm & 0x1F
          );
          break;
        default:
          handle_invalid_instruction(instruction);
          break;
        }
      break;
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void execute_ecall(Processor *p, Byte *memory) {
  int ecall_n = p->R[10]; //a0=x10 holds ecall number
  int arg1 = p->R[11]; //a1=x11 Es el primer argumento de la syscall
  switch(ecall_n) { // What do we switch on?
    case 1: //ecall write
      printf("%d",arg1); 
    case 10: //ecall exit
      exit(arg1);
    default: // undefined ecall
      printf("Illegal ecall number %d\n", ecall_n); // What stores the ecall arg?
      exit(-1);
      break;
  }
}


void execute_branch(Instruction instruction, Processor *processor) {
  /* Remember that the immediate portion of branches
     is counting half-words, so make sure to account for that. */
  int func3 = instruction.btype.funct3;
  int rs1 = instruction.btype.rs1;
  int rs2 = instruction.btype.rs2;
  switch(func3) { // What do we switch on?
    case 0x0:
      //if(R[rs1] == R[rs2]) PC<- PC + {offset, 1b'0}
      // printf("%.8x Branching to %d ", instruction.bits, get_memory_offset(instruction));
      // decode_instruction(instruction);
      if(processor->R[rs1] == processor->R[rs2]) processor->PC = processor->PC +  get_memory_offset(instruction) - 4;//Afuera siempre incremento PC, entonces compenso aca;
    break;
    case 0x01:
    //if(R[rs1] != R[rs2]) PC<- PC + {offset, 1b'0}
      // printf("%.8x Branching to %d ", instruction.bits, get_memory_offset(instruction));
      // decode_instruction(instruction);
      if(processor->R[rs1] != processor->R[rs2]) processor->PC = processor->PC +  get_memory_offset(instruction) - 4;//Afuera siempre incremento PC, entonces compenso aca;
    break;
    default:
      handle_invalid_instruction(instruction);
      exit(-1);
      break;
  }
}


void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
  switch(0) { // What do we switch on?
    /* YOUR CODE HERE */
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
  switch(0) { // What do we switch on?
    /* YOUR CODE HERE */
    default:
      handle_invalid_instruction(instruction);
      exit(-1);
      break;
  }
}


void execute_jalr(Instruction instruction, Processor *processor) {
  /* YOUR CODE HERE */
}


void execute_jal(Instruction instruction, Processor *processor) {
  /* YOUR CODE HERE */
}


void execute_auipc(Instruction instruction, Processor *processor) {
  /* YOUR CODE HERE */
}


void execute_lui(Instruction instruction, Processor *processor) {
  /* YOUR CODE HERE */
}


/* Checks that the address is aligned correctly */
int check(Address address, Alignment alignment) {
  if (address > 0 && address < MEMORY_SPACE) {
    // byte align
    if (alignment == LENGTH_BYTE) return 1;
    // half align
    if (alignment == LENGTH_HALF_WORD) return ((address % 2) == 0);
    // word align
    if (alignment == LENGTH_WORD) return ((address % 4) == 0);
  }
  return 0;
}


void store(Byte *memory, Address address, Alignment alignment, Word value, int check_align) {
  if ((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
    handle_invalid_write(address);
  }
  /* YOUR CODE HERE */
}

/** 
 * Notas: La memoria ya se creo en riscv.c
**/
Word load(Byte *memory, Address address, Alignment alignment, int check_align) {
  if ((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
    handle_invalid_read(address);
  }
  //Aca estoy haciendo un fetch, entonces solo deberia retornar un word desde PC
  //PC ya me lo mandan en risc.v en esta linea   instruction.bits = load(memory, processor->PC, LENGTH_WORD, 1);
  // printf("Contents of memory 0x%.8x at  address 0x%.8x\n", memory[address], address);
  // exit(0);
return memory[address] | (memory[address+1] << 0x8) | (memory[address+2] << 0x10) | (memory[address+3] << 0x18);}
