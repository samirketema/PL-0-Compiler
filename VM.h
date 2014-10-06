/***    PM/0 Virtual Machine Implementation

        Samir Ketema
        June 2nd, 2014
        Systems Software - COP 3402

        The following program is an virtual machine implementation of the PM/0 stack P-Machine.
        Given a specific 24 instruction ISA by the professor, I implemented a simple
        fetch-execute cycle that works down to every register and memory location.
***/
//Include necessary header files.
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>

//Declare maximum values for inputs/code.
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEX_LEVELS 3


//Declaring a struct that will hold the separate fields for a line of code
//utilizing format: OP R L M.
//OP - Operation Code, R - Register, L - Lexicographical Level
//M - Operation Data (depends on specific Operation)
struct instruction{
    int OP, R, L, M;};

//Declare + Initialize the Program Counter, Instruction Register,
//Base Pointer, and Stack Pointer to their initial values.
int pc, sp = 0;
int bp = 1;

//Initialize the stack.
int stack[MAX_STACK_HEIGHT] = {0};

//Declare CPU Registers
int r[16] = {0};

//Declare Halt flag. Initialize to zero (do not halt).
int halt = 0;

//Declare input and output files
FILE *ifp;
FILE *ofp;

void print(int cnt);
void fetch();
void execute();
int base(int L, int base);
void printStack( int flag);

//Initialize our array of code. This will hold all the code we execute for the program.
struct instruction code[MAX_CODE_LENGTH];

//Declare the instruction register.
struct instruction ir;

    //Main function
    int VM(int flag){


        //Open input and output files
        ifp =   fopen("code.txt", "r");
        ofp =   fopen("output.txt", "w");

        //If the file isn't there or could not open properly, end the program.
        if(ifp == NULL){
            printf("File could not open. Program halted.\n");
            return;
        }

        //Keep track of number of instructions the input file.
        int cnt = 0;

        //Read in the lines from the input file.
        while(!feof(ifp))
        {   fscanf(ifp, "%d", &code[cnt].OP);
            fscanf(ifp, "%d", &code[cnt].R);
            fscanf(ifp, "%d", &code[cnt].L);
            fscanf(ifp, "%d", &code[cnt].M);
            cnt++;

        }

    //Call our initial printing function named "print". (Shoutout to my homeboys C# and Java)
    print(cnt);

    cnt = 0;

    //Print the column headers for the stack tracing
    if(flag){
    printf("\n\nExecution of Program:\n");
    printf("\nInitial Values\t\t\t\tpc\tbp\tsp\n");
    }

    fprintf(ofp, "\n\nExecution of Program:\n");
    fprintf(ofp, "\nInitial Values\t\t\t\tpc\tbp\tsp\n");
    //Fetch - Execute Cycle. Runs until the halt flag is set to stop the loop.
    while(halt == 0){

        //Start the fetch part of the fetch-execute cycle.
        fetch();

        //Start the execution process of the
        execute();

        if(flag){
        printf("%d\t%d\t%d\t", ir.R, ir.L, ir.M);
        printf("%d\t%d\t%d\t", pc, bp, sp);
        }
        fprintf(ofp, "%d\t%d\t%d\t", ir.R, ir.L, ir.M);
        fprintf(ofp, "%d\t%d\t%d\t", pc, bp, sp);
        printStack(flag);

        if(( (pc==0) && (bp==0) && (sp==0) ))
            halt = 1;



    }

    //Close files.
    fclose(ifp);
    fclose(ofp);

    //End main function.
    return;
    }

    //Pre: Print takes in the number of instructions in the code array.
    //Post: The OP codes (operation code) of all instructions will be
    //  interpreted and the input code as a whole will be printed out to the
    //  output file designated for the entire program.
    void print(int cnt){

        int i;

        fprintf(ofp, "Interpreted Assembly Language\n");
        fprintf(ofp, "\nLine\tOP\tR\tL\tM");

        for(i=0; i<cnt; i++){

            int op = code[i].OP;

            //Interpret the operation.
            switch(op){
            case 1: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lit\t"); break;
            case 2: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "rtn\t"); break;
            case 3: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lod\t"); break;
            case 4: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sto\t"); break;
            case 5: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "cal\t"); break;
            case 6: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "inc\t"); break;
            case 7: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "jmp\t"); break;
            case 8: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "jpc\t"); break;
            case 9: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sio\t"); break;
            case 10:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sio\t"); break;
            case 11:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sio\t"); break;
            case 12:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "neg\t"); break;
            case 13:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "add\t"); break;
            case 14:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sub\t"); break;
            case 15:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "mul\t"); break;
            case 16:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "div\t"); break;
            case 17:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "odd\t"); break;
            case 18:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "mod\t"); break;
            case 19:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "eql\t"); break;
            case 20:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "neq\t"); break;
            case 21:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lss\t"); break;
            case 22:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "leq\t"); break;
            case 23:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "gtr\t"); break;
            case 24:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "geq\t"); break;
            default: return;
            }

            //Print the three other fields: R, L, M.
            fprintf(ofp, "%d\t", code[i].R);
            fprintf(ofp, "%d\t", code[i].L);
            fprintf(ofp, "%d\t", code[i].M);
        }
        return;
    }

    //Fetch retrieves the instruction to be executed, then
    void fetch(){

        //Fetch current instruction
        ir = code[pc];

        //Print the program counter before it's incremented.
        fprintf(ofp, "%d\t", pc);

        //Increment the Program Counter before we start execution.
        //Program Counter points to the NEXT instruction.
        pc++;

        //Return to main function.
        return;

    }

    void execute(){

        switch(ir.OP){
        //LIT
        case 1:
            fprintf(ofp, "lit\t");
            r[ir.R] = ir.M;
            break;

        //RTN
        case 2:
            fprintf(ofp, "rtn\t");
            sp = bp -1;
            bp = stack[sp+3];
            pc = stack[sp+4];
            break;

        //LOD
        case 3:
            fprintf(ofp, "lod\t");
            r[ir.R] = stack[base(ir.L, bp) + ir.M];
            break;

        //STO
        case 4:
            fprintf(ofp, "sto\t");
            stack[ base(ir.L, bp) + ir.M] = r[ir.R];
            break;

        //CAL
        case 5:
            fprintf(ofp, "cal\t");
            stack[sp+1] = 0;
            stack[sp+2] = base(ir.L, bp);
            stack[sp+3] = bp;
            stack[sp+4] = pc;
            bp = sp+1;
            pc = ir.M;
            break;

        //INC
        case 6:
            fprintf(ofp, "inc\t");
            sp = sp + ir.M;
            break;

        //JMP
        case 7:
            fprintf(ofp, "jmp\t");
            pc = ir.M;
            break;

        //JPC
        case 8:
            fprintf(ofp, "jpc\t");
            if(r[ir.R]==0)
                pc = ir.M;
            break;

        //SIO - write register to screen
        case 9:
            fprintf(ofp, "sio\t");
            printf("\nR[%d] = %d\n", ir.R, r[ir.R]);
            break;

        //SIO - read in value from screen to register.
        case 10:
            fprintf(ofp, "sio\t");
            printf("\nPlease type in the number you wish to store in the register. Press enter when finished.\n");
            scanf("%d", &r[ir.R]);
            break;

        //SIO - set halt flag to 1.
        case 11:
            fprintf(ofp, "sio\t");
            halt = 1;
            break;

        //NEG
        case 12:
            fprintf(ofp, "neg\t");
            r[ir.R] = 0 - r[ir.L];
            break;

        //ADD
        case 13:
            fprintf(ofp, "add\t");
            r[ir.R] = r[ir.L] + r[ir.M];
            break;

        //SUB
        case 14:
            fprintf(ofp, "sub\t");
            r[ir.R] = r[ir.L] - r[ir.M];
            break;

        //MUL
        case 15:
            fprintf(ofp, "mul\t");
            r[ir.R] = r[ir.L] * r[ir.M];
            break;

        //DIV
        case 16:
            fprintf(ofp, "div\t");
            r[ir.R] = ((r[ir.L])/(r[ir.M]));
            break;

        //ODD
        case 17:
            fprintf(ofp, "odd\t");
            r[ir.R] = r[ir.R]%2;
            break;

        //MOD
        case 18:
            fprintf(ofp, "mod\t");
            r[ir.R] = (r[ir.L])%(r[ir.M]);
            break;

        //EQL
        case 19:
            fprintf(ofp, "eql\t");
            if(r[ir.L]==r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;

        //NEQ
        case 20:
            fprintf(ofp, "neq\t");
            if(r[ir.L]!=r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;

        //LSS
        case 21:
            fprintf(ofp, "lss\t");
            if(r[ir.L]<r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;

        //LEQ
        case 22:
            fprintf(ofp, "leq\t");
            if(r[ir.L]<=r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;

        //GTR
        case 23:
            fprintf(ofp, "gtr\t");
            if(r[ir.L]>r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;

        //GEQ
        case 24:
            fprintf(ofp, "geq\t");
            if(r[ir.L]>=r[ir.M])
                r[ir.R]=1;
            else
                r[ir.R]=0;
            break;


        default:
            fprintf(ofp, "err\t");
            return;

        }
    return;

    }

    int base(int L, int base){

        int b1; //Find base L levels down

        b1 = base;

        while(L>0){
            b1 = stack[b1 + 1];
            L--;
        }

        return b1;
}


    void printStack(int flag){
        if(bp==0){
            return;
        }
        else{
            int i;
            for(i=1; i<=sp; i++){
                if(flag)
                    printf("%d\t", stack[i]);
                fprintf(ofp, "%d\t", stack[i]);
            }
            if(flag)
              printf("\n");
            fprintf(ofp, "\n");
            return;

        }
    }
