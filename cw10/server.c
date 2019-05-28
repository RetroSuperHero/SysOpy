#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <sys/epoll.h>
#define PORT 8080 

typedef struct clients {
    int index;
    int clientSockets[10];
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
        send(clientDescriptor, read_buffer, strlen(read_buffer), 0);
    }
    free(read_buffer);
}

int clientRegistered(int clientSocket) {
    for(int i=0; i<serverClients.index; ++i) {
        if(clientSocket == serverClients.clientSockets[i]) {
            return 1;
        }
    }
    return 0;
}

void* acceptClients() {
    while(1) {
        int newClientSocket;
        if((newClientSocket = accept(serverFileDescriptor, (struct sockaddr*) &serverAddress, (socklen_t*) &addrlen)) >= 0) {
            if(!clientRegistered(newClientSocket)) {
                serverClients.clientSockets[++serverClients.index] = newClientSocket;

                struct epoll_event monitorEvent;
                monitorEvent.events = EPOLLIN | EPOLLOUT;
                union epoll_data monitorData;
                monitorData.fd = newClientSocket;
                monitorEvent.data = monitorData;

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

int main(int argc, char const *argv[]) {
    int opt = 1;
    serverClients.index = 0;

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
    pthread_create(&acceptThread, NULL, acceptClients, NULL);
    pthread_create(&readThread, NULL, readClients, NULL);

    pthread_join(acceptThread, NULL);
    pthread_join(readThread, NULL);

    return 0;
}