#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    // 서버 정보 설정
    const char *server_ip = "8.8.8.8";  // 대상 서버 IP 주소
    int server_port = 80;               // 대상 서버 포트 번호

    // 소켓 생성
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    // 데이터 전송
    const char *message = "Hello, TCP Server!";
    send(client_socket, message, strlen(message), 0);
    printf("Data sent to server: %s\n", message);

    // 연결 종료
    close(client_socket);

    return 0;
}
