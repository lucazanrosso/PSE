#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BUFF_DIMENS 10

pthread_cond_t notEmpy;
pthread_cond_t notFull;
int avail;
double devicePosition[BUFF_DIMENS];
int iPosition = 0;
int jPosition = 0;

pthread_mutex_t mtx;
 
void appendPosition(double change) {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	while (avail == BUFF_DIMENS) {
		if (pthread_cond_wait(&notFull, &mtx) != 0) {
				printf("pthread_cond_wait\n");
				exit(EXIT_FAILURE);
		}
	}
	
	devicePosition[iPosition] = change;

	avail++;

	if (pthread_cond_signal(&notEmpy) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	iPosition = (iPosition + 1) % BUFF_DIMENS;

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

	while (avail == 0) {
		if (pthread_cond_wait(&notEmpy, &mtx) != 0) {
			printf("pthread_cond_wait\n");
			exit(EXIT_FAILURE);
		}			
	}
	
	double change = devicePosition[jPosition];
	
	avail--;

	if (pthread_cond_signal(&notFull) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	jPosition = (jPosition + 1) % BUFF_DIMENS;
	
	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
	
	return change;
}

void initMonitorPosition() {	
	if (pthread_mutex_init(&mtx, NULL) != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_init(&notEmpy, NULL) !=0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&notFull, NULL) != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	avail = 0;
}

void closeMonitorPosition() {	
	if (pthread_mutex_destroy(&mtx) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notEmpy) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notFull) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
}
