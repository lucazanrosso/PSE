#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* modelFunc(void *arg) {
	double *deviceInput = (double *) arg;
	printf("ciao\n");
	printf("ciao %f\n", deviceInput[1]);
	fflush(stdout);
}