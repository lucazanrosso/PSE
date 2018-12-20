#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define HOSTNAME "127.0.0.1"
#define PORT "3490"

int main(void) {
	
	int sockfd;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(HOSTNAME, PORT, &hints, &res) != 0) {
		// printf
		return 1;
	}

	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("client: socket");
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("client: connect");
	}

	if (res == NULL) {
		// fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	freeaddrinfo(res); // all done with this structure

	double change = 16.0;
	if (write(sockfd, &change, sizeof(change)) == -1) {
	    perror("client: failed to send message\n");
	    exit(1);
	}
	
	// printf("client: received '%f'\n", change);	

	close(sockfd);

	return 0;
}
