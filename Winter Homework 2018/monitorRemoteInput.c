#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFF_DIMENS 5

pthread_cond_t notEmpyRemoteInput;
pthread_cond_t notFullRemoteInput;
int availRemoteInput;
double deviceRemoteInput[BUFF_DIMENS];
int producerRemoteInputIndex;
int consumerRemoteInputIndex;
bool extern untilIDie;

pthread_mutex_t mtxRemoteInput;
 
void appendRemoteInput(double change) {
	if (pthread_mutex_lock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	while (availRemoteInput == BUFF_DIMENS) {
		if (pthread_cond_wait(&notFullRemoteInput, &mtxRemoteInput) != 0) {
				printf("pthread_cond_wait\n");
				exit(EXIT_FAILURE);
		}
		if (!untilIDie) {
			if (pthread_mutex_unlock(&mtxRemoteInput) != 0) {
				printf("pthread_mutex_unlock\n");
				exit(EXIT_FAILURE);
			}
			return;
		}
	}
	
	deviceRemoteInput[producerRemoteInputIndex] = change;
	producerRemoteInputIndex = (producerRemoteInputIndex + 1) % BUFF_DIMENS;
	
	availRemoteInput++;

	if (pthread_cond_signal(&notEmpyRemoteInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_unlock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

double takeRemoteInput() {
	if (pthread_mutex_lock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}

	while (availRemoteInput == 0) {
		if (pthread_cond_wait(&notEmpyRemoteInput, &mtxRemoteInput) != 0) {
			printf("pthread_cond_wait\n");
			exit(EXIT_FAILURE);
		}
		if (!untilIDie) {
			if (pthread_mutex_unlock(&mtxRemoteInput) != 0) {
				printf("pthread_mutex_unlock\n");
				exit(EXIT_FAILURE);
			}
			return 0;
		}
	}
	
	double change = deviceRemoteInput[consumerRemoteInputIndex];
	consumerRemoteInputIndex = (consumerRemoteInputIndex + 1) % BUFF_DIMENS;
	
	availRemoteInput--;

	if (pthread_cond_signal(&notFullRemoteInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_mutex_unlock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
	
	return change;
}

void forceSignalingRemoteInput() {
	if (pthread_mutex_lock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_signal(&notFullRemoteInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_signal(&notEmpyRemoteInput) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_mutex_unlock(&mtxRemoteInput) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

void initMonitorRemoteInput() {	
	if (pthread_mutex_init(&mtxRemoteInput, NULL) != 0) {
		printf("Error in mutex init\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_init(&notEmpyRemoteInput, NULL) !=0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&notFullRemoteInput, NULL) != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	availRemoteInput = 0;	
	producerRemoteInputIndex = 0;
	consumerRemoteInputIndex = 0;
}

void closeMonitorRemoteInput() {	
	if (pthread_mutex_destroy(&mtxRemoteInput) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notEmpyRemoteInput) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notFullRemoteInput) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
}
