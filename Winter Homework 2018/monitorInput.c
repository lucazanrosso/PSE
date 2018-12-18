#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BUFF_DIMENS 5

pthread_cond_t notEmpyInput;
pthread_cond_t notFullInput;
int availInput;
double deviceInput[BUFF_DIMENS];
int producerIndex;
int consumerIndex;
bool iHaveToDie = false;

pthread_mutex_t mtxInput;
 
void appendInput(double change) {
	if (pthread_mutex_lock(&mtxInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	while (availInput == BUFF_DIMENS) {
		if (pthread_cond_wait(&notFullInput, &mtxInput) != 0) {
				printf("pthread_cond_wait\n");
				exit(EXIT_FAILURE);
		}
		if (iHaveToDie) {
			if (pthread_mutex_unlock(&mtxInput) != 0) {
				printf("pthread_mutex_unlock\n");
				exit(EXIT_FAILURE);
			}
			return;
		}
	}
	
	deviceInput[producerIndex] = change;
	producerIndex = (producerIndex + 1) % BUFF_DIMENS;
	
	availInput++;
	// printf("availInput: %d\n", availInput);
	// fflush(stdout);

	if (pthread_cond_signal(&notEmpyInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_unlock(&mtxInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

double takeInput() {
	if (pthread_mutex_lock(&mtxInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}

	while (availInput == 0) {
		if (pthread_cond_wait(&notEmpyInput, &mtxInput) != 0) {
			printf("pthread_cond_wait\n");
			exit(EXIT_FAILURE);
		}
		if (iHaveToDie) {
			if (pthread_mutex_unlock(&mtxInput) != 0) {
				printf("pthread_mutex_unlock\n");
				exit(EXIT_FAILURE);
			}
			return 0;
		}
	}
	
	double change = deviceInput[consumerIndex];
	consumerIndex = (consumerIndex + 1) % BUFF_DIMENS;
	
	availInput--;

	if (pthread_cond_signal(&notFullInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_mutex_unlock(&mtxInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
	
	return change;
}

void forceSignalingInput() {
	if (pthread_mutex_lock(&mtxInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	iHaveToDie = true;
	
	if (pthread_cond_signal(&notFullInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_signal(&notEmpyInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_mutex_unlock(&mtxInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void initMonitorInput() {	
	if (pthread_mutex_init(&mtxInput, NULL) != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_init(&notEmpyInput, NULL) !=0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&notFullInput, NULL) != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	availInput = 0;	
	producerIndex = 0;
	consumerIndex = 0;
}

void closeMonitorInput() {	
	if (pthread_mutex_destroy(&mtxInput) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notEmpyInput) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notFullInput) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
}
