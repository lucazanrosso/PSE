#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "SendingResults.h"

void sendResultsToParent(int* parentToChild, int* childToParent, TimeInfo* timeInfo) {
    char buf[200];
    
    read(parentToChild[0], buf, 200); 
    sprintf(buf, "-> Process id is %d,\n-> Round trip times\n\
		-> Average %d,\n-> Maximum %d,\n-> Minimum %d", timeInfo->pid, timeInfo->average, timeInfo->max, timeInfo->min);
    write(childToParent[1], buf, strlen(buf));
}

void printResults(TimeInfo* timeInfo) {
    printf("-> Parent's Results for Signal IPC mechanisms\n");
    printf("-> Process id is %d,\n-> Round trip times\n\
		-> Average %d,\n-> Maximum %d,\n-> Minimum %d\n", timeInfo->pid, timeInfo->average, timeInfo->max, timeInfo->min);
}
        
void getAndPrintResultsFromChild(int* parentToChild, int* childToParent) {
    char buf[200];
    sprintf(buf, "Go"); 
    write(parentToChild[1], buf, strlen(buf));
    
    int noReadChars = read(childToParent[0], buf, 200);
    buf[noReadChars] = '\0';
    printf("-> Child's Results for Signal IPC mechanisms\n");
    printf("%s\n", buf);
}
