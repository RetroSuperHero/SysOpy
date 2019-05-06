#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include "chat.h"

pid_t PID;
int thisClient;
mqd_t serverQueue;
mqd_t thisClientQueue;

Message receiveMessage(int queue) {
    Message message;
    if(mq_receive(queue, (char *)&message, MESSAGE_SIZE, NULL)) {
        return message;
    } else {
        fprintf(stderr, "Wystąpił problem z odbieraniem wiadomości z kolejki serwera");
        exit(1);
    }
}

void sendMessageToClient(int queue, long messageType, char* messageText, long clientID, int textLength) {
    Message message;
    message.type = messageType;
    message.clientID = clientID;
    message.textLength = textLength;

    if(textLength <= MAX_MESSAGE_LENGTH && textLength >= 0) {
        memcpy(message.text, messageText, textLength);
    } else {
        fprintf(stderr, "Wysyłana do klienta wiadomość ma nieprawidłową długość");
        exit(1);
    }

    printf("Wysyłanie wiadomości: \n  Typ: %s \n  Klient: %ld\n  Treść: %s\n", getTypeName(message.type), message.clientID, message.text);

    if(mq_send(queue, (char*) &message, MESSAGE_SIZE, 1) == -1){
        fprintf(stderr, "Wysyłanie wiadomości do klienta nie powiodło się");
        exit(1);
    }
}

void manageInputCommand(char* command, size_t commandLength) {
    if(!strncmp(command, "ECHO", 4)) {
        sendMessageToClient(serverQueue, ECHO, command + 5, thisClient, commandLength - 5);
    } else if(!strncmp(command, "2ALL", 4)) {
        sendMessageToClient(serverQueue, TO_ALL, command + 5, thisClient, commandLength - 5);
    } else if(!strncmp(command, "FRIENDS", 7)) {
        sendMessageToClient(serverQueue, FRIENDS, command + 8, thisClient, commandLength - 8);
    } else if(!strncmp(command, "ADD", 3)) {
        sendMessageToClient(serverQueue, ADD, command + 4, thisClient, commandLength - 4);
    } else if(!strncmp(command, "DEL", 3)) {
        sendMessageToClient(serverQueue, DEL, command + 4, thisClient, commandLength - 4);
    } else if (strncmp(command, "2FRIENDS", 8) == 0) {
        sendMessageToClient(serverQueue, TO_FRIENDS, command + 9, thisClient, commandLength - 9);
    } else if(!strncmp(command, "LIST", 4)) {
        char* description = "Wyświetlenie listy klientów";
        sendMessageToClient(serverQueue, LIST, description, thisClient, strlen(description));
    } else if(!strncmp(command, "STOP", 4)) {
        exit(0);
    } else {
        printf("Podano nieprawidłową komendę: %s\n", command);
    }
}

void atExit() {
    if (PID == 0) {
        char* description = "Wiadomość zakończenia działania";
        
        mq_close(thisClientQueue);
        char thisQueuePath[15];
        sprintf(thisQueuePath, "/%d", getpid());
        mq_unlink(thisQueuePath);

        sendMessageToClient(serverQueue, STOP, description, thisClient, strlen(description));

        mq_close(serverQueue);
    }
    exit(1);
}

void handleSIGINT(int signum) {
    if (PID == 0) {
        printf("\nSIGINT - kończenie procesu klienta\n");
    }
    exit(0);
}

int main() {
    atexit(atExit);
    signal(SIGINT, handleSIGINT);

    thisClient = getpid();
    struct mq_attr messageAttributes;
    messageAttributes.mq_maxmsg = MAX_MESSAGES;
    messageAttributes.mq_msgsize = MESSAGE_SIZE;

    char thisClientPath[15];
    sprintf(thisClientPath, "/%d", thisClient);

    if((thisClientQueue = mq_open(thisClientPath, O_RDONLY | O_CREAT | O_EXCL, 0666, &messageAttributes)) == -1) {
        fprintf(stderr, "Wystąpił problem z połączeniem z kolejką serwera");
        exit(1);
    }

    if((serverQueue = mq_open(SERVER, O_WRONLY)) == -1) {
        fprintf(stderr, "Wystąpił problem z tworzeniem kolejki dla klienta");
        exit(1);
    }

    char* description = "Wiadomość nawiązania połączenia";
    sendMessageToClient(serverQueue, INIT, description, thisClient, strlen(description));
    Message message = receiveMessage(thisClientQueue);
    thisClient = message.clientID;

    printf("Klient numer: %d\n", thisClient);

    if ((PID = fork()) == 0) {
        while (1) {
            message = receiveMessage(thisClientQueue);
            if (message.type == ECHO){
                printf("Otrzymano wiadomość: \n  Typ: %s \n  Klient: %ld\n  Treść: %s\n", getTypeName(message.type), message.clientID, message.text);
            }
            if (message.type == STOP) {
                printf("Otrzymano wiadomość: \n  Typ: %s \n  Klient: %ld\n  Treść: %s\n", getTypeName(message.type), message.clientID, message.text);
                kill(getppid(), SIGUSR1);
                exit(0);
            }
        }
    } else {
        size_t MAX_COMMAND_LENGTH = MAX_MESSAGE_LENGTH + 15;
        char* command = calloc(MAX_COMMAND_LENGTH, sizeof(char));
        while (1) {
            size_t commandLength = getline(&command, &MAX_COMMAND_LENGTH, stdin);
            manageInputCommand(command, commandLength);
        }
        free(command);
    }
}