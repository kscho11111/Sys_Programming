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
    ClientInfo *info = (ClientInfo *)arg;
    
    printf("ID: %s, PW: %s\n", info->id, info->pw);
    
    if(strcmp(info->id, "kscho1") == 0 && strcmp(info->pw, "010216") == 0){
        printf("login completed!\n");
        result = 0;
    } else {
        printf("login failed!\n");
        result = 1;
    }
    return (void*)(intptr_t)result;
}

void *calc_handler(void *arg) {
    ClientInfo *info = (ClientInfo *)arg;
    printf("x: %d, y: %d\n", info->x, info->y); 
    int result = pow(info->x, info->y);
    printf("Result of calculation is: %d\n", result);
    
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

	 void* thread_return;
    int login_result, calc_result;

    pthread_join(login_thread, &thread_return);
    login_result = (int)(intptr_t)thread_return;
    
	 pthread_join(calc_thread, &thread_return);
    calc_result = (int)(intptr_t)thread_return;

	 char msg1[50], msg2[50];
    
	 if(!login_result)
		 strcpy(msg1, "login successed!\n");
	 else
		 strcpy(msg1, "login failed...\n");
    
	 sprintf(msg2, "Result of calculation is %d", calc_result);

	 char *msg = malloc(strlen(msg1) + strlen(msg2) + 3);

	 sprintf(msg, "%s%s", msg1, msg2);

	 write(clnt_sock, msg, strlen(msg)+3);
    
    close(clnt_sock);
    close(serv_sock);
    unlink(SOCK_PATH);

    return 0;
}
