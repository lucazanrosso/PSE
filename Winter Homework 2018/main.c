#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "readerAndModel.h"

#define WIDTH 80
#define HEIGHT 9

#define BUFF_DIMENS 10
#define NUM_THREAD 5

int start = -30;
int finish = 20;
int position = 0;
bool increment = true;

double deviceInput[BUFF_DIMENS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
double devicePosition[BUFF_DIMENS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//static void redraw(void);

void* readerFunc(void *arg) {
	double delta = 0;
	int i = 0;

	while(true) {
		delta = (double)rand() / (double)RAND_MAX;
		startAppend();
		deviceInput[i] = delta;
		printf("reader %d: %f\n", i, delta);
		//fflush(stdout);
		finishAppend();

		i = (i + 1) % BUFF_DIMENS;
	}
}

void* modelFunc(void *arg) {
	double delta = 0;
	int i = 0;

	while(true) {
		startTake();
		delta = deviceInput[i];
		printf("model %d: %f\n", i, delta);
		//fflush(stdout);
		finishTake();
		i = (i + 1) % BUFF_DIMENS;
	}
}

int main(void) {
	
	initMonitor(BUFF_DIMENS);
	
	pthread_t reader;
	pthread_t model;
	/*pthread_t view;
	pthread_t controller;
	pthread_t writer;*/
	
	if (pthread_create(&reader, NULL, (void *) readerFunc, (void *) 0)) {
		printf("Error in creating readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&model, NULL, (void *) modelFunc, (void *) 1)) {
		printf("Error in creating readers threads");
		exit(EXIT_FAILURE);
	}
	
	/*if (pthread_create(&view, NULL, (void *) viewFunc, (void *) 2)) {
		printf("Error in creating readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&controller, NULL, (void *) controllerFunc, (void *) 3)) {
		printf("Error in creating readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&writer, NULL, (void *) writerFunc, (void *) 4)) {
		printf("Error in creating readers threads");
		exit(EXIT_FAILURE);
	}*/
	
	if (pthread_join(reader, NULL)) {
		printf("Error in joining readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(model, NULL)) {
		printf("Error in joining readers threads");
		exit(EXIT_FAILURE);
	}
	
	/*if (pthread_join(view, NULL)) {
		printf("Error in joining readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(controller, NULL)) {
		printf("Error in joining readers threads");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(writer, NULL)) {
		printf("Error in joining readers threads");
		exit(EXIT_FAILURE);
	}*/
	
	closeMonitor();
	exit(EXIT_SUCCESS); 
	
    /*const int trigger   = (CLOCKS_PER_SEC * 50) / 1000;  // 500 ms in clocks.
    clock_t   prevClock = clock() - trigger;
	
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	fflush(stdout);
	
    while (1) {
		clock_t curClock = clock();

        if (curClock - prevClock >= trigger) {
            prevClock = curClock;
            redraw();
        }
		
    }*/
}

/*static void redraw(void) {

	const char prompt[] = "";
	printf("\r%*s\r%s", WIDTH, "", prompt);
	for (int i = -WIDTH/2; i < WIDTH/2; i++) {
		if (i < start - 1 || i > finish + 1)
			printf(" ");
		else if (i == start - 1 || i == finish + 1)
			printf("|");
		else if (i == position)
			printf("O");
		else
			printf("-");		
	}
	fflush(stdout);
	
	if (position == finish)
		increment = false;
	else if (position == start)
		increment = true;
	
	if (increment)
		position++;
	else
		position--;
}*/
