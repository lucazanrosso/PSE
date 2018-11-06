#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ParsingInput.h"

void printHelp() {
  printf("Usage: ./main [-s|-p] [-n n]\n\
    \t-s : use signals\n \
    \t-p : use pipes\n \
    \t-n n: number of samples to collect (i.e. number of tests)\n");
}

int checkInput(int numArg, char **arguments) {
  // Check the number of arguments
	if (! ((numArg >= 2) && (numArg <= 4 ) ) ) {
      printHelp(); 
			return FALSE;
  }
	// Use strcmp to compares two strings
  if ( (strcmp(arguments[1], "-s") !=0 )  && (strcmp(arguments[1], "-p") != 0 ) ) {
      printHelp(); 
			return FALSE;
  }
  if (numArg == 3){
      printHelp(); 
			return FALSE;
  }
  if ( (numArg == 4) && (strcmp(arguments[2], "-n") != 0) ) {
      printHelp(); 
			return FALSE;
  }
  
  return TRUE;
}

void parseInput(int numArg, char **arguments, char* selection, int* num) {
    if ((strcmp(arguments[1], "-s") == 0))
        *selection =  's';
    else
        *selection = 'p';
    ( ( numArg == 4 ) ? (*num = atoi(arguments[3])) : (*num = 100));
  
}