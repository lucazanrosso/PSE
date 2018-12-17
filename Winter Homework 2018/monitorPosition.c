#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

double devicePosition;

pthread_mutex_t mtx;
 
void appendPosition(double position) {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	devicePosition = position;

	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

double takePosition() {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}

	double position = devicePosition;
	
	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
	
	return position;
}

void initMonitorPosition(double position) {
	devicePosition = position;
	
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
}

void closeMonitorPosition() {	
	if (pthread_mutex_destroy(&mtx) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}
}
