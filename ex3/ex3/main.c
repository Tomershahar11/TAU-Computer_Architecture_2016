/*
ex3 - Main.c:
- the program recieves the following arguments:
@param Command line: The text file that includes all banking operations the program should perform.
@param Balance report file: The output file of the program. This file sums all balances in bank account. 
@param RunTime log file: The log file of the program. This file will centralize the program runtime documentation
- The program manages operational and managable bank accounts.
*/

/* Libraries: */
#include <stdio.h>
#include "Header.h"

int main(int argc, char *argv[]) {

	int returnValueFromBankManager = 0;

	returnValueFromBankManager = executeBankManager(argc, argv);

	return 0;
}