/***    Lexical Analyzer - PL/0 Programming Language

        Samir Ketema
        June 15, 2014
        Systems Software - COP 3402
        This is a Lexical Analyzer for the programming language PL/0.
        It reads in a source program written in PL/0, identifies errors,
        and produces as output: the source program, the source program's
        lexeme table and a list of lexemes.
***/

//Include libraries.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//Declare program constraint constants.
#define IDENTIFIER_MAX_LENGTH   11
#define NUM_MAX_LENGTH          5
#define SYMBOL_TABLE_SIZE   500


    //Data structure for the symbol.
    typedef struct{
        int kind;       //const = 1, var = 2, proc = 3
        char name[11];  //name up to 11 chars
        int val;        //number (ASCII value)
        int level;      //L Level
        int addr;       //M address

    }   symbols;

    //Create a symbol table.
    symbols ST[SYMBOL_TABLE_SIZE];

    typedef struct{
        char name[11]; //keep track of symbols before we make a symbol table.
    } name_list;

    typedef enum {
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,
    eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
    lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19,
    becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26,
    callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33, colonsym = 34
    } token_type;



    //Initialize input file character array and keep track of it's length.
    char in[20000];
    int input_length = 0;

    //Declare input and output files
    FILE *ifp = NULL;
    FILE *ofp;

    //Declare necessary arrays for tokens, numbers, and identifiers
    int tokens[20000];
    int token_cnt =  0;

    int num[20000];
    int num_cnt = 0;
    int num_print= 0;

    char id_list[20000][IDENTIFIER_MAX_LENGTH];
    int  id_ints[20000];
    int  id_cnt = 0;
    int  id_total_number = 1;

//Declare functions for converting strings to tokens and tokens to strings.
int characterToToken(char sym);
char* tokenToCharacter(int token);

    //Main Function
    void LA(int print){

        //Read in filename from command line argument.
        //char* fname = argv[1];

        //Open input and output files
        ifp =   fopen("input.txt", "r");
        ofp =   fopen("LAout.txt", "w");

        //If the file isn't there or could not open properly, end the program.
        if(ifp == NULL){
            printf("File could not open. Program halted.\n");
            return;
        }

        //Read in the input file character by character.
        char temp;
        int i = 0;
        while(fscanf(ifp,"%c", &temp) != EOF) {
            in[i]= temp;
            i++;
            input_length++;
    }
        //Close input file.
        fclose(ifp);


  /*      //Output the Original Source Program Code
        fprintf(ofp, "Source Program:\n");
        for (i=0; i<input_length; i++)
            fprintf(ofp, "%c", in[i]);
*/

    //Main Lexical Analysis Loop
    for (i=0; i< input_length; i++)
    {
        //Ignoring Comments
        if (in[i] == '/' && in[i+1] == '*')
        {
            //Look ahead from '/'
            int j=i+2;

            //Keep going until we hit the other asterisk and '/'
            while (!(in[j] == '*' && in[j+1] == '/'))
            {
                j++;
            }

            //Continue from after the comment.
            i=j+2;
            continue;
        }

        //Ignore new lines
        if (in[i] == '\n')
            continue;

        //Ignoring whitespace
        if (in[i] == ' ')
            continue;

        //Ignore tabs
        if (in[i] == '\t')
            continue;

        //Investigate possible reserved words
        if (in[i] >= 'a' && in[i] <= 'z')
        {
            switch ((int) in[i])
            {
                //Check for 'begin'
                case (int) 'b':
                    if ((in[i+1] == 'e') &&
                        (in[i+2] == 'g') &&
                        (in[i+3] == 'i') &&
                        (in[i+4] == 'n'))
                    {
                        i+=4;
                        tokens[token_cnt] = beginsym;
                        token_cnt++;
                        continue;
                    }

                case (int) 'e':
                    //Check for 'end'
                    if ((in[i+1] == 'n') &&
                        (in[i+2] == 'd'))
                    {
                        i+=2;
                        tokens[token_cnt] = endsym;
                        token_cnt++;
                        continue;
                    }
                    //Check for 'else'
                    else if ((in[i+1] == 'l') &&
                             (in[i+2] == 's') &&
                             (in[i+3] == 'e'))
                    {
                        i+=3;
                        tokens[token_cnt] = elsesym;
                        token_cnt++;
                        continue;
                    }

                //Check for 'if'
                case (int) 'i':
                    if (in[i+1] == 'f')
                    {
                        i+=1;
                        tokens[token_cnt] = ifsym;
                        token_cnt++;
                        continue;
                    }


                //Check for 'then'
                case (int) 't':
                    if ((in[i+1] == 'h') &&
                        (in[i+2] == 'e') &&
                        (in[i+3] == 'n'))
                    {
                        i+=3;
                        tokens[token_cnt] = thensym;
                        token_cnt++;
                        continue;
                    }

                case (int) 'w':
                    //Check for 'while'
                    if ((in[i+1] == 'h') &&
                        (in[i+2] == 'i') &&
                        (in[i+3] == 'l') &&
                        (in[i+4] == 'e'))
                    {
                        i+=4;
                        tokens[token_cnt] = whilesym;
                        token_cnt++;
                        continue;
                    }
                    //Check for 'write'
                    else if ((in[i+1] == 'r') &&
                             (in[i+2] == 'i') &&
                             (in[i+3] == 't') &&
                             (in[i+4] == 'e'))
                    {
                        i+=4;
                        tokens[token_cnt] = writesym;
                        token_cnt++;
                        continue;
                    }

                //Check for 'do'
                case (int) 'd':
                    if (in[i+1] == 'o')
                    {
                        i+=1;
                        tokens[token_cnt] = dosym;
                        token_cnt++;
                        continue;
                    }
                 //Check for 'call'
                case (int) 'c':
                    if ((in[i+1] == 'a') &&
                        (in[i+2] == 'l') &&
                        (in[i+3] == 'l'))
                    {
                        i+=3;
                        tokens[token_cnt] = callsym;
                        token_cnt++;
                        continue;
                    }
                    //check for 'const'
                    else if ((in[i+1] == 'o') &&
                             (in[i+2] == 'n') &&
                             (in[i+3] == 's') &&
                             (in[i+4] == 't'))
                    {
                        i+=4;
                        tokens[token_cnt] = constsym;
                        token_cnt++;
                        continue;
                    }


                //Check for 'odd'
                case (int) 'o':
                    if ((in[i+1] == 'd') &&
                        (in[i+2] == 'd'))
                    {
                        i+=2;
                        tokens[token_cnt] = oddsym;
                        token_cnt++;
                        continue;
                    }

                //Check for 'procedure
                case (int) 'p':
                    if ((in[i+1] == 'r') &&
                        (in[i+2] == 'o') &&
                        (in[i+3] == 'c') &&
                        (in[i+4] == 'e') &&
                        (in[i+5] == 'd') &&
                        (in[i+6] == 'u') &&
                        (in[i+7] == 'r') &&
                        (in[i+8] == 'e'))
                    {
                        i+=8;
                        tokens[token_cnt] = procsym;
                        token_cnt++;
                        continue;
                    }
                //Check for 'var'
                case (int) 'v':
                    if((in[i+1] == 'a') &&
                       (in[i+2] == 'r'))
					{
						i+=2;
						tokens[token_cnt] = varsym;
						token_cnt++;
						continue;
					}
                //Check for 'read'
                case (int) 'r':
                    if ((in[i+1] == 'e') &&
                        (in[i+2] == 'a') &&
                        (in[i+3] == 'd'))
                    {
                        i+=3;
                        tokens[token_cnt] = readsym;
                        token_cnt++;
                        continue;
                    }

                default:
                    break;
            }
        }

        //Takes a character and returns a token into temp.
        temp= characterToToken(in[i]);
        if (temp != 0)
        {
			int secondSymbol;
			//Check for != and <=
			if(temp == lessym){
				secondSymbol = characterToToken(in[i+1]);
				if(secondSymbol == gtrsym){
					tokens[token_cnt] = neqsym;
					token_cnt++;
					i++;
					continue;
				}
				else if(secondSymbol == eqsym){
					tokens[token_cnt] = leqsym;
					token_cnt++;
					i++;
					continue;
				}
			}

			//Check for >=
			if(temp == gtrsym){
				secondSymbol = characterToToken(in[i+1]);
				if(secondSymbol == eqsym){
					tokens[token_cnt] = geqsym;
					token_cnt++;
					i++;
					continue;
				}
			}

            //Check for :=
			if(temp == colonsym){
				secondSymbol = characterToToken(in[i+1]);
				if(secondSymbol == eqsym){
					tokens[token_cnt] = becomessym;
					token_cnt++;
					i++;
					continue;
				}
                //Else, there is an invalid symbol.
				else{
                    fprintf(ofp, "\n\nError: Invalid Symbol Provided.");
                    exit(EXIT_FAILURE);
				}
			}

			secondSymbol = characterToToken(in[i+1]);

            //Save the token we just got.
            tokens[token_cnt] = temp;
            token_cnt++;

            continue;
        }

        //Reads in identifiers to id_list
        if ((in[i] >= 'A' && in[i] <= 'Z') ||
            (in[i] >= 'a' && in[i] <= 'z'))
        {
            int flag= 1;
			int id_length;
            char id[IDENTIFIER_MAX_LENGTH];
            int j, k, l;
            //sanitize string
            for (k=0; k< IDENTIFIER_MAX_LENGTH; k++)
                id[k]= '\0';

            id_length=0;

            j=i;

            while((in[j] >= 'A' && in[j] <= 'Z') ||
                  (in[j] >= 'a' && in[j] <= 'z') ||
                  (in[j] >= '0' && in[j] <= '9'))
            {
                //identifier is too large, error out
                if (id_length > 11)
                {
                    fprintf(ofp, "\n\nError: identifier too long.");
                    exit(EXIT_FAILURE);
                }

                //otherwise store the char inside a new string for an identifier
                id[id_length] = in[j];
                id_length++;
                j++;
            }

            //store that we have an identifier
            tokens[token_cnt]= identsym;
            token_cnt++;


            for (l=0; l<id_cnt; l++)
            {
                if (strcmp(id, id_list[l]) == 0)
                {
                    id_ints[id_cnt]= l+1;
                    flag= 0;
                    break;
                }
            }
            if (flag)
            {
                id_ints[id_cnt]= id_total_number;
                id_total_number++;
            }
            strcpy(id_list[id_cnt],id);

            id_cnt++;
            i= j-1;
            continue;


        }


        //Takes in numbers
        if (in[i] >= '0' && in[i] <= '9')
        {
			int digits;
			int number;
            int j= i;
			int k;
            //Finds length of the current number
            for (j= i; j< NUM_MAX_LENGTH+i +1 ; j++)
            {

                //If there is whitespace following, that's the end of the number.
                if ((in[j] == ' ') || (in[j] == '\n') ||
                    (in[j] == '\t'))
                    break;

                //If there is a letter, it is now an invalid variable/number.
                if ((in[j] >= 'A' && in[j] <= 'Z') ||
                    (in[j] >= 'a' && in[j] <= 'z'))
                {
                    fprintf(ofp, "\n\nError:  Variable starting with number or Invalid number!");
                    exit(EXIT_FAILURE);
                }
            }

             //Set j to where we were from i.
            j=i;

            //We know we have one digit, so count on to see if the number of digits is too large.
            digits=1;
            number=0;
            while (in[j] >= '0' && in[j] <= '9')
            {
                if (digits > 5)
                {
                    fprintf(ofp, "\n\nError: Number too long.");
                    exit(EXIT_FAILURE);
                }
                digits++;
                j++;
            }

            //Get digits back to 1s place
            digits--;


            for (k= j-digits; k<j; k++)
            {
                number += (in[k]- '0') * pow(10., digits-1);
                digits--;
            }

            //Save that number to our list
            tokens[token_cnt]= numbersym;
            token_cnt++;
            num[num_cnt]= number;
            num_cnt++;

            i= j-1;
            continue;
        }

    }



	num_print= 0;
	id_cnt= 0;


    //fprintf(ofp, "\n\nLexeme Table\nlexeme\ttoken type\n");
    if(print)
        printf("\n\nLexeme Table\nlexeme\ttoken type\n");
    char* lexeme = (char*)malloc(100);
    //print lex table
/*	for (i=0; i< token_cnt; i++)
	{
		if (tokens[i]== numbersym)
		{
			fprintf(ofp, "%d\t%d\n",num[num_print], tokens[i]);
			num_print++;
		}
		else if (tokens[i]== identsym)
		{
			fprintf(ofp, "%s\t%d\n", id_list[id_cnt], tokens[i]);
            id_cnt++;
		}

		else
        {   strcpy(lexeme, tokenToCharacter(tokens[i]));
			fprintf(ofp, "%s\t%d\n", lexeme, tokens[i]);
        }
	}
*/
    id_cnt= 0;
    num_print= 0;
    int cnt = 0;


     int total_numbers = num_cnt;

    int j, symflag = 0;
    char* temp_id = (char*)malloc(100);
  //  fprintf(ofp, "\n\nSymbol Table\nindex\tsymbol\n");
    //Create symbol Table
    for (i=0; i< token_cnt; i++)
	{   symflag =0;
		if (tokens[i]== identsym){
            strcpy(temp_id, id_list[id_cnt]);
            for(j=0; j<id_cnt; j++){
                if(strcmp(ST[j].name, temp_id)==0)
                    symflag=1;
            }
            if(symflag==0){
                strcpy(ST[cnt].name, temp_id);
                ST[cnt].kind = 2;
                //fprintf(ofp, "%d\t%s\n", cnt, id_list[id_cnt]);
                id_cnt++;
                cnt++;
            }
		}

        else if(((tokens[i]== numbersym) ||
                (tokens[i]== constsym)) /*&&
                num_print<total_numbers*/){
            ST[cnt].val = num[num_print];
            ST[cnt].kind = 1;
            //fprintf(ofp, "%d\t%d\n", cnt, num[num_print]);
            cnt++;
            num_print++;
        }

	}



    id_cnt= 0;
    num_print= 0;
    cnt = 0;
    int sym_cnt = 0;
    //name_list names[SYMBOL_TABLE_SIZE];
    //Print the Lexeme List.
    //fprintf(ofp, "\n\nLexeme List:\n");
    if(print)
        printf("\n\nLexeme List:\n");
    for (i=0; i< token_cnt; i++)
	{   symflag = 0;
		if (tokens[i]== numbersym /*&& num_print<total_numbers*/)
		{
		    for(j=0; j<cnt; j++){
                if(ST[j].val==tokens[i+1]){
                    fprintf(ofp, "%d %d ", tokens[i], j);
                    if(print)
                        printf("%d %d ", tokens[i], j);
                    cnt++;
                    num_print++;
                    symflag = 1;
                    break;
                }

		    }
		    if(symflag==0){
                fprintf(ofp, "%d %d ", tokens[i], sym_cnt);
                if(print)
                    printf("%d %d ", tokens[i], sym_cnt);
                num_print++;
                sym_cnt++;
                cnt++;
		    }
		}
		else if (tokens[i]== identsym)
		{
		    strcpy(temp_id, id_list[id_cnt]);
		    //Check if we already printed this out.
		    for(j=0; j<cnt; j++){
                if(strcmp(ST[j].name, temp_id)==0){
                    fprintf(ofp, "%d %s ", tokens[i], temp_id);
                    if(print)
                        printf("%d %s ", tokens[i], temp_id);
                    cnt++;
                    id_cnt++;
                    symflag=1;
                    break;
                }
            }

		    if(symflag==0){
                fprintf(ofp, "%d %s ", tokens[i], temp_id);
                if(print)
                    printf("%d %s ", tokens[i], temp_id);
                cnt++;
                id_cnt++;
                sym_cnt++;
		    }
		}

		else
        {
			fprintf(ofp, "%d ", tokens[i]);

			if(print)
              printf("%d ", tokens[i]);

        }
	}

    fclose(ofp);

    return;
}



//This function converts a character to a token represented by an integer. Otherwise, it will return zero.
int characterToToken(char sym)
{
    switch ((int) sym)
    {

        case (int) '*':
            return multsym;

        case (int) '+':
            return plussym;

        case (int) '-':
            return minussym;

        case (int) '/':
            return slashsym;

        case (int) '(':
            return lparentsym;

        case (int) ')':
            return rparentsym;

        case (int) ',':
            return commasym;

        case (int) ';':
            return semicolonsym;

        case (int) '.':
            return periodsym;

        case (int) '<':
            return lessym;

        case (int) '>':
            return gtrsym;

        case (int) '=':
            return eqsym;

        case (int) ':':
            return colonsym;

        default:
            return 0;
    }
    return 0;
}

//Takes in a token as an int and returns the character representation of it.
char* tokenToCharacter(int token){
    char* temp;
    switch (token)
    {
        case (int) multsym:
            return "*";

        case (int) plussym:
            return "+";

        case (int) minussym:
            return "-";

        case (int) slashsym:
            return "/";

        case (int) lessym:
            return "<";

        case(int) eqsym:
            return "=";

        case (int) gtrsym:
            return ">";

		case(int) geqsym:
			return ">=";

		case(int) leqsym:
			return "<=";

		case(int) becomessym:
			return ":=";

		case(int) neqsym:
			return "<>";

        case (int) lparentsym:
            return "(";

        case (int) rparentsym:
            return ")";

        case (int) commasym:
            return ",";

        case (int) semicolonsym:
            return ";";

        case (int) periodsym:
            return ".";

        case (int) varsym:
            return "var";

        case (int) beginsym:
            return "begin";

        case (int) identsym:
            strcpy(temp,id_list[id_total_number]);
            id_total_number++;
            return temp;

        case (int) endsym:
            return "end";

        case (int) ifsym:
            return "if";

        case (int) thensym:
            return "then";

        case (int) whilesym:
            return "while";

        case (int) dosym:
            return "do";

        case (int) callsym:
            return "call";

        case (int) constsym:
            return "const";

        case (int) elsesym:
            return "else";

        case (int) procsym:
            return "procedure";

        case (int) readsym:
            return "read";

        case (int) writesym:
            return "write";

        case (int) oddsym:
            return "odd";

        case (int) nulsym:
            return NULL;

        default:
            return 0;
    }
}
