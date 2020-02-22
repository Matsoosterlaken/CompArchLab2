/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Matteus Oosterlaken
    Name 2: Emma Harper
    UTEID 1: mto436
    UTEID 2: elh2395
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction() ;

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)
#define Low8bits(x) ((x) & 0x000000FF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
            N,		/* n condition bit */
            Z,		/* z condition bit */
            P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/
int IR = 0;
void setcc(int DR) {
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.P = 0;
    if(NEXT_LATCHES.REGS[DR] == 0){//setcc();
        NEXT_LATCHES.Z = 1;
    } else if((NEXT_LATCHES.REGS[DR] & 0x8000)){
        NEXT_LATCHES.N = 1;
    } else {
        NEXT_LATCHES.P = 1;
    }
}

/*
if ((n AND N) OR (z AND Z) OR (p AND P))
PC = PCü + LSHF(SEXT(PCoffset9), 1);
 */
void BR(void){
    int BEN = ((CURRENT_LATCHES.N & ((IR & 0x0800) >> 11)) |
            (CURRENT_LATCHES.Z & ((IR & 0x0400) >> 10)) |
            (CURRENT_LATCHES.P & ((IR & 0x0200)>> 9)));


    if (BEN) {
        int PCoffset9 = IR & 0x01FF;
        PCoffset9 = PCoffset9 << 1;
        if (PCoffset9 & 0x0200) {
            PCoffset9 |= 0xFFFFFC00;
        }

        NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + PCoffset9) ;
    }
}



/*
 * if (bit[5] == 0)
DR = SR1 + SR2;
else
DR = SR1 + SEXT(imm5);
setcc();
 */
void ADD(void){
    int DR = (IR & 0x0E00)>>9;
    int SR1 = (IR & 0x01C0)>>6;
    int SR2 = IR & 0x0007;
    int imm5 = IR & 0x001F;
    SR1 = CURRENT_LATCHES.REGS[SR1];
    if (SR1 & 0x8000) {
        SR1 |= 0xFFFF0000;
    }
    SR2 = CURRENT_LATCHES.REGS[SR2];
    if (SR2 & 0x8000) {
        SR2 |= 0xFFFF0000;
    }

    if (IR & 0x0020) { //immediate
        if(imm5 & 0x0010){
            imm5 |= 0xFFFFFFE0;
        }
        NEXT_LATCHES.REGS[DR] = Low16bits(SR1 + imm5);

    } else{ //from source register
        NEXT_LATCHES.REGS[DR] = Low16bits(SR1 + SR2);

    }

    setcc(DR);


}

/*
 DR = SEXT(mem[BaseR + SEXT(boffset6)]);
setcc();
 */
void LDB(void){
    int DR = (IR & 0x0E00)>>9;
    int BR = (IR & 0x01C0)>>6;
    int boffset6 = IR & 0x003F;
    int byte = boffset6%2;
    if(boffset6 & 0x0020){
        boffset6 |= 0xFFFFFFC0;
        boffset6--;
    }
    int value = MEMORY[CURRENT_LATCHES.REGS[BR]/2 + boffset6/2][byte];
    if(value & 0x0080){
        value |= 0xFFFFFF00;
    }
    NEXT_LATCHES.REGS[DR] = Low8bits(value);

    setcc(DR);

}


/*
 mem[BaseR + SEXT(boffset6)] = SR[7:0];
 */
void STB(void){
    int SR = (IR & 0x0E00)>>9;
    int BR = (IR & 0x01C0)>>6;
    int boffset6 = IR & 0x003F;
    int byte = boffset6%2;
    if(boffset6 & 0x0020){
        boffset6 |= 0xFFFFFFC0;
        boffset6--;
    }

    int value = (CURRENT_LATCHES.REGS[SR]) & 0x00FF;
    MEMORY[(CURRENT_LATCHES.REGS[BR])/2 + (boffset6)/2][byte] = value;

}


/*
 R7 = PCë ;
if (bit[11] == 0)
PC = BaseR;
else
PC = PCë + LSHF(SEXT(PCoffset11), 1);
 */
//JSR LABEL
//JSRR BaseR
void JSR(void){
    int pcoffset11 = (IR & 0x07FF);
    int BR = (IR & 0x01C0)>>6;
    int temp = NEXT_LATCHES.PC;
    if(pcoffset11 & 0x0400){
        pcoffset11 |= 0xFFFFF800;
    }
    if(IR & 0x0800){
        pcoffset11 = pcoffset11 << 1;
        NEXT_LATCHES.PC += pcoffset11;
    } else{
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BR];
    }
    NEXT_LATCHES.REGS[7] = temp;

}


/*
if (bit[5] == 0)
DR = SR1 AND SR2;
else
DR = SR1 AND SEXT(imm5);
setcc();
 */
void AND(void){
    int DR = (IR & 0x0E00)>>9;
    int SR1 = (IR & 0x01C0)>>6;
    int SR2 = IR & 0x0007;
    int imm5 = IR & 0x001F;
    if (IR & 0x0020) { //immediate
        if(imm5 & 0x0010){
            imm5 |= 0xFFFFFFE0;
        }
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] & imm5;

    } else{ //from source register
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2];

    }

    setcc(DR);

}

/*
 DR = MEM[BaseR + LSHF(SEXT(offset6), 1)];
setcc();
 */
void LDW(void){
    int DR = (IR & 0x0E00)>>9;
    int BR = (IR & 0x01C0)>>6;
    int boffset6 = IR & 0x003F;
    if(boffset6 & 0x0020){
        boffset6 |= 0xFFFFFFC0;
    }
    //boffset6 = boffset6 << 1; // dont need to shift by 1 if divide by two below

    NEXT_LATCHES.REGS[DR] = MEMORY[(CURRENT_LATCHES.REGS[BR]/2 + boffset6)][0] + (MEMORY[(CURRENT_LATCHES.REGS[BR]/2 + boffset6)][1] << 8);

    setcc(DR);

}

/*
 MEM[BaseR + LSHF(SEXT(offset6), 1)] = SR;
 */
void STW(void){
    int SR = (IR & 0x0E00)>>9;
    int BR = (IR & 0x01C0)>>6;
    int boffset6 = IR & 0x003F;
    if(boffset6 & 0x0020){
        boffset6 |= 0xFFFFFFC0;
    }

    boffset6 = boffset6 << 1;

    int high_value = (CURRENT_LATCHES.REGS[SR]) & 0xFF00;
    high_value = high_value >> 8;
    int low_value = (CURRENT_LATCHES.REGS[SR]) & 0x00FF;
    MEMORY[CURRENT_LATCHES.REGS[BR]/2 + boffset6/2][1] = high_value;
    MEMORY[CURRENT_LATCHES.REGS[BR]/2 + boffset6/2][0] = low_value;


}

/*
 if (PSR[15] == 1) privilege mode violation
PC = MEM[R6]; R6 is the SSP
R6 = R6 + 2;
TEMP = MEM[R6];
R6 = R6 + 2;
PSR = TEMP; the privilege mode and condition codes of the interrupted process are restored
 */
void RTI(void){
    //DO NOT HAVE TO IMPLEMENT



}

/*
 if (bit[5] == 0)
DR = SR1 XOR SR2;
else
DR = SR1 XOR SEXT(imm5);
setcc();
 */
void XOR(void){
    int DR = (IR & 0x0E00)>>9;
    int SR1 = (IR & 0x01C0)>>6;
    int SR2 = (IR & 0x0007);
    int imm5 = (IR & 0x001F);
    if (IR & 0x0020) {
        if(imm5 & 0x0010){          //SEXT
            imm5 |= 0xFFE0;
        }
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] ^ imm5; //DR = SR1 XOR imm5

    } else{ //from source register
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2];

    }

    setcc(DR);


}

/*
PC = BaseR;
 */
void JMP(void){
    int BaseR = (IR & 0x01C0)>>6;
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR];

}

/*
 if (bit[4] == 0)
DR = LSHF(SR, amount4);
else
if (bit[5] == 0)
DR = RSHF(SR, amount4, 0);
else
DR = RSHF(SR, amount4, SR[15]);
setcc();
 */
void SHF(void){
    int DR = (IR & 0x0E00) >> 9;
    int SR = (IR & 0x01C0) >> 6;
    int amount4 = (IR & 0x000F);
    if (!(IR & 0x0010)) {
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR] << amount4;
    }
    else if (!(IR & 0x0020)) {
        NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR] >> amount4;
    }
    else {
        if (CURRENT_LATCHES.REGS[SR] & 0x8000) {
            SR = CURRENT_LATCHES.REGS[SR];
            SR |= 0xFFFF0000;
            NEXT_LATCHES.REGS[DR] = SR >> amount4;
        }
        else {
            NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.REGS[SR] >> amount4;
        }
    }
    NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.REGS[DR]);
    setcc(DR);


}


/*
 DR = PCë + LSHF(SEXT(PCoffset9),1);
 */
void LEA(void){
    int PCoffset9 = IR & 0x01FF;
    int DR = (IR & 0x0E00) >> 9;
    if (PCoffset9 & 0x0100) { //SEXT
        PCoffset9 |= 0xFFFFFE00;
    }
    PCoffset9 = PCoffset9 << 1;
    NEXT_LATCHES.REGS[DR] = NEXT_LATCHES.PC + PCoffset9;

}


/*
R7 = PCë ;
PC = MEM[LSHF(ZEXT(trapvect8), 1)];
 */
void TRAP(void){
    NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
    int trapvect8 = IR & 0x00FF;
    NEXT_LATCHES.PC = MEMORY[trapvect8][0] + MEMORY[trapvect8][1];


}


void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */
    //fetch

     IR = MEMORY[CURRENT_LATCHES.PC >> 1][0] + (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8);
     NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    //decode
    int opCode = IR >> 12;
    switch (opCode) {

        case 0x0 : //branch
            BR();
            break;
        case 0x1 : // ADD
            ADD();
            break;
        case 0x2 : //LDB
            LDB();
            break;

        case 0x3 :  //STB
            STB();
            break;

        case 0x4 :  //JSR(R)
            JSR();
            break;

        case 0x5 : //AND
            AND();
            break;

        case 0x6 :  //LDW
            LDW();
            break;

        case 0x7 : //STW
            STW();
            break;

        case 0x8 : //RTI
            RTI();
            break;

        case 0x9 : //XOR
            XOR();
            break;

        case 0xA : //not used

        case 0xB : //not used

        case 0xC : //JMP
            JMP();
            break;

        case 0xD : //SHF
            SHF();
            break;

        case 0xE : //LEA
            LEA();
            break;

        case 0xF : //TRAP
            TRAP();
            break;

        default:
            break;

    }

}