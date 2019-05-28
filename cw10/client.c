#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>       
#include <unistd.h>
#define PORT 8080 
   
int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serverAddress;
    char message[1024];
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Tworzenie socketa nie powiodło się\n");
        exit(1);
    }

    memset(&serverAddress, '0', sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        fprintf(stderr, "Nieprawidłowy adres\n");
        exit(1);
    }

    if(connect(sock, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Połączenie nie powiodło się\n");
        exit(1);
    }

    while(1) {
        char* buffer = calloc(1024, sizeof(char));

        scanf("%s", message);
        send(sock, message, strlen(message), 0);
        printf("message message sent\n");
        read(sock, buffer, 1024);
        printf("%s\n", buffer);

        free(buffer);
    }

    return 0;
}