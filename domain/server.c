#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <math.h>

#define BUF_SIZE 1024
#define SOCK_PATH "unix_sock"

typedef struct {
    char id[BUF_SIZE];
    char pw[BUF_SIZE];
    int x;
    int y;
} ClientInfo;

void *login_handler(void *arg) {
    ClientInfo *info = (ClientInfo *)arg;
    printf("ID: %s, PW: %s\n", info->id, info->pw);
    printf("Login result: Accepted\n");
    return NULL;
}

void *calc_handler(void *arg) {
    ClientInfo *info = (ClientInfo *)arg;
    double result = pow(info->x, info->y);
    printf("Result of %d^%d: %.2f\n", info->x, info->y, result);
    return NULL;
}

int main()
{
    int serv_sock, clnt_sock;
    struct sockaddr_un serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
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

    pthread_join(login_thread, NULL);
    pthread_join(calc_thread, NULL);

    close(clnt_sock);
    close(serv_sock);
    unlink(SOCK_PATH);

    return 0;
}
