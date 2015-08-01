Samir Ketema
Assignment 4
COP 3402 - Systems Software
_______________________________________________________________


To compile the Pl/0 Compiler using gcc:

	gcc -o compiler compiler.c -lm

To run the PL/0 Compiler, replace the "input.txt" with any input file you wish:

	./compiler

To view output files (for example, output.txt):

	vi output.txt



________



Lexical Analyzer/Scanner
	Input:  input.txt
	output: LAout.txt

Parser/Code Generator
	Input:  LAout.txt
	Output: code.txt

Virtual Machine
	Input:  code.txt
	Output: output.txt
