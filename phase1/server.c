//server.c 21011723 조경수

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 2  // 최대 클라이언트 수를 2로 지정
#define BUFFER_SIZE 256  // 버퍼 크기를 256으로 지정
#define PORT_NO 2222  // 포트 번호를 2222로 지정

typedef struct {  // Client 구조체 정의
    int sockfd;
    char name[256];
} Client;

Client clients[10];  // 최대 10개의 Client를 저장하는 배열
int num_clients = 0;  // 현재 연결된 클라이언트 수를 저장

void add_client(int sockfd, char* name) {  // 새 클라이언트 추가 함수
    clients[num_clients].sockfd = sockfd;  // 소켓 파일 디스크립터 저장
    strcpy(clients[num_clients].name, name);  // 이름 복사
    num_clients++;  // 클라이언트 수 증가
}

void print_clients() {  // 현재 연결된 클라이언트의 정보 출력
    printf("Current clients:");
    for (int i = 0; i < num_clients; i++) {
        printf("client :: %s", clients[i].name);
    }
}

void error(const char *msg) {  // 오류 메시지 출력 후 프로그램 종료 함수
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;  // 소켓 파일 디스크립터
    socklen_t clilen;  // 클라이언트 주소의 길이를 저장하는 변수
    char name_buf[BUFFER_SIZE], buffer[BUFFER_SIZE];  // 이름과 메시지를 저장하는 버퍼
    struct sockaddr_in serv_addr, cli_addr;  // 서버와 클라이언트 주소를 저장하는 구조체
    int n;  // 반환값을 저장하는 변수

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 소켓 생성
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));  // 서버 주소 초기화
    serv_addr.sin_family = AF_INET;  // 주소 체계 설정
    serv_addr.sin_addr.s_addr = INADDR_ANY;  // 모든 인터페이스에서 연결 허용
    serv_addr.sin_port = htons(PORT_NO);  // 포트 설정

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  // 소켓에 주소 바인딩
        error("ERROR on binding");

    listen(sockfd, MAX_CLIENTS);  // 클라이언트 접속 대기
    clilen = sizeof(cli_addr);  // 클라이언트 주소의 길이 설정

    while (1) {  // 클라이언트 접속을 계속 받음
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);  // 클라이언트 접속 수락
        if (newsockfd < 0) 
            error("ERROR on accept"); //에러 발생 메시지

        bzero(buffer, BUFFER_SIZE);  // 버퍼 초기화
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);  // 클라이언트로부터 메시지 받음
        if (n < 0) 
            error("ERROR reading from socket"); //에러 발생 메시지

        int num1, num2, result;
        char name[100];
        sscanf(buffer, "%s %d %d", name,  &num1, &num2);  // 메시지에서 이름, 두 숫자를 가져옴
        
        add_client(newsockfd, name);  // 새 클라이언트 추가
            
        if(strcmp(clients[num_clients-1].name, "./client_add") == 0){  // 클라이언트 이름이 "./client_add"이면
            result = num1 + num2;  // 두 숫자를 더함
				printf("First input is : %d\n", num1);
				printf("Second input is : %d\n", num2);
				printf("Output is : %d\n", result);
        }

        if(strcmp(clients[num_clients-1].name, "./client_mul") == 0){  // 클라이언트 이름이 "./client_mul"이면
            result = num1 * num2;  // 두 숫자를 곱함
        }

        sprintf(buffer, "%d", result);  // 결과를 버퍼에 저장
        n = write(newsockfd, buffer, strlen(buffer));  // 결과를 클라이언트에 전송
        if(n < 0)
            error("ERROR writing to socket");

        close(newsockfd);  // 클라이언트와의 연결 종료
    }

    close(sockfd);  // 서버 소켓 닫기
    return 0; 
}
