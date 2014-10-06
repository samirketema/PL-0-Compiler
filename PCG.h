/***    Parser/Code Generator - PL/0 Programming Language

        Samir Ketema
        July 6, 2014
        Systems Software - COP 3402
        This is a parser and code generator for the PL/0
        Programming Language. This is to work with the other included
        files provided in the compiler zip folder.

        Input: LAout.txt (lexeme list from Lexical Analyzer)

        Output: code.txt (output PL/0 machine code)
***/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SYMBOL_TABLE_SIZE 10000
#define MAX_CODE_SIZE 500

//Declaring Structs and Variables.

//Tokens for our parser
typedef struct{
  char type[3];   //Type of token (1 - 33).
  char value[12]; //Value of token (identifier, or number value).
} token;

//This is the struct for our symbol table.
typedef struct{
  int kind;
  char name[12];
  int val;
  int level;
  int addr;
} symbol;
//Make the symbol table.
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

//Instruction register format struct.
typedef struct{
  int op; //opcode.
  int r;  //register.
  int l;  //lexicographical level.
  int m;  //modifier.
} code_struct;
//Code array
code_struct codearray[MAX_CODE_SIZE];
//Code index
int cx = 0;

//Here we keep track of the register we are working with.
int register_ptr;

//Stack pointer.
int sp;

//Maximum Lexicographical Level.
int level;

//Current Lexicographical Level.
int curr_lex_level;

//This is the token we are working with as we read in tokens.
token current_token; //Current token to analyze.

//Keeps track of symbols in symbol table.
int symbol_cnt;

//File pointer to token (internal representation) file.
FILE *PCGinput;

//Retrieves next token.
int get_next_token();

//Inserts a symbol to the symbol table.
void add_to_symbol_table( int k, char name[], int val, int addr );

//Prints out error message.
void ERR( int n );

//Generates PL/0 code line with input fields.
void emit(int op, int r, int l, int m);

//procedures defined in the grammar
void program();
void block();
void statement();
void condition();
int rel_op();
void expression();
void term();
void factor();

//This is the main function of the Parser/Code Generator.
void PCG(int flag){

  int i;

   //Open lexeme list
  PCGinput = fopen("LAout.txt", "r");

  //Initialize values
  symbol_cnt = 0;
  register_ptr = 0;
  cx = 0;
  sp = 1;
  level = curr_lex_level = 1;

  //Initialize code array
  for( i = 0; i < MAX_CODE_SIZE; i++ ){
    codearray[i].op = 0;
    codearray[i].r = 0;
    codearray[i].l = 0;
    codearray[i].m = 0;
  }

  //Parse tokens
  program();

  //If it went well, no errors.
  printf( "\n\nProgram is correct syntactically!\n" );

  //Declare output file.
  FILE *fout = fopen( "code.txt", "w" );

  //Print the assembly code out to the file
  int x = 0;
  while( !( codearray[x].op == 0 && codearray[x].r == 0 && codearray[x].l == 0 && codearray[x].m == 0 ) ){
    if(flag)
        printf("%d %d %d %d\n", codearray[x].op, codearray[x].r, codearray[x].l, codearray[x].m );
    fprintf(fout, "%d %d %d %d\n", codearray[x].op, codearray[x].r, codearray[x].l, codearray[x].m );
    x++;
  }

  //Close files.
  fclose(PCGinput);
  fclose(fout);

  return;
}

//Program
void program(){

  get_next_token();

  block();


  //if( strcmp(current_token.type, "19") != 0 ) ERR(9);

  emit(11, 0, 0, 3);

}

void block(){

  char name[12];
  int val;
  int procedure_spot;

  sp = 3;
  int space = 4; //Keep track of space
  int proc_x; //Index of Procedure

  //Jump to procedure code. Need to save address of jump instruction generated.
  int jump_address = cx;

    //emit jmp
   emit( 7, 0, 0, 0 );
  //Constant
  if( !strcmp(current_token.type, "28" /*constsym*/) ){
    do{

      get_next_token();

      //Identsym
      if( strcmp(current_token.type, "2" ) != 0 ) ERR(4);

      strcpy(name, current_token.value);

      get_next_token();

      //eqlsym
      if( strcmp(current_token.type, "9" ) != 0 ) ERR(3);

      get_next_token();
      //numbersym
      if( strcmp(current_token.type, "3" ) != 0 ) ERR(2);

      //Take current value and add symbol to symbol table
      val = atoi(current_token.value);
      add_to_symbol_table( 1 /*constant*/, name, val, 0);

      get_next_token();

      //while no commasym
    } while( !strcmp(current_token.type, "17") );

    //if semicolonsym, error
    if( strcmp(current_token.type, "18" ) != 0 ) ERR(5);

    get_next_token();

  }

  //var-declaration
  if( !strcmp(current_token.type, "29") ){
    do{

      get_next_token();

      if( strcmp(current_token.type, "2") != 0 ) ERR(4);

      strcpy(name, current_token.value);

      //Add the symbol table.
      add_to_symbol_table( 2 /*variable*/, name, 0, sp );
      symbol_table[ symbol_cnt - 1 ].level = level;
	  space++;
      sp++;

      get_next_token();
    //While no commasym
    }while( !strcmp(current_token.type, "17") );

    //if no semicolon, error
    if( strcmp(current_token.type, "18") != 0 ) ERR(5);
    get_next_token();
  }


  //procedure
  while( !strcmp( current_token.type, "30") ){

    get_next_token();

    //identifier must go after procedure
    if( strcmp( current_token.type, "2") != 0 ) ERR( 4 );


    strcpy(name, current_token.value);

    //Create symbol table entry
    add_to_symbol_table( 3, name, 0, 0 );
    proc_x = symbol_cnt - 1; //Procedure index.
    symbol_table[ proc_x ].level = level;
    symbol_table[ proc_x ].addr = jump_address + 1;

    get_next_token();

    //semicolon expected
    if( strcmp( current_token.type, "18") != 0 ) ERR( 17 );

    get_next_token();

    level++;

    block();

    //semicolon expected
    //if( strcmp( current_token.type, "18" ) != 0 ) ERR( 17 );

    get_next_token();

  }








  codearray[ jump_address ].m = cx;
  emit( 6 /*inc*/, 0, 0, space );

  //statement.
  statement();

  curr_lex_level--;

}

void statement(){

  int i; //index.

  //Boolean to check if identifier is declared. If so, = 1; else, = 0.
  int declared = 0;

  //Save symbol table index for found identifier.
  int ident_index;

    //ident
  if( !strcmp(current_token.type, "2") ){

    //Check if it's in the symbol table.
    for( i = symbol_cnt - 1; i >= 0; i-- )
        if( !strcmp( current_token.value, symbol_table[i].name ) ){

            if( symbol_table[i].kind == 1 ) ERR(12);

            else if( symbol_table[i].kind == 2  ){
                declared = 1;
                ident_index = i;
            }

        }

    //Undeclared identifier.
    if( !declared ) ERR(11);

    get_next_token();

    if( strcmp(current_token.type, "20") != 0 ) ERR(3);

    get_next_token();

    expression();

        //store at necessary mem address
     emit( 4 /*sto*/, register_ptr - 1, curr_lex_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1  );


     register_ptr--;

  }

  //"call" ident
    else if( !strcmp(current_token.type, "27" /*callsym*/) ){

        int declared = 0;

        get_next_token();

        //identifier expected.
        if( strcmp(current_token.type, "2" /*identsym*/) ) ERR(14);

        //Check if identifier has been declared.
        for( i = symbol_cnt - 1; i >= 0; i-- )
            if( !strcmp( current_token.value, symbol_table[i].name ) ){ //Found!
                ident_index = i; //Save identifier index.
                declared = 1;
            }

        if( !declared ) { ERR(11 ); }

        if( symbol_table[ident_index].kind== 3 /*proc*/ ){
            emit( 5 /*cal*/, 0, level, symbol_table[ ident_index ].addr );
            curr_lex_level ++;
        }
        else
            ERR( 14 ); //Call must be followed by a procedure identifier.

        get_next_token();

    }


   //begin
  else if( !strcmp(current_token.type, "21") ){

    get_next_token();

    statement();

    while( !strcmp(current_token.type, "18") ){

      get_next_token();

      statement();

    }

    get_next_token();

  }

  // if (cond) -> stmt
  else if( !strcmp(current_token.type, "23") ){

    get_next_token();

    condition();

    if( strcmp(current_token.type, "24" ) != 0 ) ERR(16);

    get_next_token();

    //start code gen.

    int ctemp = cx; //Save current code index.


     emit( 8, register_ptr - 1, 0, 0 );


    statement();

    get_next_token();

    //else
    if( !strcmp( current_token.type, "33") ){
        //current cx
        int ctemp2 = cx;

        emit( 7 , 0, 0, 0 );

        codearray[ ctemp ].m = cx;
        get_next_token();

        statement();

        codearray[ ctemp2 ].m = cx;
        register_ptr--;

    }

    else{
        codearray[ ctemp ].m = cx;

        register_ptr--;
    }
  }

  // while(cond) do (stmt)
  else if( !strcmp(current_token.type, "25") ){

    int cx1 = cx;

    get_next_token();

    condition();

    int cx2 = cx;

    emit( 8 , register_ptr - 1, 0, 0 );

    if( strcmp(current_token.type, "26" ) != 0 ) ERR(18);

    get_next_token();

    statement();

    emit( 7 , 0, 0, cx1 );

    codearray[ cx2 ].m = cx;

    register_ptr--;

  }

  //read ident
  else if( !strcmp( current_token.type, "32" ) ){

    get_next_token();

    if( strcmp( current_token.type, "2") != 0 ) ERR(29);

    //Check if identifier is in symbol table.
    for( i = symbol_cnt - 1; i >= 0; i-- )
        if( !strcmp( current_token.value, symbol_table[i].name ) ){

            declared = 1;
            ident_index = i;

        }

    //Undeclared ident.
    if( !declared ) ERR(11);

    //take user input for register.
     emit( 10 /*sio*/, register_ptr, 0, 2 /*read*/ );

     //Read into var.
     if( symbol_table[ ident_index ].kind == 2 /*var*/ )
        emit( 4 /*sto*/, register_ptr, curr_lex_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1 ); //Register to memory.

     //error if read into const
     else if( symbol_table[ ident_index ].kind == 1 )
        ERR( 12 );

     get_next_token();

  }

  // write ident
  else if( !strcmp( current_token.type, "31") ){

    get_next_token();

    if( strcmp( current_token.type, "2") != 0 ) ERR(29);

    //Check if ident is in symbol table.
    for( i = symbol_cnt - 1; i >= 0; i-- )
        if( !strcmp( current_token.value, symbol_table[i].name ) ){

            if( symbol_table[i].kind == 1 || symbol_table[i].kind == 2  ){
                declared = 1;
                ident_index = i;
            }

        }

    //Undeclared ident.
    if( !declared ) ERR(11);



     //Get var from main mem.
     if( symbol_table[ ident_index ].kind == 2){
        emit( 3 , register_ptr, curr_lex_level - symbol_table[ ident_index ].level, symbol_table[ ident_index ].addr - 1 ); //Memory to register.
        emit( 9 , register_ptr, 0, 1 ); //Register to screen.
     }

     //Get const from symbol table.
     else if( symbol_table[ ident_index ].kind == 1){
        emit( 1 , register_ptr, 0, symbol_table[ ident_index ].val );
        emit( 9 , register_ptr, 0, 1 );
     }

     get_next_token();

  }

}

//condition
void condition(){
  if( !strcmp(current_token.type, "8") ){

    get_next_token();

    expression();

    emit( 17 /*odd*/, register_ptr - 1, register_ptr - 1, 0 );

  }

  //rel-op
  else{

    expression();

    int op = rel_op();

    if( !op ) ERR(20);

    get_next_token();

    expression();

    emit( op, register_ptr - 2, register_ptr - 2, register_ptr - 1 );
    register_ptr--;

  }

}

int rel_op(){

    //eql
  if( !strcmp( current_token.type, "9" ))
    return 19;
    //neq
  else if( !strcmp( current_token.type, "10" ) )
    return 20;
    //les
  else if( !strcmp( current_token.type, "11"  ) )
    return 21;
    //leq
  else if( !strcmp( current_token.type, "12"  ) )
    return 22;
    //gtr
  else if( !strcmp( current_token.type, "13"  ) )
    return 23;
    //geq
  else if( !strcmp( current_token.type, "14" ) )
    return 24;

  else
    return 0;

}

void expression(){

    char add_op[3];


  if( !strcmp(current_token.type, "4" ) || !strcmp(current_token.type, "5" ) ){


    strcpy( add_op, current_token.type );

    get_next_token();

    term();


    if( !strcmp( add_op, "5"  ) )
        emit( 12 /*neg*/, register_ptr - 1, register_ptr - 1, 0 );

  }

  else term();

  while( !strcmp(current_token.type, "4" ) || !strcmp(current_token.type, "5" ) ){


    strcpy( add_op, current_token.type );

    get_next_token();

    term();


    if( !strcmp( add_op, "4"  ) ){
        emit( 13 , register_ptr - 2, register_ptr - 2, register_ptr - 1 );
        register_ptr--;
    }

    else{
        emit( 14 /*sub*/, register_ptr - 2, register_ptr - 2, register_ptr - 1 );
        register_ptr--;
    }

  }

}

void term(){


   char mulop[3];


  factor();

  while( !strcmp(current_token.type, "6" ) || !strcmp(current_token.type, "7" ) ){


    strcpy( mulop, current_token.type );

    get_next_token();

    factor();

    if( !strcmp( mulop, "6"  ) ){


        emit( 15, register_ptr - 2, register_ptr - 2, register_ptr - 1 );
        register_ptr--;

    }


    else{


        emit( 16 , register_ptr - 2, register_ptr - 2, register_ptr - 1 );
        register_ptr--;

    }

  }

}

void factor(){

  int i;


  int declared = 0;


  int ident_index = 0;


  if( !strcmp(current_token.type, "2" ) ){

    //Check in sym table.
    for(  i = symbol_cnt - 1; i >= 0; i-- )
      if( !strcmp( current_token.value, symbol_table[i].name ) ){
        declared = 1; //Declared!
        ident_index = i; //Save index.
      }

    //Undeclared identifier
    if( !declared ) ERR(11);



    if( symbol_table[ident_index].kind == 2 /*var*/ )
        emit( 3 , register_ptr, curr_lex_level - symbol_table[ ident_index ].level, symbol_table[ident_index].addr - 1 );


    else if( symbol_table[ident_index].kind == 1 /*const*/ )
        emit( 1 , register_ptr, 0, symbol_table[ident_index].val );

    register_ptr++;

    get_next_token();

  }


  else if( !strcmp(current_token.type, "3") ){

    emit( 1 /*lit*/, register_ptr, 0, atoi(current_token.value) );
    register_ptr++;

    get_next_token();

  }


  else if( !strcmp(current_token.type, "15" ) ){

    get_next_token();

    expression();

    if( strcmp(current_token.type, "16" ) != 0 ) ERR(22);

    get_next_token();

  }

  else
    ERR(23);

}

int get_next_token(){


  if( fscanf( PCGinput, "%s", current_token.type ) != EOF ){


    if( !strcmp(current_token.type, "2") || !strcmp(current_token.type, "3") )
      fscanf( PCGinput, "%s", current_token.value );

    else current_token.value[0] = '\0';


    return 1;

  }

  //No more tokens.
  else{
    current_token.type[0] = '\0';
    current_token.value[0] = '\0';
    return 0;
  }

}

void ERR( int n ){
  printf("Error: ");
  switch( n ){

  case 1:
    printf("Use = instead of :=\n");
    break;
  case 2:
    printf("= must be followed by a number\n");
    break;
  case 3:
    printf("Identifier must be followed by =\n");
    break;
  case 4:
    printf("Const, int, procedure must be followed by identifier\n");
    break;
  case 5:
    printf("Semicolon or comma missing\n");
    break;
  case 6:
    printf("Incorrect symbol after procedure declaration\n");
    break;
  case 7:
    printf("Statement expected\n");
    break;
  case 8:
    printf("Incorrect symbol after statement part in block\n");
    break;
  case 9:
    printf("Period expected\n");
    break;
  case 10:
    printf("Semicolon between statements missing\n");
    break;
  case 11:
    printf("Undeclared identifier\n");
    break;
  case 12:
    printf("Assignment to constant or procedure is not allowed\n");
    break;
  case 13:
    printf("Assignment operator expected\n");
    break;
  case 14:
    printf("Call must be followed by an identifier\n");
    break;
  case 15:
    printf("Call of a constant or variable is meaningless\n");
    break;
  case 16:
    printf("Then expected\n");
    break;
  case 17:
    printf("Semicolon or } expected\n");
    break;
  case 18:
    printf("Do expected\n");
    break;
  case 19:
    printf("Incorrect symbol following statement\n");
    break;
  case 20:
    printf("Relational operator expected\n");
    break;
  case 21:
    printf("Expression must not contain a procedure identifier\n");
    break;
  case 22:
    printf("Right parenthesis missing\n");
    break;
  case 23:
    printf("The preceding factor cannot begin with this symbol\n");
    break;
  case 24:
    printf("Expression cannot begin with this symbol\n");
    break;
  case 25:
    printf("Number is too large\n");
    break;
  case 26:
    printf("End expected\n");
    break;
  case 27:
    printf("All available registers in use\n");
    break;
  case 28:
    printf("Variable not initialized\n");
    break;
  case 29:
    printf("Identifier expected after read or write\n");
    break;

  }

  //close files
  fclose(PCGinput);

  //end program
  exit(0);
}

void emit(int op, int r, int l, int m){

    codearray[cx].op = op;
    codearray[cx].r = r;
    codearray[cx].l = l;
    codearray[cx].m = m;
    cx++;



}

void add_to_symbol_table( int k, char name[], int val, int addr ){

  //add to symbol table.
  symbol_table[symbol_cnt].kind = k;
  strcpy( symbol_table[symbol_cnt].name, name );
  symbol_table[symbol_cnt].val = val;
  symbol_table[symbol_cnt].addr = addr;

  symbol_cnt++;

}
