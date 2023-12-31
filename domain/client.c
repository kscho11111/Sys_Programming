#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define SOCK_PATH "unix_sock"

typedef struct {
    char id[BUF_SIZE];
    char pw[BUF_SIZE];
    int x;
    int y;
} ClientInfo;

int main()
{
    int sock;
    struct sockaddr_un serv_adr;
    ClientInfo client_info;
    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sun_family = AF_UNIX;
    strcpy(serv_adr.sun_path, SOCK_PATH);

    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    	
	 printf("Enter ID: ");
    fgets(client_info.id, BUF_SIZE, stdin);
    client_info.id[strcspn(client_info.id, "\n")] = 0; // Remove trailing newline

    printf("Enter PW: ");
    fgets(client_info.pw, BUF_SIZE, stdin);
    client_info.pw[strcspn(client_info.pw, "\n")] = 0; // Remove trailing newline

    printf("Enter x: ");
    scanf("%d", &client_info.x);

    printf("Enter y: ");
    scanf("%d", &client_info.y);

	 write(sock, &client_info, sizeof(ClientInfo));

	 char *msg = malloc(100);
	 read(sock, msg, 100);
	 
	 printf("%s\n", msg);
	 
    close(sock);

    return 0;
}
