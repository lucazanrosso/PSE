#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "readersAndWriters.h"

#define READING_TIME 0.5
#define READERS_OTHER_TIME 5
#define WRITING_TIME 2
#define WRITERS_OTHER_TIME 0.5

#define NUM_READERS 5
#define NUM_WRITERS 5


void* readerFunc(void *arg) {
	int readerNo = (int) arg;
	for(;;) {
		startRead();
		printf("R%d: startRead\n", readerNo);
		sleep(READING_TIME);
		printf("R%d: stopRead\n", readerNo);
		endRead();
		sleep(READERS_OTHER_TIME);
	}
}

void * writerFunc(void *arg) {
	int writerNo = (int) arg;
	for(;;) {
		startWrite();
		printf("W%d: startWrite\n", writerNo);
		sleep(WRITING_TIME);
		endWrite();
		printf("W%d: stopWrite\n", writerNo);
		// endWrite();
		sleep(WRITERS_OTHER_TIME);
	}
}
 
int main() {

	int status;
	initMonitor();

	pthread_t readers[NUM_READERS];
	pthread_t writers[NUM_WRITERS];

	for (int i = 0; i < NUM_READERS; ++i) {
		status = pthread_create(&readers[i], NULL, (void *)readerFunc, (void *) i);
		if (status != 0) {
			printf("Error in creating readers threads");
			exit(EXIT_FAILURE);
		}
	}    

	for (int i = 0; i < NUM_WRITERS; ++i) {
		status = pthread_create(&writers[i], NULL, (void *)writerFunc, (void *) i);
		if (status != 0) {
			printf("Error in creating writer threads");
			exit(EXIT_FAILURE);
		}
	}
	  
	for (int i = 0; i < NUM_READERS; ++i) {
		status = pthread_join(readers[i], NULL);
		if (status != 0) {
			printf("Error in joining readers threads");
			exit(EXIT_FAILURE);
		}
	}  

	for (int i = 0; i < NUM_WRITERS; ++i) {
		status = pthread_join(writers[i], NULL);
		if (status != 0) {
			printf("Error in joining writers threads");
			exit(EXIT_FAILURE);
		}
	}  
	 
	closeMonitor();
	exit(EXIT_SUCCESS);  
}
