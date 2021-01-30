// Ryan Black 4571673
// COP 3402 P-Code Machine

#include <stdio.h>

// Definitions of the maximum stack height and maximum code length.
#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100

// Structure to store the opcode, lexicographical level, and m-value of an instruction.
typedef struct instr {
    int op;
    int l;
    int m;
} instr;

// Functions to simulate the fetch and execute cycles.
void fetch();
void execute();
void print_stack();
int base(int level);

// sb, bp, pc, ir, the code[] (array storing each line of the code) and the stack initialized as global variables
// to allow for easy access throughout the execution of the program.
// ar_splits[100] serves as an indicator of an activation record at a given index in the stack. Used
// when printing the stack to the console.
int sp = -1, bp = 0, pc = 0, HALT = 0, stack[MAX_STACK_HEIGHT];
int ar_splits[100];
instr ir;
instr code[MAX_CODE_LENGTH];

int main(void) {

    // Loop to read in the code until the assumed end of input (sys 0 3) is entered.
    for(int i=0; i < MAX_CODE_LENGTH; i++) {
        // Uses a junk character to store input following the first integer.
        // op, l, and m serve as place holders to store information about each command.
        char junk;
        int op,l,m;
        scanf("%d%c %d %d", &op, &junk, &l, &m);
        code[i].op = op; code[i].l = l; code[i].m = m;
        // Loop ends when assumed end of input is reached.
        if(op == 9 && l == 0 && m == 3)
            break;
    }

    // My attempt at formatting:
    printf("                PC   BP    SP   stack\n");
    printf("Initial Values: %d    %d     %d\n", pc, bp, sp);

    // Endless loop to simulate the virtual machine running; exits when halt flag = 0.
    while(HALT == 0) {
        fetch();
        execute();
        print_stack();
    }
}

// Function to simulate a fetch cycle; op, l, and m read into the IR and PC is incremented.
void fetch() {
    ir.op = code[pc].op;
    ir.l = code[pc].l;
    ir.m = code[pc].m;
    pc++;
}

// Function to simulate an execution cycle.
void execute() {
    switch(ir.op) {
        // LIT
        case 1:
            sp++;
            stack[sp] = ir.m;
            printf("%d LIT  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc+1, bp, sp);
            break;
       // RTN
        case 2:
            printf("%d OPR  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc+1, bp, sp - 1);
            switch(ir.m) {
                case 0:
                    stack[bp-1] = stack[sp];
                    sp = bp - 1;
                    bp = stack[sp + 2];
                    pc = stack[sp + 3];
                    break;
                // NEG
                case 1:
                    stack[sp] = -stack[sp];
                    break;
                // ADD
                case 2:
                    sp = sp - 1;
                    stack[sp] = stack[sp] + stack[sp + 1];
                    break;
                case 3:
                // SUB
                    sp = sp - 1;
                    stack[sp] = stack[sp] - stack[sp + 1];
                    break;
                // MUL
                case 4:
                    sp = sp - 1;
                    stack[sp] = stack[sp] * stack[sp + 1];
                    break;
                case 5:
                // DIV
                    sp = sp - 1;
                    stack[sp] = stack[sp] / stack[sp + 1];
                    break;
                // ODD
                case 6:
                    stack[sp] = stack[sp] % 2;
                    break;
                //
                case 7:
                    sp = sp - 1;
                    stack [sp] = stack[sp] % stack[sp+1];
                    break;
                case 8:
                    sp = sp - 1;
                    stack[sp] = stack[sp] == stack[sp+1];
                    break;
                case 9:
                    sp = sp - 1;
                    stack[sp] = stack[sp] != stack[sp+1];
                    break;
                    // LSS
                case 10:
                    sp = sp - 1;
                    stack[sp] = stack[sp] < stack[sp+1];
                    break;
                case 11:
                    sp = sp - 1;
                    stack[sp] = stack[sp] <= stack[sp+1];
                    break;
                case 12:
                    sp = sp - 1;
                    stack[sp] = stack[sp] > stack[sp+1];
                    break;
                case 13:
                    sp = sp - 1;
                    stack[sp] = stack[sp] >= stack[sp+1];
                    break;
            }
            break;
       // LOD
        case 3:
            sp = sp + 1;
            stack[sp] = stack[base(ir.l) + ir.m];
            printf("%d LOD  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
            break;
       // STO
        case 4:
            stack[base(ir.l) + ir.m] = stack[sp];
            sp = sp - 1;
            printf("%d STO  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
            break;
       // CAL
        case 5:
            stack[sp+1] = base(ir.l);
            stack[sp+2] = bp;
            stack[sp+3] = pc;
            stack[sp+4] = stack[sp];
            bp = sp + 1;
            int init_pc = pc - 1;
            pc = ir.m;
            printf("%d CAL  %d  %d     %d    %d      %d", init_pc, ir.l, ir.m, pc, bp, sp);
            break;
       // INC
        case 6:
            ar_splits[sp] = 1;
            sp = sp + ir.m;
            printf("%d INC  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
            break;
       // JMP
        case 7:
            printf("%d JMP  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, ir.m, bp, sp);
            pc = ir.m;
            break;
       // JPC
        case 8:
            if (stack[sp] == 0) {
                printf("this is stack SP %d\n", stack[sp]);
                int init_pc = pc - 1;
                pc = ir.m;
                printf("%d JPC  %d  %d     %d    %d      %d", init_pc, ir.l, ir.m, ir.m, bp, sp);
                break;
            }
            sp = sp - 1;
            printf("%d JPC  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
            break;
       //SYS
        case 9:
            switch(ir.m) {
                case 1:
                    printf("Top of Stack Value: %d\n", stack[sp]);
                    printf("%d SYS  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, ir.m, bp, sp);
                    sp = sp - 1;
                    break;
                case 2:
                    sp = sp + 1;
                    printf("Please enter an integer: \n");
                    scanf("%d", &stack[sp]);
                    printf("%d SYS  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
                    break;
                case 3:
                    printf("%d SYS  %d  %d     %d    %d      %d", pc-1, ir.l, ir.m, pc, bp, sp);
                    HALT = 1;
                    break;
            }
            break;
    }

}

int base(int level) {
    int base = bp;
    int counter = level;
    while (counter > 0) {
        base = stack[base];
        counter--;
    }
    return base;
}

// Function to print all the elements in the stack. WIP FIND THE AR!!!
void print_stack() {
    int top = sp;
    int count = 0;
    if (top == -1) {
        printf("\n");
        return;
    }

    printf("   ");

    while (top != -1) {
        printf("%d ", stack[count]);
        if (count !=0 && ar_splits[count] == 1)
            printf("%c ", '|');
        top--;
        count++;
    }
    printf("\n");
}
