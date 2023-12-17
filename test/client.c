#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define SOCK_PATH "unix_sock"

int main()
{
    int sock;
    struct sockaddr_un serv_adr;
    char buf[100] = "Hello, UNIX Domain Socket!";

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sun_family = AF_UNIX;
    strcpy(serv_adr.sun_path, SOCK_PATH);

    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    write(sock, buf, sizeof(buf));

	 read(sock, buf, sizeof(buf));
	 printf("Message from server: %s", buf);

    close(sock);

    return 0;
}

