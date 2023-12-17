// clinet.c 21011723 조경수

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define SOCK_PATH "unix_sock"

typedef struct {		// 클라이언트 정보를 저장하는 구조체
    char id[BUF_SIZE];
    char pw[BUF_SIZE];
    int x;
    int y;
} ClientInfo;

int main()
{
    int sock;			// 파일 디스크립터를 저장할 변수
    struct sockaddr_un serv_adr;	//서버의 주소 정보를 저장
    
    ClientInfo client_info;	// 클라이언트 정보를 저장하는 구조체 선언
    sock = socket(AF_UNIX, SOCK_STREAM, 0);	//UNIX 도메인 스트림 소켓 생성

    memset(&serv_adr, 0, sizeof(serv_adr)); // 서버 주소 구조체 초기화
    serv_adr.sun_family = AF_UNIX; 		// 주소 체계를 설정
    strcpy(serv_adr.sun_path, SOCK_PATH);	// 소켓 파일의 경로 설정

    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));	//서버 연결
    	
    printf("Enter ID: ");			// 사용자에게 정보 입력받기
    fgets(client_info.id, BUF_SIZE, stdin);
    client_info.id[strcspn(client_info.id, "\n")] = 0; // Remove trailing newline

    printf("Enter PW: ");
    fgets(client_info.pw, BUF_SIZE, stdin);
    client_info.pw[strcspn(client_info.pw, "\n")] = 0; // Remove trailing newline

    printf("Enter x: ");
    scanf("%d", &client_info.x);

    printf("Enter y: ");
    scanf("%d", &client_info.y);

    write(sock, &client_info, sizeof(ClientInfo)); 	//서버에 클라이언트 정보 전달

    char *msg = malloc(100);				// 메시지를 저장할 메모리 할당
    read(sock, msg, 100);				// 서버로부터 메시지 읽어옴
	 
    printf("%s\n", msg);				// 메시지 출력
	 
    close(sock);					// 소켓 닫기

    return 0;
}

