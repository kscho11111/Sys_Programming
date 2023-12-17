#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define SOCK_PATH "unix_sock"

int main()
{
    int serv_sock, clnt_sock;
    struct sockaddr_un serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    char buf[100];

    serv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sun_family = AF_UNIX;
    strcpy(serv_adr.sun_path, SOCK_PATH);

    bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    read(clnt_sock, buf, sizeof(buf));
    printf("Message from client: %s\n", buf);

	 char serv_msg[] = "Hello from server!";
	 write(clnt_sock, serv_msg, sizeof(serv_msg));

    close(clnt_sock);
    close(serv_sock);
    unlink(SOCK_PATH);

    return 0;
}

