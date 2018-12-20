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

int main(int argc, char *argv[]) {
	
	int sock, numbytes;
	struct addrinfo hints, *res;
	
	// argv

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(HOSTNAME, PORT, &hints, &res) != 0) {
		// printf
		return 1;
	}

	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("client: socket");
	}

	if (connect(sock, res->ai_addr, res->ai_addrlen) == -1) {
		close(sock);
		perror("client: connect");
	}

	if (res == NULL) {
		// fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	freeaddrinfo(res); // all done with this structure

	double change;
	if ((numbytes = recv(sock, &change, sizeof(change), 0)) == -1) {
	    perror("client: failed to send message\n");
	    exit(1);
	}
	
	printf("client: received '%f'\n", change);	

	close(sock);

	return 0;
}
