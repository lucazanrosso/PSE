#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

double devicePosition;
double deviceRemotePosition;

pthread_mutex_t mtxPosition;
pthread_mutex_t mtxRemotePosition;
 
void appendPosition(double position) {
	if (pthread_mutex_lock(&mtxPosition) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	devicePosition = position;

	if (pthread_mutex_unlock(&mtxPosition) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void appendRemotePosition(double position) {
	if (pthread_mutex_lock(&mtxRemotePosition) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	deviceRemotePosition = position;
	
	if (pthread_mutex_unlock(&mtxRemotePosition) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

double takePosition() {
	if (pthread_mutex_lock(&mtxPosition) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}

	double position = devicePosition;
	
	if (pthread_mutex_unlock(&mtxPosition) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
	
	return position;
}

double takeRemotePosition() {
	if (pthread_mutex_lock(&mtxRemotePosition) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}	
	
	double position = deviceRemotePosition;	
	
	if (pthread_mutex_unlock(&mtxRemotePosition) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}	
	
	return position;
}

void initMonitorPosition(double position) {
	devicePosition = position;
	
	if (pthread_mutex_init(&mtxPosition, NULL) != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
}

void closeMonitorPosition() {	
	if (pthread_mutex_destroy(&mtxPosition) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}
}
