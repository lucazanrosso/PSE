#ifndef SendingResults_h
#define SendingResults_h

#include <sys/types.h>

// This data type has been defined to store
// statistica data tracked by a process
typedef struct{
    pid_t pid;     // who is the processor
    int average;   // computed values
    int max; 
    int min;
} TimeInfo;

void sendResultsToParent(int* parentToChild, int* childToParent, TimeInfo* timeInfo);

void printResults(TimeInfo* timeInfo);
        
void getAndPrintResultsFromChild(int* parentToChild, int* childToParent);

#endif