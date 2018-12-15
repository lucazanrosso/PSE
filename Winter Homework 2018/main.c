#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "monitorInput.h"
#include "monitorPosition.h"

#define WIDTH 80
#define HEIGHT 9

#define MAX_LINE_LENGTH 80

// int start = -30;
// int finish = 20;
// int position = 0;
bool finish = false;
int totalInput = 0;
//bool increment = true;

//static void redraw(void);

void* readerFunc(void *arg) {
	
	FILE *device_file;
    char line[MAX_LINE_LENGTH]; 
    
    device_file = fopen("device.txt", "r"); 
    
    if (device_file == NULL) {
        printf("Can't open device.txt\n");
        exit(EXIT_FAILURE);
    }
    
    int time;
    double change;
    
    while (fgets(line, sizeof(line), device_file) ) {
        sscanf(line, "%d %lf", &time, &change);
		printf("At time: %d change of position: %lf\n", time, change);
		fflush(stdout);
        appendInput(change);
		totalInput++;
    }
    
    fclose(device_file);
	
	finish = true;

	pthread_exit(NULL);
}

void* modelFunc(void *arg) {
	double change = 0;
	double position = 0;
	int i = 0;

	while(!(finish && i == totalInput)) {
		change = takeInput();
		printf("The position has changed by: %lf\n", change);
		fflush(stdout);
		
		position += change;
			
		printf("Current position: %lf\n", position);
		fflush(stdout);
		appendPosition(position);
		i++;
	}
	
	pthread_exit(NULL);
}

void* viewFunc(void *arg) {
	double position = 0;
	int i = 0;

	while(!(finish && i == totalInput)) {
		position = takePosition();
		printf("Position displayed: %lf\n", position);
		fflush(stdout);
		i++;
	}
	
	pthread_exit(NULL);
}

void* controllerFunc(void *arg) {
	/*double position = 0;
	int i = 0;

	while(true) {
		startTakePosition();
			// printf("The position has changed by: %lf\n", change);
			// fflush(stdout);
		finishTakePosition();			
		
		i = (i + 1) % BUFF_DIMENS;
	}*/
}

int main(void) {
	
	initMonitorInput();
	initMonitorPosition();
	
	pthread_t reader;
	pthread_t model;
	pthread_t view;
	pthread_t controller;
	// pthread_t writer;
	
	if (pthread_create(&reader, NULL, (void *) readerFunc, (void *) 0) != 0) {
		printf("Error in creating reader thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&model, NULL, (void *) modelFunc, (void *) 1) != 0) {
		printf("Error in creating model thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&view, NULL, (void *) viewFunc, (void *) 2) != 0) {
		printf("Error in creating view thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&controller, NULL, (void *) controllerFunc, (void *) 3) != 0) {
		printf("Error in creating controller thread");
		exit(EXIT_FAILURE);
	}
	
	/*if (pthread_create(&writer, NULL, (void *) writerFunc, (void *) 4) != 0) {
		printf("Error in creating writer thread");
		exit(EXIT_FAILURE);
	}*/
	
	if (pthread_join(reader, NULL)) {
		printf("Error in joining reader thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(model, NULL) != 0) {
		printf("Error in joining model thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(view, NULL) != 0) {
		printf("Error in joining view thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(controller, NULL) != 0) {
		printf("Error in joining controller thread");
		exit(EXIT_FAILURE);
	}
	
	/*if (pthread_join(writer, NULL)) {
		printf("Error in joining writer thread");
		exit(EXIT_FAILURE);
	}*/
	
	closeMonitorInput();
	closeMonitorPosition();
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
