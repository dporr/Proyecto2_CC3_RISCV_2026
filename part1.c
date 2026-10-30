#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"


// forward declarations
void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction);
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_auipc(Instruction);
void write_lui(Instruction);
void write_jalr(Instruction);
void write_jal(Instruction);
void write_ecall(Instruction);


void decode_instruction(Instruction instruction) {
  /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
  int opcode = instruction.opcode;
//   printf("opcode %d %x:", opcode, opcode);
  // OPcodes que procesamos:
  //REF https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_GREEN_CARD.pdf
  //Tipo, Hex, Dec, Binario
  // TIPO R: 0x33: 51: 0110011
  // Tipo I: 0x13: 19: 0010011
  // Tipo I: 0x03: 3: 0000011
  // Tipo I: 0x73: 115: 1110011
  // Tipo I: 0x67: 103: 1100111
//   exit(0);
  switch(opcode) { // What do we switch on?
    case 0x33:
      write_rtype(instruction);
      break;
    case 0x13:
      write_itype_except_load(instruction);
      break;
    case 0x03:
    case 0x73:
      write_ecall(instruction);
      break;
    case 0x63:
      write_branch(instruction);
      break;
    case 0x37:
      write_lui(instruction);
      break;
    case 0x6F:
      write_jal(instruction);
      break;
    default: // undefined opcode
      handle_invalid_instruction(instruction);
      break;
  }
}


void write_rtype(Instruction instruction) {
    /** Ref del lab https://cc-3.github.io/projs/proj02/#parte-1
     * INSTRUCCIÓN 	        FUNCT3 	FUNCT7/IMM 	OPERACIÓN
        add rd, rs1, rs2 	0x0 	0x00	    R[rd]<-R[rs1] + R[rs2]
        mul rd, rs1, rs2 	0x0 	0x01 	    R[rd]<-(R[rs1] * R[rs2]) [31:0]
        sub rd, rs1, rs2 	0x0 	0x20 	    R[rd]<-R[rs1] - R[rs2]
        sll rd, rs1, rs2 	0x1 	0x00 	    R[rd]<-R[rs1] << R[rs2]
        mulh rd, rs1, rs2 	0x1 	0x01     	R[rd]<-(R[rs1] * R[rs2]) [63:32]
        slt rd, rs1, rs2 	0x2 	0x00    	R[rd]<-(R[rs1] < R[rs2]) ? 1 : 0
        xor rd, rs1, rs2 	0x4 	0x00    	R[rd]<-R[rs1] ^ R[rs2]
        div rd, rs1, rs2 	0x4 	0x01    	R[rd]<-R[rs1] / R[rs2]
        srl rd, rs1, rs2 	0x5 	0x00    	R[rd]<-R[rs1] >> R[rs2]
        sra rd, rs1, rs2 	0x5 	0x20    	R[rd]<-R[rs1] >> R[rs2]
        or rd, rs1, rs2 	0x6 	0x00 	    R[rd]<-R[rs1] | R[rs2]
        rem rd, rs1, rs2 	0x6 	0x01    	R[rd]<-R[rs1] % R[rs2]
        and rd, rs1, rs2 	0x7 	0x00    	R[rd]<-R[rs1] & R[rs2]
     */
  int func3 = instruction.rtype.funct3;
    // printf("%d %x", func3, func3);
  int func7 = instruction.rtype.funct7;
  switch(func3) { // What do we switch on?
    case 0x0:
        //Feo switch nesteado xD
       switch (func7) {
        case 0x0:
            // printf("ACA imprimo: \n");
            printf(RTYPE_FORMAT, "add",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
            );
        break;
        case 0x01:
            printf(RTYPE_FORMAT, "mul",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
        );
                    printf("\n");
            break;
        case 0x20:
            // printf("Aca imprimo\n");
            printf(RTYPE_FORMAT, "sub",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
        );
            printf("\n");
            break;
        //Termini swithc nesteado para Rtype
       
       default:
        break;
       }
        break;
    case 0x1:
        switch(func7) {
            case 0x0:
                printf(RTYPE_FORMAT, "sll",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                );
                break;
            case 0x01:
                printf(RTYPE_FORMAT, "mulh",
                instruction.rtype.rd,
                instruction.rtype.rs1,
                instruction.rtype.rs2
                 );
                break;
        }
        break;
    case 0x2:
        printf(RTYPE_FORMAT, "stl",
            instruction.rtype.rd,
            instruction.rtype.rs1,
            instruction.rtype.rs2
            );
        break;
    case 0x3:
        //Creo que este  func3 no existe.
        handle_invalid_instruction(instruction);
        break;
    case 0x4:
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
    case 0x5:
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
    case 0x6:
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
    case 0x7:
        printf(RTYPE_FORMAT, "and`",
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


void write_itype_except_load(Instruction instruction) {
  int func3 = instruction.itype.funct3;
  switch(func3) { // What do we switch on?
    case 0x0:
     //BUG: -- Fixed con bitExtender
     // < 0000101c: addi	x8, x8, -1 (esperado)
     // > 0000101c: addi	x8, x8, 4095 (Mi output)
      printf(ITYPE_FORMAT,
      "addi",
      instruction.itype.rd,
      instruction.itype.rs1,
      bitExtender(instruction.itype.imm, 12)
      );
      // print_raw(instruction);
      break;
    case 0x07:
      printf(ITYPE_FORMAT,
      "andi",
      instruction.itype.rd,
      instruction.itype.rs1,
      instruction.itype.imm
      );
      break;
    case 0x06:
      printf(ITYPE_FORMAT,
      "ori",
      instruction.itype.rd,
      instruction.itype.rs1,
      instruction.itype.imm
      );
      break;
    case 0x02:
      printf(ITYPE_FORMAT,
      "slti",
      instruction.itype.rd,
      instruction.itype.rs1,
      instruction.itype.imm
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
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void write_load(Instruction instruction) {
  switch(0) { // What do we switch on?
    /* YOUR CODE HERE */
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void write_store(Instruction instruction) {
  switch(0) { // What do we switch on?
    /* YOUR CODE HERE */
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


void write_branch(Instruction instruction) {
  int func3 = instruction.btype.funct3;
  switch(func3) { // What do we switch on?
    case 0x0:
    printf(BRANCH_FORMAT,
      "beq",
      instruction.btype.rs1,
      instruction.btype.rs2,
      //imm[12|10:5] rs2 rs1 funct3 imm[4:1|11] opcode B-type
      //Aca debo calcular un offset entre imm7 e imm5
      //Ref. https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_GREEN_CARD.pdf
      instruction.btype.imm5 
      //Por ahora funciona, pero seguro esto va a ser un bug
    );
    break;

    case 0x01:
    break;
    
    default:
      handle_invalid_instruction(instruction);
      break;
  }
}


/* For the writes, probably a good idea to take a look at utils.h */

void write_auipc(Instruction instruction) {
  /* YOUR CODE HERE */
}


void write_lui(Instruction instruction) {
  printf(LUI_FORMAT,
  instruction.utype.rd,
  instruction.utype.imm
  );
}


void write_jalr(Instruction instruction) {
  /* YOUR CODE HERE */
}


void write_jal(Instruction instruction) {
  printf(JAL_FORMAT,
  instruction.jtype.rd,
  //imm[20|10:1|11|19:12] rd opcode J-type
  // se debe armar un operando por partes
  // imm[20] + imm[10:1] + imm[11] + imm[19:12] = imm
  //Fix: Aparentemente siempre hay un 0, imm[0] == 0...
  get_jump_distance(instruction)
  );
}


void write_ecall(Instruction instruction) {
  printf(ECALL_FORMAT);
}


void print_rtype(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
}


void print_itype_except_load(char *name, Instruction instruction, int imm) {
  /* YOUR CODE HERE */
}


void print_load(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
}


void print_store(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
}


void print_branch(char *name, Instruction instruction) {
  /* YOUR CODE HERE */
}
