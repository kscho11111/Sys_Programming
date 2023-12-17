#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define MAX_CLIENTS 2    // 최대 클라이언트 수를 2로 지정
#define BUFFER_SIZE 256  // 버퍼 크기를 256으로 지정
#define PORT_NO 2222     // 포트 번호를 2222로 지정
#define BUF_SIZE 1024
#define SOCK_PATH "unix_sock"

typedef struct {  // Client 구조체 정의
    int sockfd;
    char name[256];
} Client;

Client clients[10];  // 최대 10개의 Client를 저장하는 배열
int num_clients = 0;  // 현재 연결된 클라이언트 수를 저장

typedef struct {
    int a;
    int b;
} numbers;

void add_client(int sockfd, char* name) {  // 새 클라이언트 추가 함수
    clients[num_clients].sockfd = sockfd;  // 소켓 파일 디스크립터 저장
    strcpy(clients[num_clients].name, name);  // 이름 복사
    num_clients++;  // 클라이언트 수 증가
}

void* add_thread(void* arg) {
    numbers* nums = (numbers*)arg;    // 구조체 선언
    int result = nums->a + nums->b;   // result에 결과값 저장

	 printf("First input is : %d\n", nums->a);  // 입력 값 및 result 값 출력
	 printf("Second input is : %d\n", nums->b);
	 printf("Result!: %d\n", result);
 
	 return (void*)(intptr_t)result;             // result값 반환
}

void* mul_thread(void* arg){
	numbers* nums = (numbers*)arg;             // 구조체 선언
	int result = nums->a * nums->b;		   // result에 결과값 저장

	printf("First input is : %d\n", nums->a);  // 입력 값 및 result 값 출력
	printf("Second input is : %d\n", nums->b);
	printf("Result: %d\n", result);

	return (void*)(intptr_t)result;             //result 값 반환
}

void error(const char *msg) {  // 오류 메시지 출력 후 프로그램 종료 함수
    perror(msg);
    exit(1);
}

typedef struct {        //client의 정보를 저장하는 구조체 선언
    char id[BUF_SIZE];
    char pw[BUF_SIZE];
    int x;
    int y;
} ClientInfo;

void *login_handler(void *arg) {                // login의 여부를 정하는 함수
    int result;
    ClientInfo *info = (ClientInfo *)arg;       //client의 정보를 저장한 구조체 선언

    printf("ID: %s, PW: %s\n", info->id, info->pw); // id와 pw를 출력

    if(strcmp(info->id, "kscho1") == 0 && strcmp(info->pw, "010216") == 0){ //id와 pw가 일치하면 0 저장
        printf("login completed!\n");                               // 서버 화면에 login completed 출력
        result = 0;
    } else {                                         // 불일치 하면 login failed 출력
        printf("login failed!\n");
        result = 1;
    }
    return (void*)(intptr_t)result;
}

void *calc_handler(void *arg) {                      // 계산을 해주는 함수
    ClientInfo *info = (ClientInfo *)arg;
    printf("x: %d, y: %d\n", info->x, info->y);      // 입력받은 인자 출력
    int result = pow(info->x, info->y);
    printf("Result of calculation is: %d\n", result); // 결과값 저장 및 출력

	 return (void*)(intptr_t)result;
}

void *internet_socket_server(void *arg) {
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
        numbers nums = {0,0};
		  nums.a = num1;
		  nums.b = num2;

        add_client(newsockfd, name);  // 새 클라이언트 추가
            
        if(strcmp(clients[num_clients-1].name, "./client_add") == 0){  // 클라이언트 이름이 "./client_add"이면
				pthread_t thread_add;

				if (pthread_create(&thread_add, NULL, add_thread, &nums) != 0) {
					perror("Failed to create thread");
					return -1;
				}

				void* status;
				pthread_join(thread_add, &status);  // 스레드가 끝날 때까지 기다린 후, 반환값을 status에 저장

				int result = (int)(intptr_t)status;
				sprintf(buffer, "%d", result);
				buffer[strlen(buffer)] = NULL;
				n = write(newsockfd, buffer, strlen(buffer)+1);
		  }

        if(strcmp(clients[num_clients-1].name, "./client_mul") == 0){  // 클라이언트 이름이 "./client_mul"이면
            pthread_t thread_mul;

				if (pthread_create(&thread_mul, NULL, mul_thread, &nums) != 0) {
					perror("Failed to create thread");
					return -1;
				}

				void* status;
				pthread_join(thread_mul, &status);  // 스레드가 끝날 때까지 기다린 후, 반환값을 status에 저장

				int result = (int)(intptr_t)status;  // 반환값을 int 타입으로 변환
				//printf("Result: %d\n", result);
				
				sprintf(buffer, "%d", result);
				buffer[strlen(buffer)] = NULL;
				n = write(newsockfd, buffer, strlen(buffer)+1);
		  }

        if(n < 0)
            error("ERROR writing to socket");

        close(newsockfd);  // 클라이언트와의 연결 종료
    }

    close(sockfd);  // 서버 소켓 닫기
    return 0;
}

void *domain_socket_server(void *arg) {
    int serv_sock, clnt_sock;			// 파일 디스크립터를 저장할 변수 선언
    struct sockaddr_un serv_adr, clnt_adr;	// 주소를 저장할 구조체
    socklen_t clnt_adr_sz;			//구조체의 크기를 저장
    char buf[100];

    ClientInfo client_info;			// 구조체 선언
    serv_sock = socket(AF_UNIX, SOCK_STREAM, 0); 	// UNIX 도메인 스트림 소켓 생성
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sun_family = AF_UNIX;
    strcpy(serv_adr.sun_path, SOCK_PATH);

    bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)); // 소켓 바인딩
    listen(serv_sock, 5);			// 소켓을 리스닝 상태로 바꾸기

	 while(1){

    	 clnt_adr_sz = sizeof(clnt_adr);
    	 clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    	 read(clnt_sock, &client_info, sizeof(ClientInfo)); // 클라이언트로부터 정보 가져오기

	 	 pthread_t login_thread, calc_thread; 		// thread 생성
    	 pthread_create(&login_thread, NULL, login_handler, (void *)&client_info); // 함수 실행 
    	 pthread_create(&calc_thread, NULL, calc_handler, (void *)&client_info);

	 void* thread_return;		//반환값을 저장할 변수 선언
    	 int login_result, calc_result;

    	 pthread_join(login_thread, &thread_return);	// 반환 값 받고 int형으로 바꾸기
    	 login_result = (int)(intptr_t)thread_return;

	 pthread_join(calc_thread, &thread_return);	//반환 값 받고 int 형으로 바꾸기
    	 calc_result = (int)(intptr_t)thread_return;

	  	 char msg1[50], msg2[50];

	  	 if(!login_result)	//로그인 성공 시 login successed 저장
			 strcpy(msg1, "login successed!\n");
		 else			// 로그인 실패 시 login failed 저장
			 strcpy(msg1, "login failed...\n");

		 sprintf(msg2, "Result of calculation is %d", calc_result); //계산의 결과 저장

		 char *msg = malloc(strlen(msg1) + strlen(msg2) + 3); // 문자열을 저장할 변수 선언

		 sprintf(msg, "%s%s", msg1, msg2);	// 두 문자열 합치기

	 	write(clnt_sock, msg, strlen(msg)+3); 	// 클라이언트로 보내기

	    close(clnt_sock);		//클라이언트 소켓 닫기
	 }

	 close(serv_sock);		// 서버 소켓 닫기
	 unlink(SOCK_PATH);

    return 0;
}

int main(){
    pthread_t internet_thread, domain_thread; // 2개의 thread 생성
					// 각각 internet socket과 domain socket 을 실행
    pthread_create(&internet_thread, NULL, internet_socket_server, NULL);
    pthread_create(&domain_thread, NULL, domain_socket_server, NULL);

    pthread_join(internet_thread, NULL);	// 소켓 닫기
    pthread_join(domain_thread, NULL);

	return 0;
}

