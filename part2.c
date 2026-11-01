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
  // printf("Me pasaron: %.8x\n que decofica: ", instruction.bits);
  // decode_instruction(instruction);
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
    case 0x37: //lui
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
      // printf("PC at: %.8x\n", processor->PC);
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
                // R[rd]<-R[rs1] ^ R[rs2]
                processor->R[rd] = (processor->R[rs1] ^ processor->R[rs2]);
                break;
            case 0x01:
                //R[rd]<-R[rs1] / R[rs2]
                if(processor->R[rs2] == 0) handle_invalid_instruction(instruction); //No dividimos por cero
                processor->R[rd] = processor->R[rs1] / processor->R[rs2];
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
            case 0x0: //or
                //R[rd]<-R[rs1] | R[rs2]
                processor->R[rd] = (processor->R[rs1] | processor->R[rs2]);
                break;
            case 0x01: //rem
              //R[rd]<-R[rs1] % R[rs2]
              processor->R[rd] = ((int)processor->R[rs1] %  (int)processor->R[rs2]);
              // printf("RS1: %.8x RS2: %.8x intended %.8x\n",
              // processor->R[rs1],processor->R[rs2],
              // processor->R[rs1] % processor->R[rs2]);
              break;
        }
        break;
    case 0x7: //and
        //R[rd]<-R[rs1] & R[rs2]
        processor->R[rd] = processor->R[rs1] & processor->R[rs2]; 
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
  int imm = bitExtender(instruction.itype.imm, 12); //Hay que hacer sing extension a los inmediatos...
  switch(func3) { // What do we switch on?
    case 0x0: //addi
      //R[rd]<- R[rs1] + imm 
      processor->R[rd] = processor->R[rs1] + imm;
      break;
    case 0x07: //andi
    //R[rd]<- R[rs1] & imm
      processor->R[rd] = processor->R[rs1] & imm;
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
        case 0x00: //srli
          processor->R[rd] = 
          processor->R[rs1] >>  (instruction.itype.imm & 0x1F);
          break;
        case 0x20: //srai
          //R[rd]<- R[rs1] >> imm 
          processor->R[rd] = ((sWord) processor->R[rs1] >>  (instruction.itype.imm & 0x1F));
          //BUG: Por que imm viene corrupto aca? 
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
      break;
    case 10: //ecall exit
      printf("exiting the simulator\n");
      exit(0);
      break;
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
 int func3 = instruction.itype.funct3;
  int rd = instruction.itype.rd;
  int rs1 = instruction.itype.rs1;
  int offset = get_memory_offset(instruction);
  switch(func3) { // What do we switch on?
    case 0x0: //lb
      // R[rd]<- SignExt(Mem(R[rs1] + offset, byte)) 
      processor->R[rd] =  bitExtender(
        memory[processor->R[rs1] + offset], 8 
      );
      break;
    case 0x1: //lh
      // R[rd]<- SignExt(Mem(R[rs1] + offset, half)) 
      processor->R[rd] =  bitExtender(
        (memory[processor->R[rs1] + offset + 1] << 0x8) |
         memory[processor->R[rs1] + offset], 16 
      );

      break;
    case 0x2: //lw
      // R[rd]<- SignExt(Mem(R[rs1] + offset, byte)) ]
      // print_mem("lw", memory, processor, instruction);
      processor->R[rd] = 
       memory[processor->R[rs1] + offset +3 ]<< 0x18;

      processor->R[rd] |= 
        memory[processor->R[rs1] + offset+2] << 0x10;

      processor->R[rd] |= 
        memory[processor->R[rs1] + offset+1] << 0x08;

      processor->R[rd] |= 
        memory[processor->R[rs1] + offset]  << 0x00;
      break;
    default:
      handle_invalid_instruction(instruction);
      exit(-1);
      break;
  }
}


void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
  int func3 = instruction.stype.funct3;
  int rs1 = instruction.stype.rs1;
  int rs2 = instruction.stype.rs2;
  int offset = get_memory_offset(instruction);
  switch(func3) { // What do we switch on?
    case 0x0: //sb
      // Mem(R[rs1] + offset)<- R[rs2][7:0]
      memory[
        processor->R[rs1] + offset
      ] =  (Byte) (processor->R[rs2] & 0xFF);
      // print_mem("SB",memory, processor, instruction);
      break;
    case 0x1: //sh
      //Mem(R[rs1] + offset)<- R[rs2][15:0]
      memory[
        processor->R[rs1] + offset + 1
      ] =  (Byte) ((processor->R[rs2] & 0xFF00) >> 0x8);

      memory[
        processor->R[rs1] + offset
      ] =  (Byte) (processor->R[rs2] & 0xFF);
      // print_mem("SH", memory, processor, instruction);
      break;
    case 0x2: //sw
      //Mem(R[rs1] + offset)<- R[rs2]
       memory[
        processor->R[rs1] + offset + 3
      ] =  (Byte) ((processor->R[rs2] & 0xFF000000) >> 0x18);

      memory[
        processor->R[rs1] + offset + 2
      ] =  (Byte) ((processor->R[rs2] & 0xFF0000) >> 0x10);

       memory[
        processor->R[rs1] + offset + 1
      ] =  (Byte) ((processor->R[rs2] & 0xFF00) >> 0x8);

       memory[
        processor->R[rs1] + offset
      ] =  (Byte) (processor->R[rs2] & 0xFF);

      // print_mem("SW", memory, processor, instruction);
      break;
    default:
      handle_invalid_instruction(instruction);
      exit(-1);
      break;
  }
}


void execute_jalr(Instruction instruction, Processor *processor) {
    int rd = instruction.itype.rd;
    int imm = bitExtender(instruction.itype.imm, 12);
    int rs1 = instruction.itype.rs1;
    //1. R[rd]<- PC + 4
    processor->R[rd] = processor->PC + 4;
    //2. PC<- R[rs1] + imm
    processor->PC = processor->R[rs1] + imm;

}


void execute_jal(Instruction instruction, Processor *processor) {
    handle_invalid_instruction(instruction);
    printf("Not impl jal\n");
}


void execute_auipc(Instruction instruction, Processor *processor) {
    //R[rd]<- PC + {offset, 12b'0} Esto se parece mucho a lui
    //rd = PC + (imm << 12)
    int rd = instruction.utype.rd;
    int imm = instruction.utype.imm;
    processor->R[rd] = processor->PC + (imm << 12);
}


void execute_lui(Instruction instruction, Processor *processor) {
  //R[rd]<- {offset, 12b'0} 
  //rd = imm << 12
  int rd = instruction.utype.rd;
  unsigned  imm = instruction.utype.imm;
  processor->R[rd] = imm << 12;
  // printf("I lui'd %.8x\n", processor->R[rd]);
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
  memory[address] = (Byte) ((value & 0xFF000000) >> 0x18);
  memory[address+1] = (Byte) ((value & 0xFF0000) >> 0x10);
  memory[address+2] = (Byte) ((value & 0xFF00) >> 0x8);;
  memory[address+3] = (Byte) ((value & 0xFF));;
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
