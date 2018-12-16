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
int timeView = 12;
int timeController = 20;

bool readerHasFinished = false;
bool modelHasFinished = false;
int totalInput = 0;

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
    
    while (fgets(line, sizeof(line), device_file) != NULL) {
        sscanf(line, "%d %lf", &time, &change);
        appendInput(change);
		// printf("At time: %d change of position: %lf:\n", time, change);
		// fflush(stdout);
		totalInput++;
    }
    

    fclose(device_file);
	readerHasFinished = true;

	pthread_exit(NULL);
}

void* modelFunc(void *arg) {
	double change = 0;
	double position = INITIAL_POSITION;
	int i = 0;

	while(!(readerHasFinished && i == totalInput)) {
		change = takeInput();
		// printf("The position %d has changed by: %lf\n", i, change);
		// fflush(stdout);
		
		position += change;
		if (position > finalWall)
			position = finalWall;
		else if (position < initialWall)
			position = initialWall;
			
		appendPosition(position);
		// printf("Model position %d: %lf\n", i, position);
		// fflush(stdout);
		i++;
	}
	
	modelHasFinished = true;
	
	pthread_exit(NULL);
}

void* viewFunc(void *arg) {
	double position = 0;
	int i = 0;
	
	printf("\n\n\n");
	while(!(modelHasFinished && i == totalInput)) {
		position = takePosition(0);
		// printf("View position %d: %lf\n", i, position);
		// fflush(stdout);
		displayPosition(position);
		i++;
		usleep(timeView * 1000);
	}
	printf("\n\n\n");
	
	pthread_exit(NULL);
}

void* controllerFunc(void *arg) {
	double position = 0;
	int i = 0;
	
	FILE *outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        printf("Can't open output.txt\n");
        exit(EXIT_FAILURE);
    }
	
	while(!(modelHasFinished && i == totalInput)) {
		position = takePosition(1);
		// printf("Controller position %d: %lf\n", i, position);
		// fflush(stdout);
		// usleep(timeController * 100);
		fprintf(outputFile, "%d %lf \n", i, position);
		i++;
	}
	
	fclose(outputFile);
	
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	
	if (argc != 5) {
		printf("Wrong command line input");
		exit(EXIT_FAILURE);
	}
	
	initialWall = strtol(argv[1], NULL, 10);
	finalWall = strtol(argv[2], NULL, 10);
	timeController = strtol(argv[3], NULL, 10);
	timeView = strtol(argv[4], NULL, 10);	
	
	initMonitorInput();
	initMonitorPosition();
	
	pthread_t reader;
	pthread_t model;
	pthread_t view;
	pthread_t controller;
	
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
}