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
#include "chat.h"

int serverQueue;
int lastClientIndex = -1;
int clientsQueues[MAX_CLIENTS];
int lastFriendIndex = -1;
int friends[MAX_CLIENTS];

Message receiveMessage(int queue) {
    Message message;
    if(msgrcv(queue, &message, MESSAGE_SIZE, 0, 0)) {
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

    if (msgsnd(queue, &message, MESSAGE_SIZE, 0) == -1){
        fprintf(stderr, "Wysyłanie wiadomości do klienta nie powiodło się");
        exit(1);
    }
}

void manageInitMessage(Message message) {
    lastClientIndex++;
    if(lastClientIndex >= MAX_CLIENTS) {
        fprintf(stderr, "Przekroczono maksymalną liczbę klientów");
        exit(1);
    }
    if ((clientsQueues[lastClientIndex] = msgget(message.clientID, 0))) {
        sendMessageToClient(clientsQueues[lastClientIndex], INIT, "Wiadomośc nawiązania połączenia", lastClientIndex, 0);
    } else {
        fprintf(stderr, "Wystąpił problem z odnalezieniem kolejki klienta");
        exit(1);
    }
}

char* addClientAndTime(Message* message) {
    time_t now;
    time(&now);
    char* result = calloc(MAX_MESSAGE_LENGTH + 100, sizeof(char));

    sprintf(result, "%s    Czas: %s    Klient: %ld", message->text, ctime(&now), message->clientID);
    message->textLength = strlen(result);
    return result;
}

void manageEchoMessage(Message message) {
    char* messageText = addClientAndTime(&message);
    sendMessageToClient(clientsQueues[message.clientID], ECHO, messageText, 0, message.textLength);
    free(messageText);
}
            
void manageToOneMessage(Message message) {
    char* messageText = addClientAndTime(&message);
    sendMessageToClient(clientsQueues[message.clientID], ECHO, messageText, 0, message.textLength);
    free(messageText);
}

void manageListMessage(Message message) {
    char* result = calloc((lastClientIndex+1) * 50, sizeof(char));
    for(int i=0; i<=lastClientIndex; ++i) {
        char part[50];
        sprintf(part, "Klient: %d kolejka: %d | ", i, clientsQueues[i]);
        strcat(result, part);
    }
    sendMessageToClient(clientsQueues[message.clientID], ECHO, result, 0, (lastClientIndex+1) * 50);
}

void manageToAllMessage(Message message) {
    char* messageText = addClientAndTime(&message);
    for(int i=0; i<=lastClientIndex; ++i) {
        if(clientsQueues[i] != 0) {
            sendMessageToClient(clientsQueues[i], ECHO, messageText, 0, message.textLength);
        }
    }
    free(messageText);
}

void manageAddFriendsMessage(Message message) {
    char* friendsString = calloc(message.textLength, sizeof(char));
    memcpy(friendsString, message.text, message.textLength);
    char *friend = strtok(friendsString, " ");

    while(friend != NULL) {
        int alreadyInList = 0;
        int friendID = atoi(friend);
        lastFriendIndex++;

        for(int i = 0; i<lastFriendIndex; i++) {
            if(friends[i] == friendID) {
                alreadyInList = 1;
            }
        }

        if(alreadyInList) {
            fprintf(stderr, "Dodano dwóch jednakowych znajomych do listy");
            exit(1);
        }

        friends[lastFriendIndex] = friendID;
        
        if(lastClientIndex >= MAX_CLIENTS) {
            fprintf(stderr, "Dodano zbyt wielu znajomych do listy");
            exit(1);
        }

        friend = strtok(NULL, " ");
    }
    free(friendsString);
}

void manageFriendsMessage(Message message) {
    lastFriendIndex = -1;
    for(int i = 0; i<MAX_CLIENTS; i++) {
        friends[i] = -1;
    }
    
    manageAddFriendsMessage(message);
}

void manageDelFriendsMessage(Message message) {
    char* friendsString = calloc(message.textLength, sizeof(char));
    memcpy(friendsString, message.text, message.textLength);
    char *friend = strtok(friendsString, " ");

    while(friend != NULL) {
        int friendID = atoi(friend);
        for(int i=0; i<=lastFriendIndex; ++i) {
            if(friends[i] == friendID) {
                for(int j=i; j<lastFriendIndex; ++j) {
                    friends[j] = friends[j+1];
                }
                lastFriendIndex--;
            }
        }
        friend = strtok(NULL, " ");
    }
    free(friendsString);
}

void manageToFriendsMessage(Message message) {
    char* messageText = addClientAndTime(&message);
    for(int i=0; i<=lastFriendIndex; ++i) {
        if(clientsQueues[i] != 0) {
            sendMessageToClient(clientsQueues[friends[i]], ECHO, messageText, 0, message.textLength);
        }
    }
    free(messageText);
}

void normalizeQueue() {
    for(int i=0; i<=lastClientIndex; ++i) {
        if(clientsQueues[i] == 0) {
            for(int j=i; j<lastClientIndex; ++j) {
                clientsQueues[j] = clientsQueues[j+1];
            }
            lastClientIndex--;
        }
    }
}

void manageStopMessage(Message message) {
    clientsQueues[message.clientID] = 0;
    normalizeQueue();
}

void manageReceivedMessage(Message message) {
    switch (message.type) {
        case ECHO:
            manageEchoMessage(message);
            break;
        case LIST:
            manageListMessage(message);
            break;
        case FRIENDS:
            manageFriendsMessage(message);
            break;
        case ADD:
            manageAddFriendsMessage(message);
            break;
        case DEL:
            manageDelFriendsMessage(message);
            break;
        case TO_ALL:
            manageToAllMessage(message);
            break;
        case TO_FRIENDS:
            manageToFriendsMessage(message);
            break;
        case TO_ONE:
            manageToOneMessage(message);
            break;
        case STOP:
            manageStopMessage(message);
            break;
        case INIT:
            manageInitMessage(message);
            break;
    }
}

void atExit() {
    for (int i = 0; i<=lastClientIndex; ++i) {
        if (clientsQueues[i] != 0) {
            char* description = "Wiadomość zakończenia działania";
            sendMessageToClient(clientsQueues[i], STOP, description, 0, strlen(description));
            receiveMessage(serverQueue);
        }
    }
    msgctl(serverQueue, IPC_RMID, NULL);
}

void handleSIGINT(int signum) {
    printf("\nSIGINT - kończenie procesu serwera\n");
    exit(0);
}

int main() {
    atexit(atExit);
    signal(SIGINT, handleSIGINT);
    Message receivedMessage;

    key_t serverKey = ftok(getenv("HOME"), 'e');

    for(int i = 0; i<MAX_CLIENTS; i++){
        friends[i] = -1;
    }

    if ((serverQueue = msgget(serverKey, IPC_CREAT | 0666))) {
        printf("Wystartowano serwer o ID %d\n", serverQueue);

        while(1) {
            receivedMessage = receiveMessage(serverQueue);
            printf("Otrzymano wiadomość: \n  Typ: %s \n  Klient: %ld\n  Treść: %s\n", getTypeName(receivedMessage.type), receivedMessage.clientID, receivedMessage.text);
            manageReceivedMessage(receivedMessage);
        }
    } else {
        fprintf(stderr, "Wystąpił problem z tworzeniem kolejki dla serwera");
        exit(1);
    }
}