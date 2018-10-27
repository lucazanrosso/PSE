#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include "Signals.h"
#include "SendingResults.h"

#define DEBUG

// this variable is used by the handlerForComm. We cannot pass as a parameter (only int signo)
// we are forced to declare it as a global variable
int signalsToBeSent = 0; 
pid_t receiver = -1;

struct timeval start, end;
struct timeval globalStart, globalEnd;
long int loopTime = 0;

void handlerForComm(int signo) {
	if (signalsToBeSent > 0) {
		#ifdef DEBUG
		if (receiver == getppid()) 
				printf("got a signal.. send a kill to parent\n");
		else
				printf("got a signal.. send a kill to child\n");
		fflush(stdout);
		#endif
		kill(receiver, SIGUSR1);
		signalsToBeSent--;
	}
}

void childSignals(pid_t sendTo, int n, TimeInfo *timeInfo)  {
	printf("Child Signals Test\n"); 

	struct sigaction sigActionsForComm;  
	sigActionsForComm.sa_handler = handlerForComm;
	sigemptyset(&sigActionsForComm.sa_mask);
	sigActionsForComm.sa_flags = 0;
		
	if ( sigaction (SIGUSR1, &sigActionsForComm, NULL) < 0) {
		perror ( " Sigaction failed for SIGUSER1 " ) ;
		exit (EXIT_FAILURE) ;
	}
	
	receiver = sendTo;
	signalsToBeSent = n;
	
	gettimeofday (&globalStart, NULL);
	
	while (signalsToBeSent > 0) {
		gettimeofday(&start, NULL);
		pause();
		gettimeofday(&end, NULL);
		loopTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		if (signalsToBeSent != n) {
			if (loopTime > timeInfo->max)
				timeInfo->max = loopTime;
			if (loopTime < timeInfo->min)
				timeInfo->min = loopTime;
		}
	}
		
	gettimeofday(&globalEnd, NULL);
	timeInfo->average = ((globalEnd.tv_sec * 1000000 + globalEnd.tv_usec) - (globalStart.tv_sec * 1000000 + globalStart.tv_usec))/n;
	//printf("Child: min: %ld, max: %ld, average: %ld\n", min, max, average);

}


void parentSignals(pid_t sendTo, int n, TimeInfo *timeInfo)  {
	printf("Parent Signals Test\n");   
			
	struct sigaction sigActionsForComm;      
	sigActionsForComm.sa_handler = handlerForComm;
	sigemptyset(&sigActionsForComm.sa_mask);
	sigActionsForComm.sa_flags = 0;

	if ( sigaction (SIGUSR1, &sigActionsForComm, NULL) < 0) {
		perror ( " Sigaction failed for SIGUSER1 " ) ;
		exit (EXIT_FAILURE) ;
	} 
	
	//sleep(1);

	receiver = sendTo;
	signalsToBeSent = n-1;
	
	gettimeofday (&globalStart, NULL);

	printf("... send a kill to child\n");
	kill(receiver, SIGUSR1);

	gettimeofday (&globalStart, NULL);
	
	while (signalsToBeSent > 0) {
		gettimeofday(&start, NULL);
		pause();
		gettimeofday(&end, NULL);
		loopTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		if (loopTime > timeInfo->max)
			timeInfo->max = loopTime;
		if (loopTime < timeInfo->min)
			timeInfo->min = loopTime;
	}

	gettimeofday(&globalEnd, NULL);
	timeInfo->average = ((globalEnd.tv_sec * 1000000 + globalEnd.tv_usec) - (globalStart.tv_sec * 1000000 + globalStart.tv_usec))/n;
	//printf("Parent: min: %ld, max: %ld, average: %ld\n", min, max, average);

}
