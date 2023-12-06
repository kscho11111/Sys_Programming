//client_add.c 21011723 chokyungsoo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 256
#define PORT_NO 2222

char name[100];

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void *send_and_receive(void *arg) {
    int sockfd = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int num1, num2, n;

    //printf("Enter client name: ");
	 //scanf("%s", name);

	 printf("Enter first number: ");
	 scanf("%d", &num1);

	 printf("Enter second number: ");
	 scanf("%d", &num2);

	 sprintf(buffer, "%s %d %d", name, num1, num2);
	 n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");

    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) 
        error("ERROR reading from socket");

    printf("Result: %s\n", buffer);
    return NULL;
}

int main(int argc, char *argv[]) {
	 strcpy(name, argv[0]);
	 int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 2) {
        fprintf(stderr,"usage %s hostname", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(PORT_NO);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    pthread_t thread1, thread2;

	 //if(strcmp(argv[0], "./client_add") == 0) printf("correct!");
	 
	 if(strcmp(argv[0], "./client_add") == 0)
	 {
		 pthread_create(&thread1, NULL, send_and_receive, &sockfd);
		 pthread_join(thread1, NULL);
	 }

	 else{
		 pthread_create(&thread2, NULL, send_and_receive, &sockfd);
		 pthread_join(thread2, NULL);
	 }


	 close(sockfd);
    return 0;
}
