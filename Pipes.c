#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include "Pipes.h"
#include "SendingResults.h"

#define DEBUG

char buf[1];

/**
 * Synch 
 * 
 * the following two functions are used to send a msg
 * between the parent and the child... somehow this
 * synchronizes the two processes
 */
void childSync(int* parentToChild, int* childToParent) {
	close(parentToChild[1]);
	close(childToParent[0]);
	read(parentToChild[0], buf, 1);
	write(childToParent[1], buf, 1);
}

void parentSync(int* parentToChild, int* childToParent) {
	close(parentToChild[0]);
	close(childToParent[1]);
	write(parentToChild[1], buf, 1);
	read(childToParent[0], buf, 1);
}

/**
 * Child and parent execution on pipes test
 * 
 * for n-times
 * just send around a message from the parent to the child and back
 * 
 */

void childPipes(int* parentToChild, int* childToParent, int n, TimeInfo *timeInfo) {
	buf[0] = 'c';

	struct timeval start, end;
	struct timeval globalStart, globalEnd;
	long int loopTime = 0;

	printf("Child Pipes Test\n");

	gettimeofday(&globalStart, NULL);
	for (int i=0; i < n; ++i) {     
		read(parentToChild[0], buf, 1);
		
		gettimeofday(&end, NULL);
		loopTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		if (i != 0) {
			if (loopTime > timeInfo->max)
				timeInfo->max = loopTime;
			if (loopTime < timeInfo->min)
				timeInfo->min = loopTime;
		}

		#ifdef DEBUG
		printf("Child: got a %c\n", buf[0]);
		buf[0] = 'c';		
		#endif
				
		gettimeofday(&start, NULL);
		write(childToParent[1], buf, 1);
	}

	gettimeofday(&globalEnd, NULL);
	timeInfo->average = ((globalEnd.tv_sec * 1000000 + globalEnd.tv_usec) - (globalStart.tv_sec * 1000000 + globalStart.tv_usec))/n;
	//printf("Child: min: %ld, max: %ld, average: %ld\n", min, max, average);
}

void parentPipes(int* parentToChild, int* childToParent, int n, TimeInfo *timeInfo) {
	buf[0] = 'p';
	
	struct timeval start, end;
	struct timeval globalStart, globalEnd;
	long int loopTime = 0;
	
	printf("Child Parent Test\n");
 
	gettimeofday(&globalStart, NULL);
	for (int i=0; i < n; ++i) {
		gettimeofday(&start, NULL);
		
		write(parentToChild[1], buf, 1);
		read(childToParent[0], buf, 1);
		
		gettimeofday(&end, NULL);
		loopTime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
		
		if (loopTime > timeInfo->max)
			timeInfo->max = loopTime;
		if (loopTime < timeInfo->min)
			timeInfo->min = loopTime;
					
		#ifdef DEBUG
		printf("Parent: got a %c\n", buf[0]);
		buf[0] = 'p';
		#endif
	}
	
	gettimeofday(&globalEnd, NULL);
	timeInfo->average =  ((globalEnd.tv_sec * 1000000 + globalEnd.tv_usec) - (globalStart.tv_sec * 1000000 + globalStart.tv_usec))/n;
	//printf("Parent: min: %ld, max: %ld, average: %ld\n", min, max, average);
}
