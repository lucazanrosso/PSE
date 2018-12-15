#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

pthread_cond_t notEmpy;
pthread_cond_t notFull;
int avail;
int buff_dim;

pthread_mutex_t mtx;
 
void startAppend() {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	while (avail == buff_dim) {
		if (pthread_cond_wait(&notFull, &mtx) != 0) {
				printf("pthread_cond_wait\n");
				exit(EXIT_FAILURE);
		}
	}			


	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void finishAppend() {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	avail++;

	if (pthread_cond_signal(&notEmpy) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void startTake() {
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
	
	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void finishTake() {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	avail--;

	if (pthread_cond_signal(&notFull) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	} 
}

void initMonitor(int dim) {	
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

	buff_dim = dim;
	avail = 0;
}

void closeMonitor() {	
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
