#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#include "monitorRemoteInput.h"

#define PORT "3499"
#define BACKLOG 5
#define MAXDATASIZE 100

bool extern untilIDie;
 
void* serverFunc(void *arg) {
	
	int sockfd, n;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
		perror("server: getaddrinfo\n");
		pthread_exit(NULL);
	}
	
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("server: socket");
	}
	
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
	}

	if (res == NULL)  {
		perror("server: failed to bind\n");
		pthread_exit(NULL);
	}

	freeaddrinfo(res);

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		pthread_exit(NULL);
	}

	// printf("server: waiting for connections...\n");
	
		sin_size = sizeof their_addr;
	sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (sockfd == -1) {
		perror("accept");
	}

	double change = 0;
	
	while(untilIDie) {
		if ((n = read(sockfd, &change, sizeof(change))) == -1)
			perror("send");
		if (n == 0)
			break;
		appendRemoteInput(change);
		// printf("client: received %d %ld %f\n", n, sizeof(change), change);
	}			

	close(sockfd);

	printf("Closing server thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}
