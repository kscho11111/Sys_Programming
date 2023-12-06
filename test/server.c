// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 256
#define PORT_NO 2222

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NO);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, MAX_CLIENTS);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        bzero(buffer, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) 
            error("ERROR reading from socket");

        int num1, num2, result;
        sscanf(buffer, "%d %d", &num1, &num2);

        // The first client adds the numbers, the second one multiplies them.
        static int client_count = 0;
        if (client_count == 0) {
            result = num1 + num2;
        } else {
            result = num1 * num2;
        }
        client_count = (client_count + 1) % MAX_CLIENTS;

        sprintf(buffer, "%d", result);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        close(newsockfd);
    }

    close(sockfd);
    return 0; 
}
