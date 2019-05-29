#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <sys/epoll.h>
#include <string.h>
#define PORT 8080 

typedef struct clients {
    int index;
    int working[10];
    char** clientsNames;
    struct epoll_event monitorsEvents[10];
} clients;

clients serverClients;
int serverFileDescriptor;
struct sockaddr_in serverAddress;
int addrlen = sizeof(serverAddress);
int monitorDescriptor;

void readClient(int clientDescriptor) {
    char* read_buffer = calloc(100, sizeof(char));
    int bytes_read = recv(clientDescriptor, read_buffer, 100, MSG_DONTWAIT);
    if(bytes_read != -1) {
        printf("Read '%s'\n", read_buffer);
        for(int i=0; i<serverClients.index; ++i) {
            if(serverClients.monitorsEvents[i].data.fd == clientDescriptor) {
                serverClients.working[i] = 0;
            }
        }
        //send(clientDescriptor, read_buffer, strlen(read_buffer), 0);
    }
    free(read_buffer);
}

int clientRegistered(char* clientName) {
    for(int i=0; i<serverClients.index; ++i) {
        if(!strcmp(clientName, serverClients.clientsNames[i])) {
            return 1;
        }
    }
    return 0;
}

void* acceptClients() {
    while(1) {
        int newClientSocket;
        if((newClientSocket = accept(serverFileDescriptor, (struct sockaddr*) &serverAddress, (socklen_t*) &addrlen)) >= 0) {
            char* newClientName = calloc(10, sizeof(char));
            read(newClientSocket, newClientName, 10);

            if(!clientRegistered(newClientName)) {
                serverClients.clientsNames[serverClients.index] = newClientName;
                serverClients.working[serverClients.index++] = 0;

                struct epoll_event monitorEvent;
                monitorEvent.events = EPOLLIN | EPOLLOUT;
                union epoll_data monitorData;
                monitorData.fd = newClientSocket;
                monitorEvent.data = monitorData;
                send(newClientSocket, "Zarejestrowano", 20, 0);

                epoll_ctl(monitorDescriptor, EPOLL_CTL_ADD , newClientSocket, &monitorEvent);  
            } else {
                send(newClientSocket, "Już zarejestrowany", 20, 0);
            }
        }
    }
}

void* readClients() {
    while(1) {
        epoll_wait(monitorDescriptor, serverClients.monitorsEvents, 10, -1);
        for(int i = 0; i<serverClients.index; i++) {
            readClient(serverClients.monitorsEvents[i].data.fd);
        }
    }
}

void* readTerminal() {
    while(1) {
        char* fileName = calloc(1024, sizeof(char));
        scanf("%s", fileName);
        FILE* textFile = fopen(fileName, "r");

        fseek(textFile, 0L, SEEK_END);
        int fileLength = ftell(textFile);
        fseek(textFile, 0L, SEEK_SET);

        char* text = calloc(fileLength, sizeof(char));
        char* line;
        size_t len = 0;

        while(getline(&line, &len, textFile) != -1) {
            strcat(text, line);
        }

        serverClients.working[0] = 1;
        char* fileLengthString = calloc(100, sizeof(char));
        sprintf(fileLengthString, "%d", fileLength);
        send(serverClients.monitorsEvents[0].data.fd, fileLengthString, 100, 0);
        send(serverClients.monitorsEvents[0].data.fd, text, fileLength, 0);
        free(fileLengthString);
        free(text);
    }
}

int main(int argc, char const *argv[]) {
    int opt = 1;
    serverClients.index = 0;
    serverClients.clientsNames = calloc(10, sizeof(char*));

    if(!(serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0))) {
        fprintf(stderr, "Tworzenie socketa nie powiodło się\n");
        exit(1);
    }

    if(setsockopt(serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        fprintf(stderr, "Ustawienie opcji socketa nie powiodło się\n");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if(bind(serverFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Bindowanie socketa nie powiodło się\n");
        exit(1);
    }

    if(listen(serverFileDescriptor, 10) < 0) {
        fprintf(stderr, "Nasłuchiwanie socketa nie powiodło się\n");
        exit(1);
    }

    monitorDescriptor = epoll_create1(0);

    pthread_t acceptThread;
    pthread_t readThread;
    pthread_t terminalThread;
    pthread_create(&acceptThread, NULL, acceptClients, NULL);
    pthread_create(&readThread, NULL, readClients, NULL);
    pthread_create(&terminalThread, NULL, readTerminal, NULL);

    pthread_join(acceptThread, NULL);
    pthread_join(readThread, NULL);
    pthread_join(terminalThread, NULL);

    return 0;
}