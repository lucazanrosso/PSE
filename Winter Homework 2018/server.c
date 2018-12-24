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
#include "monitorPosition.h"

#define PORT "3510"
#define BACKLOG 5
#define MAXDATASIZE 100

bool extern untilIDie;
bool extern onlineMode;
 
void* serverFunc(void *arg) {
	
	int sockfd, new_fd, n;
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
	
	fd_set readfds;
	int rv = 0;
	struct timeval timeout;

	// printf("server: waiting for connections...\n");
	while(untilIDie) {
			
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;

		if ((rv = select(sockfd + 1, &readfds, NULL, NULL, &timeout)) == -1)
			perror("select");
		else if (rv == 0) {
			// printf("Waiting ready connection...");
		} else {
			printf("ciao3");
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1) {
				perror("accept");
			}

			double change = 0;
			bool firstRemoteInput = true;
			
			while(untilIDie) {
				if ((n = read(new_fd, &change, sizeof(change))) == -1)
					perror("Failed to read message");
				else if (n == 0) {
					onlineMode = false;
					break;
				} else {
					if (firstRemoteInput) {
						onlineMode = true;
						appendPosition(change);
						firstRemoteInput = false;
					} else {
						appendRemoteInput(change);
					}		
				}
				// printf("client: received %d %ld %f\n", n, sizeof(change), change);
			}
			close(new_fd);
		}
	}		

	close(sockfd);

	printf("Closing server thread\n");
	fflush(stdout);
	pthread_exit(NULL);
}
