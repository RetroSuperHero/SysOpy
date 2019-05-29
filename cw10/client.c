#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>       
#include <unistd.h>
#include <string.h>
#define PORT 8080 

int working = 0;

int stringToInt(char* string) {
    int result = 0;
    int len = strlen(string);
    for(int i=0; i<len; i++) {
        result = result*10 + (string[i] - '0');
    }
    return result;
}

void calculate(char* text, int sock) {
    int words = 0;
    strtok(text, " \n\r");
    while(text) {
        words++;
        text = strtok(NULL, " \n\r");
    }
    char* count = calloc(100, sizeof(char));
    sprintf(count, "%d", words);
    send(sock, count, 100, 0);
    free(count);
    working = 0;
}
   
int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serverAddress;
    char message[1024];

    if(argc != 2) {
        fprintf(stderr, "Nieprawidłowa liczba argumentów\n");
        exit(1);
    }
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    send(sock, argv[1], strlen(argv[1]), 0);
    char* buffer = calloc(20, sizeof(char));
    read(sock, buffer, 20);
    printf("%s\n", buffer);
    if(!strcmp(buffer, "Już zarejestrowany")) {
        exit(1);
    }
    free(buffer);

    while(1) {
        char* message = calloc(100, sizeof(char));
        read(sock, message, 100);
        if(!strcmp(message, "PING")) {
            send(sock, "PING", 4, 0);
        } else if(!working) {
            int textLength = stringToInt(message);
            char* buffer = calloc(textLength, sizeof(char));
            read(sock, buffer, textLength);
            working = 1;
            calculate(buffer, sock);
            free(buffer);
        }
        free(message);
    }

    return 0;
}