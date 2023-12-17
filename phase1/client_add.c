// client_add.c 21011723 조경수

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 256  // 버퍼 크기를 256으로 정의
#define PORT_NO 2222  // 포트 번호를 2222로 정의

char name[100];  // 사용자 이름을 저장하는 문자열 배열

void error(const char *msg) {  // 오류 메시지 출력 후 프로그램 종료
    perror(msg);
    exit(0);
}

void *send_and_receive(void *arg) {  // 서버로부터 메시지를 보내고 받는 함수
    int sockfd = *((int *)arg);  // 소켓 파일 디스크립터
    char buffer[BUFFER_SIZE];  // 메시지를 저장하는 버퍼
    int num1, num2, n;  // 두 개의 숫자와 반환값을 저장하는 변수

    printf("Enter first number: ");  // 첫 번째 숫자를 입력 받음
    scanf("%d", &num1);

    printf("Enter second number: ");  // 두 번째 숫자를 입력 받음
    scanf("%d", &num2);

    sprintf(buffer, "%s %d %d", name, num1, num2);  // 버퍼에 이름, 첫 번째 숫자, 두 번째 숫자를 저장
    n = write(sockfd, buffer, strlen(buffer));  // 버퍼의 내용을 서버로 전송
    if (n < 0) 
        error("ERROR writing to socket");

    bzero(buffer, BUFFER_SIZE);  // 버퍼 초기화
    n = read(sockfd, buffer, BUFFER_SIZE - 1);  // 서버로부터 메시지를 받음
    if (n < 0) 
        error("ERROR reading from socket");

    printf("Result: %s %d\n", buffer, strlen(buffer));  // 결과 출력
    return NULL;
}

int main(int argc, char *argv[]) {
    strcpy(name, argv[0]);  // 이름을 argv[0]으로 설정
    int sockfd, n;
    struct sockaddr_in serv_addr;  // 서버 주소를 저장하는 구조체
    struct hostent *server;  // 호스트 정보를 저장하는 구조체

    if (argc < 2) {  // 인자가 2개 미만이면 에러 메시지 출력 후 종료
        fprintf(stderr,"usage %s hostname", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 소켓 생성
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);  // 호스트 이름으로 호스트 정보를 가져옴
    if (server == NULL) {  // 호스트 정보가 없으면 에러 메시지 출력 후 종료
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));  // 서버 주소 초기화
    serv_addr.sin_family = AF_INET;  // 주소 체계 설정
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);  // 주소 복사
    serv_addr.sin_port = htons(PORT_NO);  // 포트 설정

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  // 서버에 연결
        error("ERROR connecting");

	 send_and_receive(&sockfd);

    close(sockfd);  // 소켓 닫기
    return 0;
}
