#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "readersAndWriters.h"

// Global variables

int readers = 0;
int writers = 0;
pthread_cond_t OKtoRead;
pthread_cond_t OKtoWrite;

pthread_mutex_t mtx; 
// need this mutex 'cause POSIX thread library does not have monitor
int waitingWriters = 0;
int waitingReaders = 0;
// need these because we don't have an empty(cond) in POSIX thread library
 
void startRead() {
	pthread_mutex_lock(&mtx);

	while ( (writers != 0) || (waitingWriters != 0) ) {
		waitingReaders++; 
		pthread_cond_wait(&OKtoRead, &mtx);
		waitingReaders--;
		if (writers == 0)
			break;
	}  
	readers++;

	pthread_mutex_unlock(&mtx);
	// no need of cascaded unblocking... we have broadcast!!!
}

void endRead() {
	pthread_mutex_lock(&mtx);

	readers--;
	if (readers == 0)
		pthread_cond_signal(&OKtoWrite);

	pthread_mutex_unlock(&mtx);
}

void startWrite() {
	pthread_mutex_lock(&mtx);

	while ( (writers != 0) || (readers != 0) ) {
		  waitingWriters++;
		  pthread_cond_wait(&OKtoWrite, &mtx);
		  waitingWriters--;
		  // printf("start %d %d %d %d ", readers, writers, waitingReaders, waitingWriters);
	}
	writers++;

	pthread_mutex_unlock(&mtx);
}

void endWrite() {
	pthread_mutex_lock(&mtx);
	writers--;	
	if (waitingReaders == 0) {
		// printf("start %d %d %d %d ", readers, writers, waitingReaders, waitingWriters);
		pthread_cond_signal(&OKtoWrite); 		
	}		
	else {
		pthread_cond_broadcast(&OKtoRead);
		//printf("start %d %d %d %d ", readers, writers, waitingReaders, waitingWriters);
	}

	pthread_mutex_unlock(&mtx);  
}

void initMonitor() {
	int status;
	status = pthread_mutex_init(&mtx, NULL);
	if (status != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
	status = pthread_cond_init(&OKtoRead, NULL);
	if (status != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}
	status = pthread_cond_init(&OKtoWrite, NULL);
	if (status != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	readers = 0;
	writers = 0;
	waitingReaders = 0;
	waitingWriters = 0;
}

void closeMonitor() {
	int status;
	status = pthread_mutex_destroy(&mtx);  
	if (status != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}

	status = pthread_cond_destroy(&OKtoRead);
	if (status != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}

	status = pthread_cond_destroy(&OKtoWrite);
	if (status != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
}
