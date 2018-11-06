/*
Domande
1 - Sleep in Signals
2 - Array buf in Pipes, è un'unica istanza
3 - Dichiarazione di TimeInfo in main
4 - A capo nell'ultimo got a signal...
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

#include "Pipes.h"
#include "Signals.h"
#include "ParsingInput.h"
#include "SendingResults.h"

void mySigHandler ( int signo ) {
	printf("   OUCH! I got signal %d\n", signo);
}

int main(int argc, char **argv) { 

	if (!checkInput(argc, argv))
		exit(EXIT_FAILURE);
	
	int n = 100;
	char selection;
	
	parseInput(argc, argv, &selection, &n);
	
	
	struct sigaction mySigActions;

	/* setup the SIGHUP */
	mySigActions.sa_handler = mySigHandler;
	sigemptyset(&mySigActions.sa_mask);
	mySigActions.sa_flags = 0;
	if ( sigaction (SIGHUP, &mySigActions, NULL) < 0) {
		perror ( " Sigaction failed for SIGHUP " ) ;
		exit (1) ;
	} 

	 
	/* setup the SIGINT */
	mySigActions.sa_handler = mySigHandler;
	sigemptyset(&mySigActions.sa_mask);
	mySigActions.sa_flags = 0;
	if ( sigaction ( SIGINT , &mySigActions , NULL ) < 0) {
		perror ( " Sigaction failed SIGINT " ) ;
		exit (1) ;
	}


	printf("Type of Test: ");
	if (selection == 's')
		printf("signals\n");
	else 
		printf("pipes\n");
	
	printf("Number of samples to collect: %d\n", n);
    
	// instantiate two arrays to use as pipes
	int parentToChild[2], childToParent[2]; 

	// Check that pipes have been created correctly
	if (pipe(parentToChild) < 0 || pipe(childToParent) < 0) {
		printf("An error has been detected during the creation of pipes");
		exit(EXIT_FAILURE);
	} 
		
	// Fork the child process   
	pid_t pid = fork();
	
	TimeInfo timeInfo;
	timeInfo.pid = getpid();
	timeInfo.average = 0;
	timeInfo.max = 0;
	timeInfo.min = INT_MAX;
	
	// Check if it is the parent or child process
	switch ( pid ) {
	case -1:
		// Error
		fprintf (stderr, "Error doing a fork!\n " ) ;
		exit ( -1) ;
	case 0:
		// Child process
		childSync(parentToChild, childToParent);
		if (selection == 'p')
			childPipes(parentToChild, childToParent, n, &timeInfo);
		else
			childSignals(getppid(), n, &timeInfo);
		sendResultsToParent(parentToChild, childToParent, &timeInfo);
		break ;
	default :
		// Parent process
		parentSync(parentToChild, childToParent);
		if (selection == 'p')
			parentPipes(parentToChild, childToParent, n, &timeInfo);
		else
			parentSignals(pid, n, &timeInfo);
		printf("\n\n");
        printResults(&timeInfo);
        printf("\n\n");
        getAndPrintResultsFromChild(parentToChild, childToParent);
        printf("\n\n");
		// Wait the child to terminate
		wait ( NULL ) ;
	} 

	exit(EXIT_SUCCESS); 
}
