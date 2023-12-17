#include <stdio.h>
#include <unistd.h>
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

void *login_handler(void *arg) {
    int result;
	 char* serv_msg = malloc(sizeof(char) * 50);
	 ClientInfo *info = (ClientInfo *)arg;

	 printf("ID: %s, PW: %s\n", info->id, info->pw);
	 
	 if(info->id == "kscho1" && info->pw == "010216"){
	 	strcpy(serv_msg,"login completed!");
		printf("login completed!\n");
		result = 0;
	 }
	 
	 else{
		 strcpy(serv_msg,"login failed!");
		 printf("login failed!\n");
		 result = 1;
	 }

	 //printf("%s\n", serv_msg);

    //write(clnt_sock, serv_msg, sizeof(serv_msg));

	 return (void*)(intptr_t)result;
}

void *calc_handler(void *arg) {
    char* serv_msg = malloc(sizeof(char) * 50);
	 ClientInfo *info = (ClientInfo *)arg;
	 
	 printf("x: %d, y: %d\n", info->x, info->y); 

    int result = pow(info->x, info->y);
	 sprintf(serv_msg, "Result of calculation is: %d", result);
    printf("Result of calculationis: %d\n", result);

	 //write(clnt_sock, serv_msg, sizeof(serv_msg));

    return (void*)(intptr_t)result;
}

int main()
{
    int serv_sock, clnt_sock;
    struct sockaddr_un serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    char buf[100];

	 ClientInfo client_info;
    serv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sun_family = AF_UNIX;
    strcpy(serv_adr.sun_path, SOCK_PATH);

    bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    read(clnt_sock, &client_info, sizeof(ClientInfo));

	 pthread_t login_thread, calc_thread;
    pthread_create(&login_thread, NULL, login_handler, (void *)&client_info);
    pthread_create(&calc_thread, NULL, calc_handler, (void *)&client_info);

	 void* msg1;
	 void* msg2;

    pthread_join(login_thread, &msg1);
    pthread_join(calc_thread, &msg2);

	 int res1 = (int)(intptr_t)msg1;
	 int res2 = (int)(intptr_t)msg2;

	 sprintf(msg1, "login : %d", res1);
	 sprintf(msg2, "Result of calculation is %d", res2);

	 //msg1[strlen(msg1)] = NULL;
	 //msg2[strlen(msg2)] = NULL;

	 //sprintf(buffer, "%d", result);
	 //buffer[strlen(buffer)] = NULL;

	 //write(clnt_sock, buffer, strlen(buffer)+1);
	 //printf("%d %d\n", sizeof(msg1), sizeof(msg2));

	 write(clnt_sock, msg1, sizeof(msg1));
	 write(clnt_sock, msg2, sizeof(msg2));

	 //char serv_msg[] = "Hello from server!";
	 //write(clnt_sock, serv_msg, sizeof(serv_msg));

    close(clnt_sock);
    close(serv_sock);
    unlink(SOCK_PATH);

    return 0;
}
