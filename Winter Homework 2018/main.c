#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "monitorInput.h"
#include "monitorPosition.h"

#define DISPLAY_WIDTH 80
#define INITIAL_POSITION 0

#define MAX_LINE_LENGTH 80

int initialWall = -30;
int finalWall = 20;

bool readerHasFinished = false;
int totalInput = 0;
//bool increment = true;

void displayPosition(double position);

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
		// printf("At time: %d change of position: %lf\n", time, change);
		// fflush(stdout);
        appendInput(change);
		totalInput++;
    }
    
    fclose(device_file);
	readerHasFinished = true;

	pthread_exit(NULL);
}

void* modelFunc(void *arg) {
	double change = 0;
	double position = 0;
	int i = 0;

	while(!(readerHasFinished && i == totalInput)) {
		change = takeInput();
		// printf("The position has changed by: %lf\n", change);
		// fflush(stdout);
		
		position += change;
		if (position > finalWall)
			position = finalWall;
		else if (position < initialWall)
			position = initialWall;
			
		// printf("Current position: %lf\n", position);
		// fflush(stdout);
		appendPosition(position);
		i++;
	}
	
	pthread_exit(NULL);
}

void* viewFunc(void *arg) {
	double position = 0;
	int i = 0;
	
	printf("\n\n\n");
	while(!(readerHasFinished && i == totalInput)) {
		position = takePosition();
		// printf("Position displayed: %lf\n", position);
		// fflush(stdout);
		displayPosition(position);
		i++;
	}
	printf("\n\n\n");
	
	pthread_exit(NULL);
}

/*void* controllerFunc(void *arg) {
	double position = 0;
	int i = 0;

	while(true) {
		startTakePosition();
			// printf("The position has changed by: %lf\n", change);
			// fflush(stdout);
		finishTakePosition();			
		
		i = (i + 1) % BUFF_DIMENS;
	}
}*/

int main(void) {
	
	initMonitorInput();
	initMonitorPosition();
	
	pthread_t reader;
	pthread_t model;
	pthread_t view;
	// pthread_t controller;
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
	
	/*if (pthread_create(&controller, NULL, (void *) controllerFunc, (void *) 3) != 0) {
		printf("Error in creating controller thread");
		exit(EXIT_FAILURE);
	}*/
	
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
	
	/*if (pthread_join(controller, NULL) != 0) {
		printf("Error in joining controller thread");
		exit(EXIT_FAILURE);
	}*/
	
	/*if (pthread_join(writer, NULL)) {
		printf("Error in joining writer thread");
		exit(EXIT_FAILURE);
	}*/
	
	closeMonitorInput();
	closeMonitorPosition();
	exit(EXIT_SUCCESS); 
}

void displayPosition(double position) {
	
	int pos;
	if (position >= 0)
		pos = (int) position + 0.5;
	else
		pos = (int) position - 0.5;

	const char prompt[] = "";
	printf("\r%*s\r%s", DISPLAY_WIDTH, "", prompt);
	for (int i = -DISPLAY_WIDTH/2; i < DISPLAY_WIDTH/2; i++) {
		if (i < initialWall - 1 || i > finalWall + 1)
			printf(" ");
		else if (i == initialWall - 1 || i == finalWall + 1)
			printf("|");
		else if (i == pos)
			printf("O");
		else
			printf("-");		
	}
	fflush(stdout);
	usleep(100000);
}
