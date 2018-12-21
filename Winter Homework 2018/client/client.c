#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define HOSTNAME "127.0.0.1"
#define PORT "3499"
#define MAX_LINE_LENGTH 80

int main(void) {
	
	int sockfd;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(HOSTNAME, PORT, &hints, &res) != 0) {
		perror("client: getaddrinfo\n");
		return 1;
	}

	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("client: socket\n");
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		perror("client: connect\n");
	}

	if (res == NULL) {
		perror("client: failed to connect\n");
		return 2;
	}

	FILE *device_file;
    char line[MAX_LINE_LENGTH]; 
    
    device_file = fopen("device.txt", "r"); 
    
    if (device_file == NULL) {
        printf("Can't open device.txt\n");
        exit(EXIT_FAILURE);
    }
    
    int time;
    double change;  
	
    while (1) {		
		if (fgets(line, sizeof(line), device_file) != NULL)
			sscanf(line, "%d %lf", &time, &change);
		else
			change = 0;		
		if (write(sockfd, &change, sizeof(change)) == -1) {
			perror("client: failed to send message\n");
		}
		usleep(100000);
    }

	freeaddrinfo(res);
    fclose(device_file);

	close(sockfd);

	return 0;
}
