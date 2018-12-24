#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
// #include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "server.h"
#include "monitorInput.h"
#include "monitorRemoteInput.h"
#include "monitorPosition.h"

#define DISPLAY_WIDTH 80
#define INITIAL_POSITION 0
#define MAX_LINE_LENGTH 80

struct CommandLine {
    int initialWall;
    int finalWall;
    int timeController; 
    int timeView;
};

sigset_t signalMask;
bool extern untilIDie;
bool extern onlineMode;

void displayPosition(double position, int initialWall, int finalWall);

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
    
	// struct timeval start, end;
	// gettimeofday(&start, NULL);
    while (untilIDie) {
		if (fgets(line, sizeof(line), device_file) != NULL)
			sscanf(line, "%d %lf", &time, &change);
		else
			change = 0;		
		appendInput(change);
		// gettimeofday(&end, NULL);
		// printf("At time: %ld change of position: %f\n", (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec), change);
		// fflush(stdout);
		// gettimeofday(&start, NULL);
		usleep(100000);
    }

    fclose(device_file);

	printf("Closing interface thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* modelFunc(void *arg) {
	struct CommandLine *commandLine = (struct CommandLine *) arg;
	int initialWall = commandLine->initialWall;
	int finalWall = commandLine->finalWall;
	
	double change = 0;
	double position = INITIAL_POSITION;
	int i = 0;
	
	while(untilIDie) {			
		change = takeInput();
		if (onlineMode)
			change = takeRemoteInput();
		printf("The position %d has changed by: %lf. Online Mode: %d\n", i, change, onlineMode);
		fflush(stdout);
		
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
	struct CommandLine *commandLine = (struct CommandLine *) arg;
	int initialWall = commandLine->initialWall;
	int finalWall = commandLine->finalWall;
	int timeView = commandLine->timeView;
	double position = 0;
	int time = 0;
	
	printf("\n\n\n");
	while(untilIDie) {
		position = takePosition();
		// printf("View position at time %d: %lf\n", time, position);
		// fflush(stdout);
		displayPosition(position, initialWall, finalWall);
		usleep(timeView * 10000);
		time += timeView;
	}
	
	printf("Closing view thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}

void* controllerFunc(void *arg) {
	struct CommandLine *commandLine = (struct CommandLine *) arg;
	int timeController = commandLine->timeController;
	double position = 0;
	int time = 0;
	
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
		forceSignalingRemoteInput();
	}
	
	printf("\nClosing killer thread\n");
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	
	if (argc != 5) {
		printf("Wrong command line input: you have to type ./haptic par1 par2 par3 par4\n\
				- par1: int initial wall\n\
				- par2: int final wall\n\
				- par3: int controller refresh time\n\
				- par4: int view refresh time\n");
		exit(EXIT_FAILURE);
	}
	
	struct CommandLine commandLine = {strtol(argv[1], NULL, 10), 
			strtol(argv[2], NULL, 10), 
			strtol(argv[3], NULL, 10), 
			strtol(argv[4], NULL, 10)};
			
	onlineMode = false;
	
	initMonitorInput();
	initMonitorRemoteInput();
	initMonitorPosition(INITIAL_POSITION);
	
	pthread_t interface;
	pthread_t model;
	pthread_t view;
	pthread_t controller;
	pthread_t killer;
	pthread_t server;
	
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
	
	if (pthread_create(&model, NULL, (void *) modelFunc, (void *) &commandLine) != 0) {
		printf("Error in creating model thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&view, NULL, (void *) viewFunc, (void *) &commandLine) != 0) {
		printf("Error in creating view thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&controller, NULL, (void *) controllerFunc, (void *) &commandLine) != 0) {
		printf("Error in creating controller thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&killer, NULL, (void *) killerFunc, (void *) NULL) != 0) {
		printf("Error in creating killer thread");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&server, NULL, (void *) serverFunc, (void *) NULL) != 0) {
		printf("Error in creating server thread");
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
	
	if (pthread_join(server, NULL)) {
		printf("Error in joining server thread");
		exit(EXIT_FAILURE);
	}
	
	closeMonitorInput();
	closeMonitorRemoteInput();
	closeMonitorPosition();
	
	printf("EXIT SUCCESS\n");
	fflush(stdout);
	exit(EXIT_SUCCESS); 
}

void displayPosition(double position, int initialWall, int finalWall) {
	
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
