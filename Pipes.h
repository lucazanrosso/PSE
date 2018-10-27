#ifndef Pipes_h
#define Pipes_h

#include "SendingResults.h"

void childSync(int* parentToChild, int* childToParent);
void parentSync(int* parentToChild, int* childToParent);

void childPipes(int* parentToChild, int* childToParent, int n, TimeInfo *timeInfo);
void parentPipes(int* parentToChild, int* childToParent, int n, TimeInfo *timeInfo);

#endif
