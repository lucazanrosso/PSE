#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "monitorInput.h"
#include "monitorPosition.h"

#define DISPLAY_WIDTH 80
#define INITIAL_POSITION 0

#define MAX_LINE_LENGTH 80

int initialWall;
int finalWall;

sigset_t signalMask;
int untilIDie;

void displayPosition(double position);

void* interfaceFunc(void *arg) {
	
	FILE *device_file;
    char line[MAX_LINE_LENGTH]; 
    
    device_file = fopen("device.txt", "r"); 
    
    if (device_file == NULL) {
        printf("Can't open device.txt\n");
        exit(EXIT_FAILURE);
    }
    
    int time;
    double change;
    
    while (untilIDie) {
		if (fgets(line, sizeof(line), device_file) != NULL)
			sscanf(line, "%d %lf", &time, &change);
		else
			change = 0;
		// printf("At time: %d change of position: %lf\n", time, change);
		// fflush(stdout);
		appendInput(change);
		usleep(10000);
    }

    fclose(device_file);

	printf("Closing interface thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* modelFunc(void *arg) {
	double change = 0;
	double position = INITIAL_POSITION;
	int i = 0;
	
	while(untilIDie) {
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
	
	printf("Closing model thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* viewFunc(void *arg) {
	double position = 0;
	int time = 0;
	int timeView = *((int*) arg);
	
	printf("\n\n\n");
	while(untilIDie) {
		position = takePosition();
		// printf("View position at time %d: %lf\n", time, position);
		// fflush(stdout);
		displayPosition(position);
		usleep(timeView * 10000);
		time += timeView;
	}
	
	printf("Closing view thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* controllerFunc(void *arg) {
	double position = 0;
	int time = 0;
	int timeController = *((int*) arg);
	
	remove("output.txt");
	
	while(untilIDie) {
		FILE *outputFile = fopen("output.txt", "a");
		if (outputFile == NULL) {
			printf("Can't open output.txt\n");
			exit(EXIT_FAILURE);
		}
		position = takePosition();
		fprintf(outputFile, "%d %lf \n", time, position);
		fclose(outputFile);
		// printf("Controller position at time %d: %lf\n", time, position);
		// fflush(stdout);
		usleep(timeController * 10000);
		time += timeController;
	}
	
	printf("Closing controller thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* killerFunc(void *arg) {
	
	int signalCaught;

    if (sigwait(&signalMask, &signalCaught)) {
		printf("Error in handling signal\n");
		exit(EXIT_FAILURE);
	}

	if (signalCaught == SIGINT) {
		untilIDie = false;
		forceSignalingInput();
	}
	
	printf("\nClosing killer thread\n");
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	
	if (argc != 5) {
		printf("Wrong command line input");
		exit(EXIT_FAILURE);
	}
	
	initialWall = strtol(argv[1], NULL, 10);
	finalWall = strtol(argv[2], NULL, 10);
	int timeController = strtol(argv[3], NULL, 10);
	int timeView = strtol(argv[4], NULL, 10);	
	
	initMonitorInput();
	initMonitorPosition(INITIAL_POSITION);
	
	pthread_t interface;
	pthread_t model;
	pthread_t view;
	pthread_t controller;
	pthread_t killer;
	
	sigemptyset (&signalMask);
    sigaddset (&signalMask, SIGINT);
	untilIDie = true;
	
	if (pthread_sigmask (SIG_BLOCK, &signalMask, NULL)) {
		printf("Error in creating sigmask\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&interface, NULL, (void *) interfaceFunc, (void *) NULL) != 0) {
		printf("Error in creating interface thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&model, NULL, (void *) modelFunc, (void *) NULL) != 0) {
		printf("Error in creating model thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&view, NULL, (void *) viewFunc, (void *) &timeView) != 0) {
		printf("Error in creating view thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&controller, NULL, (void *) controllerFunc, (void *) &timeController) != 0) {
		printf("Error in creating controller thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&killer, NULL, (void *) killerFunc, (void *) NULL) != 0) {
		printf("Error in creating killer thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_join(interface, NULL)) {
		printf("Error in joining interface thread");
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
	
	if (pthread_join(killer, NULL)) {
		printf("Error in joining killer thread");
		exit(EXIT_FAILURE);
	}
	
	closeMonitorInput();
	closeMonitorPosition();
	
	printf("EXIT SUCCESS\n");
	fflush(stdout);
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
