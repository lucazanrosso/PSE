#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BUFF_DIMENS 10

pthread_cond_t notEmptyPosition[2];
pthread_cond_t notFullPosition;
int availPosition[2];
double devicePosition[BUFF_DIMENS];
int producerPosition;
int consumerPosition[2];

pthread_mutex_t mtx;
 
void appendPosition(double change) {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	
	while (availPosition[0] == BUFF_DIMENS || availPosition[1] == BUFF_DIMENS) {
		if (pthread_cond_wait(&notFullPosition, &mtx) != 0) {
				printf("pthread_cond_wait\n");
				exit(EXIT_FAILURE);
		}
	}
	
	devicePosition[producerPosition] = change;

	availPosition[0]++;
	availPosition[1]++;

	if (pthread_cond_signal(&notEmptyPosition[0]) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_signal(&notEmptyPosition[1]) != 0) {
		printf("pthread_cond_signal\n");
		exit(EXIT_FAILURE);
	}
	
	producerPosition = (producerPosition + 1) % BUFF_DIMENS;

	if (pthread_mutex_unlock(&mtx) != 0) {
		printf("pthread_mutex_unlock\n");
		exit(EXIT_FAILURE);
	}
}

double takePosition(int consumer) {
	if (pthread_mutex_lock(&mtx) != 0) {
		printf("pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}

	while (availPosition[consumer] == 0) {
		if (pthread_cond_wait(&notEmptyPosition[consumer], &mtx) != 0) {
			printf("pthread_cond_wait\n");
			exit(EXIT_FAILURE);
		}			
	}
	
	double change = devicePosition[consumerPosition[consumer]];
	
	availPosition[consumer]--;

	if (availPosition[consumer] >= availPosition[sizeof(availPosition) - consumer - 1])
		if (pthread_cond_signal(&notFullPosition) != 0) {
			printf("pthread_cond_signal\n");
			exit(EXIT_FAILURE);
		}
	
	consumerPosition[consumer] = (consumerPosition[consumer] + 1) % BUFF_DIMENS;
	
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
	
	if (pthread_cond_init(&notEmptyPosition[0], NULL) !=0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_init(&notEmptyPosition[1], NULL) !=0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&notFullPosition, NULL) != 0) {
		printf("Error in cond init\n");
		exit(EXIT_FAILURE);
	}

	availPosition[0] = 0;
	availPosition[1] = 0;
	
	producerPosition = 0;
	consumerPosition[0] = 0;
	consumerPosition[1] = 0;
}

void closeMonitorPosition() {	
	if (pthread_mutex_destroy(&mtx) != 0) {
		printf("Error in mutex destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notEmptyPosition[0]) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_cond_destroy(&notEmptyPosition[1]) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_destroy(&notFullPosition) != 0) {
		printf("Error in cond destroy\n");
		exit(EXIT_FAILURE);
	}
}
