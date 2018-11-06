#ifndef ParsingInput_h
#define ParsingInput_h

#define TRUE 1
#define FALSE 0

void printHelp();
int checkInput(int numArg, char **arguments);
void parseInput(int numArg, char **arguments, char* currentTest, int* num);

#endif